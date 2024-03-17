
//-------------------------------------------------------------------------------//
// Author: Stefan Lörwald, Universität Heidelberg                                //
// License: CC BY-NC 4.0 http://creativecommons.org/licenses/by-nc/4.0/legalcode //
//-------------------------------------------------------------------------------//

#include "sampling.h"

#include <cassert>
#include <cstring>


using namespace panda;

bool panda::sampling::samplingFlag(int argc, char** argv)
{
    assert( argc > 0 && argv != nullptr );
    for ( int i = 1; i < argc; ++i )
    {
        // check if current flag is the sampling flag
        if ( std::strcmp(argv[i], "--probabilistic") == 0 )
        {
            return true;
        }
    }
    // return default value
    return false;
}

