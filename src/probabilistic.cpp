
//-------------------------------------------------------------------------------//
// Author: Stefan Lörwald, Universität Heidelberg                                //
// License: CC BY-NC 4.0 http://creativecommons.org/licenses/by-nc/4.0/legalcode //
//-------------------------------------------------------------------------------//

#include "probabilistic.h"

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

int panda::probabilistic::probabilisticFlag(int argc, char** argv)
{
   assert( argc > 0 && argv != nullptr );
   for ( int i = 1; i < argc; ++i )
   {
      if ( std::strcmp(argv[i], "-p") == 0 )
      {
         if ( i + 1 == argc )
         {
            throw std::invalid_argument("Command line option \"-p\" needs 0 or 1 as parameter.");
         }
         return interpretParameter(argv[i + 1]);
      }
      else if ( std::strncmp(argv[i], "-p=", 3) == 0 )
      {
         return interpretParameter(argv[i] + 3);
      }
      else if ( std::strncmp(argv[i], "--probabilistic=", 16) == 0 )
      {
         return interpretParameter(argv[i] + 16);
      }
      else if ( std::strncmp(argv[i], "-p", 2) == 0 || std::strncmp(argv[i], "--p", 3) == 0 )
      {
         throw std::invalid_argument("Illegal parameter. Did you mean \"-p <0/1>\" or \"--probabilistic=<0/1>\"?");
      }
   }
   const auto default_value = 0;
   return default_value;
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
         std::string message = "Command line option \"-p <0/1>\" / \"--recursion=<0/1>\"";
         message += " needs 0 or 1 as parameter.";
         throw std::invalid_argument(message);
      }
      std::string rest;
      stream >> rest;
      if ( !rest.empty() )
      {
          std::string message = "Command line option \"-p <0/1>\" / \"--recursion=<0/1>\"";
          message += " needs 0 or 1 as parameter.";
          throw std::invalid_argument(message);
      }
      if ( n < 0 || n > 1 )
      {
          std::string message = "Command line option \"-p <0/1>\" / \"--recursion=<0/1>\"";
          message += " needs 0 or 1 as parameter.";
          throw std::invalid_argument(message);
      }
      return n;
   }
}

