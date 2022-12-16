
//-------------------------------------------------------------------------------//
// Author: Stefan Lörwald, Universität Heidelberg                                //
// License: CC BY-NC 4.0 http://creativecommons.org/licenses/by-nc/4.0/legalcode //
//-------------------------------------------------------------------------------//

#define COMPILE_TEMPLATE_ALGORITHM_CLASSES

#include "algorithm_equivalence.h"

#undef COMPILE_TEMPLATE_ALGORITHM_CLASSES

#include <set>
#include <numeric>
#include <iostream>
#include <fstream>
#include <unistd.h>
#include <stdio.h>


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
   // TODO: Add docs for this function
    for (int i = 0; i < vertex_map.size(); i++) {
      const auto pos = indicesVerticesPoly.find(i);
      if (pos != indicesVerticesPoly.end()) {
         indicesVerticesPoly.erase(i);
         indicesVerticesPoly.insert(vertex_map[i]);
      }
   }
   return indicesVerticesPoly;
}


template <typename Integer>
bool panda::algorithm::equivalenceGAP(const Row<Integer>& row, const Vertices<Integer>& vertices) {
    // calculate indices of vertices
    std::set<int> indices = indicesVerticesOnFace(row, vertices);
    std::string s;
    s += "[";
    for (auto const &e: indices) {
        s += std::to_string(e + 1);
        s += ',';
    }
    s.pop_back();
    s += ']';
    // Connect to GAP Server using FIFO named pipes
    std::string cwd = get_current_dir_name();
    std::ifstream in(cwd + "/fromgap.pipe");
    std::ofstream out(cwd + "/togap.pipe");
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

template <typename Integer, typename TagType>
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