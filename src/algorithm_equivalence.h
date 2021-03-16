
//-------------------------------------------------------------------------------//
// Author: Stefan Lörwald, Universität Heidelberg                                //
// License: CC BY-NC 4.0 http://creativecommons.org/licenses/by-nc/4.0/legalcode //
//-------------------------------------------------------------------------------//

#pragma once

#include "maps.h"

namespace panda
{
   namespace algorithm
   {
      /// Checks if two polytopes are equivalent under the given vertex maps
      /// IF under a vertex map the vertices of one polytope can be mapped to the vertices of the other
      /// both polytopes are considered equal
      bool equivalencePolyToPoly(const std::vector<int>&, const std::vector<int>&, const VertexMaps&);
   }
}

/// #include "algorithm_equivalence.eti"

