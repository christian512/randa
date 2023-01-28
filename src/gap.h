
//-------------------------------------------------------------------------------//
// Author: Stefan Lörwald, Universität Heidelberg                                //
// License: CC BY-NC 4.0 http://creativecommons.org/licenses/by-nc/4.0/legalcode //
//-------------------------------------------------------------------------------//

#include <vector>
#include <mutex>
#include "matrix.h"
#include "symmetries.h"
#pragma once

namespace panda
{
    class Gap
    {
    public:
        /// initializes GAP
        bool initialize(Symmetries);
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


    private:
        /// write gap program to file
        // TODO: Implement symmetries to write them to the GAP file
        bool write_gap_prg(Symmetries);
        /// execute GAP program
        void execute();


    };
}

