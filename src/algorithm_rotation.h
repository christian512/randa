
//-------------------------------------------------------------------------------//
// Author: Stefan Lörwald, Universität Heidelberg                                //
// License: CC BY-NC 4.0 http://creativecommons.org/licenses/by-nc/4.0/legalcode //
//-------------------------------------------------------------------------------//

#pragma once

#include "maps.h"
#include "matrix.h"
#include "row.h"
#include "tags.h"

namespace panda
{
   namespace algorithm
   {
      /// Returns all adjacent rows (or class representatives) of a row by using the rotation algorithm.
      template <typename Integer, typename TagType>
      Facets<Integer> rotation(const Vertices<Integer>&, const Facet<Integer>&, const Maps&, TagType, int, int, bool);
      /// Returns all vertices that lie on the facet (satisfy the inequality with equality).
      template <typename Integer>
      Vertices<Integer> verticesWithZeroDistance(const Vertices<Integer>&, const Facet<Integer>&);
   }
}

#include "algorithm_rotation.eti"

