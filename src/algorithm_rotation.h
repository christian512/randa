
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
      Facets<Integer> rotation(const Vertices<Integer>&, const Facet<Integer>&, const Maps&, TagType);
      /// Returns all adjacent rows (or class representatives) of a row using a recursive rotation algorithm
      template <typename Integer, typename TagType>
      Facets<Integer> rotation_recursive(const Vertices<Integer>&, const Facet<Integer>&, const Maps&, TagType, int, int, int);
      /// Returns all faces for a given set of vertices and some faces (or class representatives) of a row using a recursive rotation algorithm
      template <typename Integer, typename TagType>
      Facets<Integer> getRidgesRecursive(const Vertices<Integer>&, const Facet<Integer>&, const Maps&, TagType, int, int, const Vertices<Integer>&, int);
       /// Returns  faces for a given set of vertices and some faces (or class representatives) of a row using a probabilistic recursive rotation algorithm
       template <typename Integer, typename TagType>
       Facets<Integer> getRidgesRecursiveProbabilistic(const Vertices<Integer>&, const Facet<Integer>&, const Maps&, TagType, int, int, const Vertices<Integer>&, int);
   }
}

#include "algorithm_rotation.eti"

