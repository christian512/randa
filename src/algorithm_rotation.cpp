
//-------------------------------------------------------------------------------//
// Author: Stefan Lörwald, Universität Heidelberg                                //
// License: CC BY-NC 4.0 http://creativecommons.org/licenses/by-nc/4.0/legalcode //
//-------------------------------------------------------------------------------//

#define COMPILE_TEMPLATE_ALGORITHM_ROTATION

#include "algorithm_rotation.h"

#undef COMPILE_TEMPLATE_ALGORITHM_ROTATION

#include <algorithm>
#include <cassert>
#include <iostream>

#include "algorithm_classes.h"
#include "algorithm_fourier_motzkin_elimination.h"
#include "algorithm_inequality_operations.h"
#include "algorithm_integer_operations.h"
#include "algorithm_row_operations.h"

using namespace panda;

namespace {
    /// Rotates a facet around a ridge. It's the exact same algorithm as for vertices.
    template<typename Integer>
    Facet<Integer> rotate(const Vertices<Integer> &, Vertex<Integer>, const Facet<Integer> &, Facet<Integer>);

    /// Returns all ridges on a facet (equivalent to all facets of the facet).
    template<typename Integer>
    Inequalities<Integer> getRidges(const Vertices<Integer> &, const Facet<Integer> &);

    /// Returns all vertices that lie on the facet (satisfy the inequality with equality).
    template<typename Integer>
    Vertices<Integer> verticesWithZeroDistance(const Vertices<Integer> &, const Facet<Integer> &);

    /// Returns all indices of vertices that lie on the facet (satisfy the inequality with equality).
    template<typename Integer>
    std::vector<int> indicesVerticesWithZeroDistance(const Vertices<Integer> &, const Facet<Integer> &);
}

template<typename Integer, typename TagType>
Matrix<Integer> panda::algorithm::rotation(const Matrix<Integer> &matrix,
                                           const Row<Integer> &input,
                                           const Maps &maps,
                                           TagType tag) {
    // as the first step of the rotation, the furthest Vertex w.r.t. the input facet is calculated.
    // this will be the same vertex for all neighbouring ridges, hence, only needs to be computed once.
    const auto furthest_vertex = furthestVertex(matrix, input);
    const auto ridges = getRidges(matrix, input);
    std::set <Row<Integer>> output;
    for (const auto &ridge : ridges) {
        const auto new_row = rotate(matrix, furthest_vertex, input, ridge);
        output.insert(new_row);
    }
    return classes(output, maps, tag);
}

template<typename Integer, typename TagType>
Matrix<Integer> panda::algorithm::rotation_recursive(const Matrix<Integer> &matrix,
                                                     const Row<Integer> &input,
                                                     const Maps &maps,
                                                     TagType tag,
                                                     int curr_recursion_level,
                                                     int max_recursion_level) {
    const auto furthest_vertex = furthestVertex(matrix, input);
    const auto ridges = getRidgesRecursive(matrix, input, maps, tag, curr_recursion_level, max_recursion_level, matrix);
    std::set <Row<Integer>> output;
    for (const auto &ridge : ridges) {
        const auto new_row = rotate(matrix, furthest_vertex, input, ridge);
        // do equivalence check via GAP
        if (!equivalenceGAP(new_row, matrix, matrix, curr_recursion_level)) {
            output.insert(new_row);
        }

    }
    return classes(output, maps, tag);
}

template<typename Integer, typename TagType>
Matrix<Integer> panda::algorithm::getRidgesRecursive(const Matrix<Integer> &matrix,
                                                     const Row<Integer> &input,
                                                     const Maps &maps,
                                                     TagType tag,
                                                     int curr_recursion_level,
                                                     int max_recursion_level,
                                                     const Matrix<Integer> &all_vertices) {
    // if no recursion just give the ridges by FME
    if (curr_recursion_level == max_recursion_level) {
        return getRidges(matrix, input);
    }
    // get the vertices on the face and a single ridge
    const auto vertices_on_facet = verticesWithZeroDistance(matrix, input);
    auto ridges = algorithm::fourierMotzkinEliminationHeuristic(vertices_on_facet);
    // only consider a single ridge
    auto ridge = *ridges.begin();
    // get the subridges of this ridge
    const auto vertices_on_ridge = verticesWithZeroDistance(vertices_on_facet, ridge);
    auto subridges = algorithm::getRidgesRecursive(vertices_on_ridge, ridge, maps, tag, curr_recursion_level + 1,
                                                   max_recursion_level, all_vertices);
    // rotate the ridge around the subridges
    std::set <Row<Integer>> output;
    const auto furthest_vertex = furthestVertex(vertices_on_facet, ridge);
    for (const auto &subridge: subridges) {
        const auto new_ridge = rotate(vertices_on_facet, furthest_vertex, ridge, subridge);
        output.insert(new_ridge);
    }
    return classes(output, maps, tag);
}

namespace {
    template<typename Integer>
    Facet<Integer> rotate(const Vertices<Integer> &vertices, Vertex<Integer> vertex, const Facet<Integer> &facet,
                          Facet<Integer> ridge) {
        // the calculation of the initial vertex, which has to be the furthest vertex w.r.t. "facet", is calculated outside of this function as it is the same for all rotations.
        auto d_f = algorithm::distance(facet, vertex);
        auto d_r = algorithm::distance(ridge, vertex);
        do {
            const auto gcd_ds = algorithm::gcd(d_f, d_r);
            if (gcd_ds > 1) {
                d_f /= gcd_ds;
                d_r /= gcd_ds;
            }
            ridge = d_f * ridge - d_r * facet;
            const auto gcd_value = algorithm::gcd(ridge);
            assert(gcd_value != 0);
            if (gcd_value > 1) {
                ridge /= gcd_value;
            }
            vertex = algorithm::nearestVertex(vertices, ridge);
            d_f = algorithm::distance(facet, vertex);
            d_r = algorithm::distance(ridge, vertex);
        } while (d_r != 0);
        return ridge;
    }

    template<typename Integer>
    Inequalities<Integer> getRidges(const Vertices<Integer> &vertices, const Facet<Integer> &facet) {
        const auto vertices_on_facet = verticesWithZeroDistance(vertices, facet);
        assert(!vertices_on_facet.empty());
        return algorithm::fourierMotzkinElimination(vertices_on_facet);
    }

    template<typename Integer>
    Vertices<Integer> verticesWithZeroDistance(const Vertices<Integer> &vertices, const Facet<Integer> &facet) {
        Vertices<Integer> selection;
        std::copy_if(vertices.cbegin(), vertices.cend(), std::back_inserter(selection),
                     [&facet](const Vertex<Integer> &vertex) {
                         return (algorithm::distance(facet, vertex) == 0);
                     });
        return selection;
    }

    template<typename Integer>
    std::vector<int> indicesVerticesWithZeroDistance(const Vertices<Integer> &vertices, const Facet<Integer> &facet) {
        std::vector<int> selection;
        for (int i = 0; i < vertices.size(); i++) {
            if (algorithm::distance(facet, vertices[i]) == 0) {
                selection.push_back(i);
            }
        }
        return selection;
    }
}

