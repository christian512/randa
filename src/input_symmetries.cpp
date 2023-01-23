
//-------------------------------------------------------------------------------//
// Author: Stefan Lörwald, Universität Heidelberg                                //
// License: CC BY-NC 4.0 http://creativecommons.org/licenses/by-nc/4.0/legalcode //
//-------------------------------------------------------------------------------//

#include <string>

#include "input_symmetries.h"
#include "symmetries.h"

using namespace panda;

namespace
{
   Symmetry symmetry(std::istream&);
   bool contains_valid_chars(std::string);
}

Symmetries panda::input::implementation::symmetries(std::istream& stream)
{
   Symmetries symmetries;
   while ( stream )
   {
      const auto symmetry = ::symmetry(stream);
      if (symmetry.find_first_not_of("0123456789(,) ") == std::string::npos)
      {
         symmetries.push_back(symmetry);
      }
   }
   return symmetries;
}

namespace
{

   using namespace input;
   Symmetry symmetry(std::istream& stream)
   {
       // Get line from the stream
       std::string line;
       std::getline(stream, line);
       // TODO: Maybe I have to cutoff the \n in the end here.
       return line;
   }
}

