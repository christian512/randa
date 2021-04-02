
//-------------------------------------------------------------------------------//
// Author: Stefan Lörwald, Universität Heidelberg                                //
// License: CC BY-NC 4.0 http://creativecommons.org/licenses/by-nc/4.0/legalcode //
//-------------------------------------------------------------------------------//

#define COMPILE_TEMPLATE_ALGORITHM_CLASSES
#include "algorithm_classes.h"
#undef COMPILE_TEMPLATE_ALGORITHM_CLASSES

#include <algorithm>
#include <cassert>
#include <cstddef>
#include <set>
#include <tuple>
#include <vector>
#include <numeric>
#include <iostream>
#include <fstream>

#include "algorithm_map_operations.h"
#include "algorithm_row_operations.h"

using namespace panda;

template <typename Integer, typename TagType>
Row<Integer> panda::algorithm::classRepresentative(const Row<Integer>& row, const Maps& maps, TagType tag)
{
   assert( !row.empty() );
   const auto matrix = getClass(row, maps, tag);
   assert( !matrix.empty() );
   return *matrix.crbegin(); // Important detail: last element is chosen as the representative
}

template <typename Integer, typename TagType>
std::set<Row<Integer>> panda::algorithm::getClass(const Row<Integer>& row, const Maps& maps, TagType tag)
{
   assert( !row.empty() );
   std::set<Row<Integer>> rows;
   rows.insert(row);
   using Iterator = typename std::set<Row<Integer>>::iterator;
   std::vector<Iterator> iterators;
   iterators.push_back(rows.begin());
   while ( !iterators.empty() )
   {
      const auto& current_row = *iterators.back();
      iterators.pop_back();
      for ( const auto& map : maps )
      {
         const auto new_row = apply(map, current_row, tag);
         Iterator iterator;
         bool inserted;
         std::tie(iterator, inserted) = rows.insert(new_row);
         if ( inserted )
         {
            iterators.push_back(iterator);
         }
      }
   }
   return rows;
}

template <typename Integer, typename TagType>
Matrix<Integer> panda::algorithm::classes(Matrix<Integer> input, const Maps& maps, TagType tag)
{
   std::set<Row<Integer>> rows(input.cbegin(), input.cend());
   return classes(rows, maps, tag);
}

template <typename Integer, typename TagType>
Matrix<Integer> panda::algorithm::classes(std::set<Row<Integer>> rows, const Maps& maps, TagType tag)
{
   Matrix<Integer> classes;
   while ( !rows.empty() )
   {
      const auto row_class = getClass(*rows.begin(), maps, tag);
      assert( !row_class.empty() );
      classes.push_back(*row_class.crbegin()); // Important detail: last element is chosen as the representative
      for ( const auto& row : row_class )
      {
         const auto position = rows.find(row);
         if ( position != rows.end() )
         {
            rows.erase(position);
         }
      }
   }
   return classes;
}

template <typename Integer>
bool panda::algorithm::equivalenceGAP(const Row<Integer>& row, const Vertices<Integer>& vertices, const Vertices<Integer>& all_vertices) {
    // calculate indices of vertices
    std::set<int> indices;
    for (int i = 0; i < vertices.size(); i++) {
        Integer dist = std::inner_product(vertices[i].cbegin(), vertices[i].cend(), row.cbegin(), Integer{0});
        if (dist == 0) {
            auto it = find(all_vertices.begin(), all_vertices.end(), vertices[i]);
            if ( it != all_vertices.end()){
                int idx = it - all_vertices.begin();
                indices.insert(idx);
            }
            else {
                std::cerr << "This should not happen! " << std::endl;
            }
        }
    }
    std::string s;
    s += "[";
    for (auto const &e: indices) {
        s += std::to_string(e + 1);
        s += ',';
    }
    s.pop_back();
    s += ']';
    // Connect to GAP Server using FIFO named pipes
    std::ifstream in("/home/chris/fromgap.pipe");
    std::ofstream out("/home/chris/togap.pipe");
    std::string line;
    // write the vertices on face
    out << s << std::endl;
    std::getline(in, line);
    if (line == "true") {
        return true;
    } else {
        return false;
    }
}

template <typename Integer>
std::set<int> panda::algorithm::indicesVerticesOnFace(const Row<Integer>& facet, const Vertices<Integer>& vertices)  {
    std::set<int> selection;
    for (int i = 0; i < vertices.size(); i++) {
        Integer dist = std::inner_product(vertices[i].cbegin(), vertices[i].cend(), facet.cbegin(), Integer{0});
        if (dist == 0) {
            selection.insert(i);
        }
    }
    return selection;
}
