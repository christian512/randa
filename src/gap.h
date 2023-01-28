
//-------------------------------------------------------------------------------//
// Author: Stefan Lörwald, Universität Heidelberg                                //
// License: CC BY-NC 4.0 http://creativecommons.org/licenses/by-nc/4.0/legalcode //
//-------------------------------------------------------------------------------//

#pragma once

#include <vector>
#include <mutex>
#include "matrix.h"
#include "symmetries.h"
#include <map>


namespace panda
{
    class Gap
    {
    public:
        /// initializes GAP
        template <typename Integer>
        bool initialize(Symmetries, const Vertices<Integer>&);
        /// stops GAP
        bool stop() const;
        /// checks for equivalence using GAP
        std::vector<int> equivalence() const;
    private:
        mutable std::mutex mutex;
        const char *fifo_to_gap = "togap.pipe";
        const char *fifo_from_gap = "fromgap.pipe";
        const char *gap_prg_file = "gap_prg.g";
        bool running = false;
        std::map<std::string, int> vertex_lookup;


    private:
        /// write gap program to file
        bool write_gap_prg(Symmetries);
        /// execute GAP program
        void execute();
        /// convert a vertex to a string
        template <typename Integer>
        std::string vertex_to_string(const Vertex<Integer>&);
        /// fill the lookup table
        template <typename Integer>
        bool fill_lookup_table(const Vertices<Integer>&);
        /// lookup the index of a vertex
        template <typename Integer>
        int get_index(const Vertex<Integer>&);
    };
}

#include "gap.eti"