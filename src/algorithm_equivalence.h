
//-------------------------------------------------------------------------------//
// Author: Stefan Lörwald, Universität Heidelberg                                //
// License: CC BY-NC 4.0 http://creativecommons.org/licenses/by-nc/4.0/legalcode //
//-------------------------------------------------------------------------------//

#pragma once

#include "maps.h"
#include <set>

namespace panda
{
   namespace algorithm
   {
      /// Checks if two polytopes are equivalent under the given vertex maps
      /// IF under a vertex map the vertices of one polytope can be mapped to the vertices of the other
      /// both polytopes are considered equal
      bool equivalencePolyToPoly(std::set<int>, std::set<int>, const VertexMaps&);
      /// Function to map the indices of a polytope under a vertex map
      std::set<int> mapVerticesPolytope(std::set<int>, const VertexMap&);
   }
}

