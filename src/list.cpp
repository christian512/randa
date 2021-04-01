
//-------------------------------------------------------------------------------//
// Author: Stefan Lörwald, Universität Heidelberg                                //
// License: CC BY-NC 4.0 http://creativecommons.org/licenses/by-nc/4.0/legalcode //
//-------------------------------------------------------------------------------//

#define COMPILE_TEMPLATE_LIST
#include "list.h"
#undef COMPILE_TEMPLATE_LIST

#include <cstddef>
#include <iostream>
#include <sstream>
#include <fstream>
#include <algorithm>
#include <numeric>


#include "algorithm_row_operations.h"
#include "algorithm_equivalence.h"

using namespace panda;

#define PRINT_DONE_COUNTER /// if enabled, the beginning of processing a row will be announced.

#ifdef PRINT_DONE_COUNTER
   #include "cpp_feature_check_thread_local.h"
   #if HAS_FEATURE_THREAD_LOCAL != 0
      namespace
      {
         thread_local std::size_t index;
      }
   #else
      #include <map>
      #include <thread>
      namespace
      {
         std::map<std::thread::id, std::size_t> indices;
      }
   #endif
#endif

template <typename Integer, typename TagType>
void panda::List<Integer, TagType>::put(const Matrix<Integer>& matrix) const
{
   for ( const auto& row : matrix )
   {
      put(row);
   }
   std::lock_guard<std::mutex> lock(mutex);
   --workers;
   #ifdef PRINT_DONE_COUNTER
   #if HAS_FEATURE_THREAD_LOCAL == 0
   auto index = indices[std::this_thread::get_id()];
   #endif
   if ( index > 0 )
   {
      std::stringstream stream;
      stream << "Done processing #" << index << '\n';
      std::cerr << stream.str();
   }
   #endif
}

template <typename Integer, typename TagType>
void panda::List<Integer, TagType>::put(const Matrix<Integer>& matrix, const Vertices<Integer>& vertices, const VertexMaps& vertex_maps) const
{
   for ( const auto& row : matrix )
   {
      put(row, vertices, vertex_maps);
   }
   std::lock_guard<std::mutex> lock(mutex);
   --workers;
   #ifdef PRINT_DONE_COUNTER
   #if HAS_FEATURE_THREAD_LOCAL == 0
   auto index = indices[std::this_thread::get_id()];
   #endif
   if ( index > 0 )
   {
      std::stringstream stream;
      stream << "Done processing #" << index << '\n';
      std::cerr << stream.str();
   }
   #endif
}


template <typename Integer, typename TagType>
void panda::List<Integer, TagType>::put(const Row<Integer>& row) const
{
   std::lock_guard<std::mutex> lock(mutex);
   Iterator it;
   bool added;
   std::tie(it, added) = rows.insert(row);
   if ( added )
   {
      if ( std::is_same<TagType, tag::facet>::value )
      {
         algorithm::prettyPrintln(std::cout, row, names, "<=");
      }
      else
      {
         std::cout << row << '\n';
      }
      std::cout.flush();
      iterators.push_back(it);
      condition.notify_one();
   }
}
template <typename Integer, typename TagType>
void panda::List<Integer, TagType>::put(const Row<Integer>& row, const Vertices<Integer>& vertices, const VertexMaps& vertex_maps) const
{
   std::lock_guard<std::mutex> lock(mutex);
   Iterator it;
   bool added;
   bool equiv = false;
   std::set<int> indicesVerticesPolytopeOne;
   // It would be better to use functions from algorithm_equivalence, but I don't know how to do that.
   // So we implement the functions here.
   if (vertex_maps.size() > 0)
   {
      indicesVerticesPolytopeOne = indicesVerticesOnFace(row, vertices);
      for (const auto& poly : all_polys)
      {
         // check equivalence of the two polytopes
         if ( equivalencePolyToPoly(indicesVerticesPolytopeOne, poly, vertex_maps)){
            equiv = true;
            break;
         }
      }
   }
   // Perform equivalence check with GAP server
   std::ifstream in("/home/chris/fromgap.pipe");
   std::ofstream out("/home/chris/togap.pipe");
   // writing / reading
   indicesVerticesPolytopeOne = indicesVerticesOnFace(row, vertices);
   std::string s;
   s += '[';
   for (auto const& e : indicesVerticesPolytopeOne)
   {
       // std::cerr << e+1 << std::endl;
       s += std::to_string(e+1);
       s += ',';
   }
   s.pop_back();
   s += ']';
   out << s << std::endl;
   out.flush();
   std::string line;
   std::getline(in, line);
   if (line == "true"){
       equiv = true;
   }
   if ( !equiv ) {
      std::tie(it, added) = rows.insert(row);
   } else {
      added = false;
   }

   if ( added && !equiv)
   {
      if (vertex_maps.size() > 0)
      {
         all_polys.push_back(indicesVerticesPolytopeOne);
      }
      if ( std::is_same<TagType, tag::facet>::value )
      {
         algorithm::prettyPrintln(std::cout, row, names, "<=");
      }
      else
      {
         std::cout << row << '\n';
      }
      std::cout.flush();
      iterators.push_back(it);
      condition.notify_one();
   }
}
template <typename Integer, typename TagType>
Row<Integer> panda::List<Integer, TagType>::get() const
{
   if ( empty() ) // abort
   {
      const auto it = rows.insert(Row<Integer>{}).first;
      std::unique_lock<std::mutex> lock(mutex);
      iterators.push_back(it);
      condition.notify_all();
   }
   std::unique_lock<std::mutex> lock(mutex);
   condition.wait(lock, [&](){ return !iterators.empty(); });
   ++workers;
   const auto row = *iterators.front();
   if ( !row.empty() )
   {
      iterators.erase(iterators.begin());
   }
   #ifdef PRINT_DONE_COUNTER
   if ( !row.empty() )
   {
      ++counter;
      #if HAS_FEATURE_THREAD_LOCAL == 0
      indices[std::this_thread::get_id()] = counter;
      #else
      index = counter;
      #endif
      std::stringstream stream;
      stream << "Processing #" << counter << " of at least " << rows.size();
      stream << " class" << ((rows.size() == 1) ? "" : "es") << '\n';
      std::cerr << stream.str();
   }
   #endif
   return row;
}

template <typename Integer, typename TagType>
panda::List<Integer, TagType>::List(const Names& names_)
:
   names(names_),
   mutex(),
   workers(1),
   condition(),
   rows(),
   iterators(),
   counter(0)
{
}

template <typename Integer, typename TagType>
bool panda::List<Integer, TagType>::empty() const
{
   const std::lock_guard<std::mutex> lock(mutex);
   return workers == 0 && iterators.empty();
}

template <typename Integer, typename TagType>
std::set<int> panda::List<Integer, TagType>::indicesVerticesOnFace(const Row<Integer>& facet, const Vertices<Integer>& vertices) const
{
   std::set<int> selection;
   for ( int i = 0; i < vertices.size(); i++){
      Integer dist = std::inner_product(vertices[i].cbegin(), vertices[i].cend(), facet.cbegin(), Integer{0});
      if ( dist == 0) {
         selection.insert(i);
      }
   }
   return selection;
}
template <typename Integer, typename TagType>
bool panda::List<Integer, TagType>::equivalencePolyToPoly(const std::set<int>& indicesVerticesPolyOne, const std::set<int>& indicesVerticesPolyTwo,
                                                          const VertexMaps& vertex_maps) const{

         // check for the same number of vertices
         if (indicesVerticesPolyOne.size() != indicesVerticesPolyTwo.size()) {
            return false;
         }
         // since we are using sets we can simply check for equivalence
         if ( indicesVerticesPolyOne == indicesVerticesPolyTwo) {
            return true;
         }
         for ( const auto& vertex_map : vertex_maps){
            auto newIndicesVerticesPolyTwo = mapVerticesPolytope(indicesVerticesPolyTwo, vertex_map);
            if (indicesVerticesPolyOne == newIndicesVerticesPolyTwo) {
               return true;
            }
         }
         return false;
      }

template <typename Integer, typename TagType>
std::set<int> panda::List<Integer, TagType>::mapVerticesPolytope(std::set<int> indicesVerticesPolytope, const VertexMap& vertex_map) const {
         std::set<int> newIndices;
         for (int i = 0; i < vertex_map.size(); i++) {
            const auto pos = indicesVerticesPolytope.find(i);
            if (pos != indicesVerticesPolytope.end()) {
               newIndices.insert(vertex_map[i]);
            }
         }
         return newIndices;
      }