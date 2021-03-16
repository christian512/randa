
//-------------------------------------------------------------------------------//
// Author: Stefan Lörwald, Universität Heidelberg                                //
// License: CC BY-NC 4.0 http://creativecommons.org/licenses/by-nc/4.0/legalcode //
//-------------------------------------------------------------------------------//

#define COMPILE_TEMPLATE_JOB_MANAGER
#include "job_manager.h"
#undef COMPILE_TEMPLATE_JOB_MANAGER

//#define BENCHMARK_LOAD_BALANCING

#include <cassert>
#include <iostream>
#include <sstream>

#include "algorithm_row_operations.h"

using namespace panda;
template <typename Integer, typename TagType>
void panda::JobManager<Integer, TagType>::put(const Matrix<Integer>& matrix) const
{
   // TODO: Here call the put with vertices and vertex_maps
   rows.put(matrix);
   std::cerr << "WATCH OUT: Put new facets without equivalence check size \n";
}
template <typename Integer, typename TagType>
void panda::JobManager<Integer, TagType>::put(const Matrix<Integer>& matrix, const Vertices<Integer>& vertices, const VertexMaps& vertex_maps) const
{
   // TODO: Implement the equivalence check
   rows.put(matrix);
   std::cerr << "TODO: Implement the Equivalence check \n";
}

template <typename Integer, typename TagType>
void panda::JobManager<Integer, TagType>::put(const Row<Integer>& row) const
{
   rows.put(row);
   std::cerr << "WATCH OUT: Put new facets without equivalence check size \n";
}

template <typename Integer, typename TagType>
void panda::JobManager<Integer, TagType>::put(const Row<Integer>& row, const Vertices<Integer>& vertices, const VertexMaps& vertexMaps) const
{
   // TODO: Implement the Equivalence check.
   rows.put(row);
   std::cerr << "TODO: Implement the Equivalence check \n";
}

template <typename Integer, typename TagType>
Row<Integer> panda::JobManager<Integer, TagType>::get() const
{
   return rows.get();
}

#ifndef MPI_SUPPORT
   #pragma GCC diagnostic push
   #pragma GCC diagnostic ignored "-Wunused-parameter"
   #pragma clang diagnostic push
   #pragma clang diagnostic ignored "-Wunused-parameter"
#endif
template <typename Integer, typename TagType>
panda::JobManager<Integer, TagType>::JobManager(const Names& names_, const int number_of_processors, const int threads_per_processor)
:
   communication(),
   rows(names_),
   request_threads() // vital implementation detail: threads may access other members, hence, the threads must be destroyed first (Destruction in reverse order of construction).
{
   #ifdef MPI_SUPPORT
   assert( number_of_processors > 0 );
   assert( threads_per_processor > 0 );
   for ( int id = 1; id < number_of_processors; ++id ) // for all nodes except the master
   {
      for ( int i = 0; i < threads_per_processor; ++i )
      {
         request_threads.emplace_front([&,id]() // capture id by value, as it is a local variable
         {
            #ifdef BENCHMARK_LOAD_BALANCING
            std::size_t count(0);
            #endif
            while ( true )
            {
               const auto facet = rows.get();
               communication.toSlave(facet, id);
               if ( facet.empty() ) // if facet is empty, the slave will stop working.
               {
                  break;
               }
               #ifdef BENCHMARK_LOAD_BALANCING
               ++count;
               #endif
               const auto results = communication.fromSlave<Integer>(id);
               put(results);
            }
            #ifdef BENCHMARK_LOAD_BALANCING
            std::stringstream stream;
            stream << "Node " << id << ": " << count << '\n';
            std::cerr << stream.str();
            #endif
         });
      }
   }
   #endif // MPI_SUPPORT
}

template <typename Integer, typename TagType>
panda::JobManager<Integer, TagType>::JobManager(const Names& names_, const int number_of_processors, const int threads_per_processor, const Vertices<Integer>& vertices, const VertexMaps& vertex_maps)
        :
        communication(),
        rows(names_),
        request_threads(), // vital implementation detail: threads may access other members, hence, the threads must be destroyed first (Destruction in reverse order of construction).
        my_vertices(vertices),
        my_vertex_maps(vertex_maps)
{
   #ifdef MPI_SUPPORT
   assert( number_of_processors > 0 );
   assert( threads_per_processor > 0 );
   for ( int id = 1; id < number_of_processors; ++id ) // for all nodes except the master
   {
      for ( int i = 0; i < threads_per_processor; ++i )
      {
         request_threads.emplace_front([&,id]() // capture id by value, as it is a local variable
         {
            #ifdef BENCHMARK_LOAD_BALANCING
            std::size_t count(0);
            #endif
            while ( true )
            {
               const auto facet = rows.get();
               communication.toSlave(facet, id);
               if ( facet.empty() ) // if facet is empty, the slave will stop working.
               {
                  break;
               }
               #ifdef BENCHMARK_LOAD_BALANCING
               ++count;
               #endif
               const auto results = communication.fromSlave<Integer>(id);
               // Use the other put function
               put(results, vertices, vertex_maps);
            }
            #ifdef BENCHMARK_LOAD_BALANCING
            std::stringstream stream;
            stream << "Node " << id << ": " << count << '\n';
            std::cerr << stream.str();
            #endif
         });
      }
   }
   #endif // MPI_SUPPORT
}
#ifndef MPI_SUPPORT
   #pragma GCC diagnostic pop
   #pragma clang diagnostic pop
#endif

