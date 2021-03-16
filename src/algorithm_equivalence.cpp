
//-------------------------------------------------------------------------------//
// Author: Stefan Lörwald, Universität Heidelberg                                //
// License: CC BY-NC 4.0 http://creativecommons.org/licenses/by-nc/4.0/legalcode //
//-------------------------------------------------------------------------------//

#define COMPILE_TEMPLATE_ALGORITHM_CLASSES

#include "algorithm_equivalence.h"

#undef COMPILE_TEMPLATE_ALGORITHM_CLASSES

#include <set>


using namespace panda;

bool panda::algorithm::equivalencePolyToPoly(std::set<int> indicesVertexPolyOne, std::set<int> indicesVertexPolyTwo,
                                             const VertexMaps &vertex_maps) {
   // Check the number of the vertices
   if (indicesVertexPolyOne.size() != indicesVertexPolyTwo.size()) {
      return false;
   }
   // As both lists are sets (i.e. ordered) we can use the equality operator
   if (indicesVertexPolyOne == indicesVertexPolyTwo) {
      return true;
   }
   for (const auto &vertex_map : vertex_maps) {
      auto newIndicesVertexPolyTwo = mapVerticesPolytope(indicesVertexPolyTwo, vertex_map);
      if (newIndicesVertexPolyTwo == indicesVertexPolyOne) {
         return true;
      }
   }
   return false;
}

std::set<int> panda::algorithm::mapVerticesPolytope(std::set<int> indicesVerticesPoly, const VertexMap &vertex_map) {
   for (int i = 0; i < vertex_map.size(); i++) {
      const auto pos = indicesVerticesPoly.find(i);
      if (pos != indicesVerticesPoly.end()) {
         indicesVerticesPoly.erase(i);
         indicesVerticesPoly.insert(vertex_map[i]);
      }
   }
   return indicesVerticesPoly;
}