
//-------------------------------------------------------------------------------//
// Author: Stefan Lörwald, Universität Heidelberg                                //
// License: CC BY-NC 4.0 http://creativecommons.org/licenses/by-nc/4.0/legalcode //
//-------------------------------------------------------------------------------//

#pragma once

#include <set>

#include "maps.h"
#include "matrix.h"
#include "row.h"
#include "tags.h"

namespace panda
{
   namespace algorithm
   {
      /// returns the unique class representative. The class is generated with the provided maps.
      /// Precondition: if input is a facet, the facet must be normalized.
      template <typename Integer, typename TagType>
      Row<Integer> classRepresentative(const Row<Integer>&, const Maps&, TagType);
      /// Creates a set of rows which is the complete class containing the input row.
      /// Precondition: if input is a facet, the facet must be normalized.
      template <typename Integer, typename TagType>
      std::set<Row<Integer>> getClass(const Row<Integer>&, const Maps&, TagType);
      /// Reduces a list of rows to just the representatives.
      /// Precondition: if input are facets, then these facets must be normalized.
      template <typename Integer, typename TagType>
      Matrix<Integer> classes(Matrix<Integer>, const Maps&, TagType);
      /// Reduces a list of rows to just the representatives.
      /// Precondition: if input are facets, then these facets must be normalized.
      template <typename Integer, typename TagType>
      Matrix<Integer> classes(std::set<Row<Integer>>, const Maps&, TagType);
      // Runs equivalence check using a running GAP server
      template <typename Integer>
      bool equivalenceGAP(const Row<Integer>&, const Vertices<Integer>&, const Vertices<Integer>&, int);
      // Runs the equivalence check by sending multiple polytopes to GAP server
      template <typename Integer>
      Matrix<Integer> equivalenceGAPList(const Matrix<Integer>&, const Matrix<Integer>&, const Vertices<Integer>&, const Vertices<Integer>&, int);
      // Calculates the indices of the vertices that are on the face
      template <typename Integer>
      std::set<int> indicesVerticesOnFace(const Row<Integer>&, const Vertices<Integer>&);
   }
}

#include "algorithm_classes.eti"

