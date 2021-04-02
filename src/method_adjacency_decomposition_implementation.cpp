
//-------------------------------------------------------------------------------//
// Author: Stefan Lörwald, Universität Heidelberg                                //
// License: CC BY-NC 4.0 http://creativecommons.org/licenses/by-nc/4.0/legalcode //
//-------------------------------------------------------------------------------//

#define COMPILE_TEMPLATE_METHOD_ADJACENCY_DECOMPOSITION_IMPLEMENTATION
#include "method_adjacency_decomposition_implementation.h"
#undef COMPILE_TEMPLATE_METHOD_ADJACENCY_DECOMPOSITION_IMPLEMENTATION

#include <algorithm>
#include <cassert>
#include <future>
#include <iostream>
#include <list>

#include "algorithm_classes.h"
#include "algorithm_fourier_motzkin_elimination.h"
#include "algorithm_map_operations.h"
#include "algorithm_matrix_operations.h"
#include "algorithm_rotation.h"
#include "algorithm_row_operations.h"
#include "concurrency.h"
#include "recursion.h"
#include "joining_thread.h"
#include "message_passing_interface_session.h"

using namespace panda;

namespace
{
   template <typename Integer>
   std::pair<Equations<Integer>, Maps> reduce(const JobManager<Integer, tag::facet>&, const std::tuple<Matrix<Integer>, Names, Maps, Matrix<Integer>, VertexMaps>& data);

   template <typename Integer>
   std::pair<Equations<Integer>, Maps> reduce(const JobManagerProxy<Integer, tag::facet>&, const std::tuple<Matrix<Integer>, Names, Maps, Matrix<Integer>, VertexMaps>& data);

   template <typename Integer, template <typename, typename> class JobManagerType>
   std::pair<Equations<Integer>, Maps> reduce(const JobManagerType<Integer, tag::vertex>&, const std::tuple<Matrix<Integer>, Names, Maps, Matrix<Integer>, VertexMaps>& data);

   template <typename Integer>
   std::future<void> initializePool(JobManager<Integer, tag::facet>&, const Matrix<Integer>&, const Maps&, const Matrix<Integer>&, const Equations<Integer>&);

   template <typename Integer>
   std::future<void> initializePool(JobManager<Integer, tag::vertex>&, const Matrix<Integer>&, const Maps&, const Matrix<Integer>&, const Equations<Integer>&);

   template <typename Integer, typename TagType>
   std::future<void> initializePool(JobManagerProxy<Integer, TagType>&, const Matrix<Integer>&, const Maps&, const Matrix<Integer>&, const Equations<Integer>&);
}

template <template <typename, typename> class JobManagerType, typename Integer, typename TagType>
void panda::implementation::adjacencyDecomposition(int argc, char** argv, const std::tuple<Matrix<Integer>, Names, Maps, Matrix<Integer>, VertexMaps>& data,TagType tag)
{
   const auto node_count = mpi::getSession().getNumberOfNodes();
   const auto thread_count = concurrency::numberOfThreads(argc, argv);
   // Get the maximum recursion level from the function call
   const auto max_recursion_depth = recursion::recursionDepth(argc, argv);
   int curr_recursion_depth = 0;
   const auto& input = std::get<0>(data);
   const auto& names = std::get<1>(data);
   const auto& known_output = std::get<3>(data);
   const auto& vertex_maps = std::get<4>(data);
   // Here the Jobmanager is initialized with the vertices (i.e. input) and the symmetries (i.e. vertex maps).
   JobManagerType<Integer, TagType> job_manager(names, node_count, thread_count, input, vertex_maps);
   const auto reduced_data = reduce(job_manager, data);
   const auto& equations = std::get<0>(reduced_data);
   const auto& maps = std::get<1>(reduced_data);
   std::list<JoiningThread> threads;
   auto future = initializePool(job_manager, input, maps, known_output, equations);
   for ( int i = 0; i < thread_count; ++i )
   {
      threads.emplace_front([&]()
      {
         while ( true )
         {
            const auto job = job_manager.get();
            if ( job.empty() )
            {
               break;
            }
            const auto jobs = algorithm::rotation_recursive(input, job, maps, tag, curr_recursion_depth, max_recursion_depth);
            job_manager.put(jobs);
         }
      });
   }
   future.wait();
}

namespace
{
   template <typename Integer, typename Callable>
   std::pair<Equations<Integer>, Maps> reduce(const std::tuple<Matrix<Integer>, Names, Maps, Matrix<Integer>, VertexMaps>& data, Callable&& callable)
   {
      const auto& vertices = std::get<0>(data);
      const auto& names = std::get<1>(data);
      const auto& original_maps = std::get<2>(data);
      const auto equations = algorithm::extractEquations(vertices);
      if ( !equations.empty() )
      {
         callable(equations, names);
         const auto maps = algorithm::normalize(original_maps, equations);
         return std::make_pair(equations, maps);
      }
      return std::make_pair(equations, original_maps);
   }

   template <typename Integer>
   std::pair<Equations<Integer>, Maps> reduce(const JobManager<Integer, tag::facet>&, const std::tuple<Matrix<Integer>, Names, Maps, Matrix<Integer>, VertexMaps>& data)
   {
      return reduce(data, [](const Matrix<Integer>& equations, const Names& names)
      {
         std::cout << "Equations:\n";
         algorithm::prettyPrint(std::cout, equations, names, "=");
         std::cout << '\n';
      });
   }

   template <typename Integer>
   std::pair<Equations<Integer>, Maps> reduce(const JobManagerProxy<Integer, tag::facet>&, const std::tuple<Matrix<Integer>, Names, Maps, Matrix<Integer>, VertexMaps>& data)
   {
      return reduce(data, [](const Matrix<Integer>&, const Names&) {});
   }

   template <typename Integer, template <typename, typename> class JobManagerType>
   std::pair<Equations<Integer>, Maps> reduce(const JobManagerType<Integer, tag::vertex>&, const std::tuple<Matrix<Integer>, Names, Maps, Matrix<Integer>, VertexMaps>& data)
   {
      const auto& original_maps = std::get<2>(data);
      return std::make_pair(Equations<Integer>{}, original_maps);
   }

   template <typename Integer, typename TagType>
   std::future<void> initializationOnMaster(JobManager<Integer, TagType>& manager, const Matrix<Integer>& matrix, const Maps& maps, const Matrix<Integer>& known_output, const Equations<Integer>& equations, const std::string& type_string)
   {
      assert ( (!std::is_same<TagType, tag::vertex>::value || equations.empty()) );
      if ( !maps.empty() )
      {
         std::cout << "Reduced ";
      }
      std::cout << type_string << ":\n";
      // Initialize the process (so that other processes start).
      if ( !known_output.empty() )
      {
         auto tmp = algorithm::normalize(known_output.front(), equations);
         tmp = algorithm::classRepresentative(tmp, maps, TagType{});
         manager.put(Matrix<Integer>{tmp});
      }
      else
      {
         auto facets = algorithm::fourierMotzkinEliminationHeuristic(matrix);
         for ( auto& facet : facets )
         {
            if ( !algorithm::equivalenceGAP(facet, matrix, matrix, 0)){
                manager.put(facet);
            }
         }
      }
      // Add the remaining known facets from file asynchronously.
      auto future = std::async(std::launch::async, [&]()
      {
         for ( const auto& facet : known_output )
         {
            auto tmp = algorithm::normalize(facet, equations);
            tmp = algorithm::classRepresentative(tmp, maps, TagType{});
            manager.put(tmp);
         }
      });
      return future;
   }

   template <typename Integer>
   std::future<void> initializePool(JobManager<Integer, tag::facet>& manager, const Matrix<Integer>& matrix, const Maps& maps, const Matrix<Integer>& known_output, const Equations<Integer>& equations)
   {
      return initializationOnMaster(manager, matrix, maps, known_output, equations, "Inequalities");
   }

   template <typename Integer>
   std::future<void> initializePool(JobManager<Integer, tag::vertex>& manager, const Matrix<Integer>& matrix, const Maps& maps, const Matrix<Integer>& known_output, const Equations<Integer>&)
   {
      return initializationOnMaster(manager, matrix, maps, known_output, {}, "Vertices / Rays");
   }

   template <typename Integer, typename TagType>
   std::future<void> initializePool(JobManagerProxy<Integer, TagType>&, const ConvexHull<Integer>&, const Maps&, const Inequalities<Integer>&, const Equations<Integer>&)
   {
      // only the manager on the root node performs a heuristic to get initial facets.
      auto future = std::async(std::launch::async, [](){});
      return future;
   }
}

