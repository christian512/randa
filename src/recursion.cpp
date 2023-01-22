
//-------------------------------------------------------------------------------//
// Author: Stefan Lörwald, Universität Heidelberg                                //
// License: CC BY-NC 4.0 http://creativecommons.org/licenses/by-nc/4.0/legalcode //
//-------------------------------------------------------------------------------//

#include "recursion.h"

#include <cassert>
#include <cstdlib>
#include <cstring>
#include <sstream>
#include <stdexcept>
#include <string>
#include <thread>

using namespace panda;

namespace
{
   /// Tries to read a number from char*.
   int interpretParameter(char*);
}

int panda::recursion::maxNumRecursions(int argc, char** argv)
{
    assert( argc > 0 && argv != nullptr );
    for ( int i = 1; i < argc; ++i )
    {
        if ( std::strcmp(argv[i], "-r") == 0 )
        {
            if ( i + 1 == argc )
            {
                throw std::invalid_argument("Command line option \"-r <n>\" needs an integral parameter.");
            }
            return interpretParameter(argv[i + 1]);
        }
        else if ( std::strncmp(argv[i], "-r=", 3) == 0 )
        {
            return interpretParameter(argv[i] + 3);
        }
        else if ( std::strncmp(argv[i], "--recursion_levels=", 19) == 0 )
        {
            return interpretParameter(argv[i] + 10);
        }
        else if ( std::strncmp(argv[i], "-r", 2) == 0 || std::strncmp(argv[i], "--r", 3) == 0 )
        {
            throw std::invalid_argument("Illegal parameter. Did you mean \"-r <n>\" or \"--recursion_levels=<n>\"?");
        }
    }
    // return default value
    return 0;
}

int panda::recursion::minNumVertices(int argc, char** argv)
{
    assert( argc > 0 && argv != nullptr );
    for ( int i = 1; i < argc; ++i )
    {
        if ( std::strcmp(argv[i], "-d") == 0 )
        {
            if ( i + 1 == argc )
            {
                throw std::invalid_argument("Command line option \"-d <n>\" needs an integral parameter.");
            }
            return interpretParameter(argv[i + 1]);
        }
        else if ( std::strncmp(argv[i], "-d=", 3) == 0 )
        {
            return interpretParameter(argv[i] + 3);
        }
        else if ( std::strncmp(argv[i], "--recursion_min_numvertices=", 27) == 0 )
        {
            return interpretParameter(argv[i] + 10);
        }
        else if ( std::strncmp(argv[i], "-d", 2) == 0 || std::strncmp(argv[i], "--d", 3) == 0 )
        {
            throw std::invalid_argument("Illegal parameter. Did you mean \"-r <n>\" or \"--recursion_min_numvertices=<n>\"?");
        }
    }
    // return default value
    return 0;
}

namespace
{
   int interpretParameter(char* string)
   {
      assert( string != nullptr );
      std::istringstream stream(string);
      int n;
      if ( !(stream >> n) )
      {
         std::string message = "Command line option \"-d <n>\" ";
         message += " needs an integral parameter.";
         throw std::invalid_argument(message);
      }
      std::string rest;
      stream >> rest;
      if ( !rest.empty() )
      {
         std::string message = "Command line option \"-d <n>\" ";
         message += " needs an integral parameter.";
         throw std::invalid_argument(message);
      }
      if ( n <= 0 )
      {
         std::string message = "Command line option \"-d <n>\" ";
         message += " needs an integral parameter greater zero.";
         throw std::invalid_argument(message);
      }
      return n;
   }
}

