
//-------------------------------------------------------------------------------//
// Author: Stefan Lörwald, Universität Heidelberg                                //
// License: CC BY-NC 4.0 http://creativecommons.org/licenses/by-nc/4.0/legalcode //
//-------------------------------------------------------------------------------//

#include "input_vertexmap.h"

#include <algorithm>
#include <stdexcept>
#include <string>

#include "input_common.h"
#include "input_keywords.h"
#include "istream_peek_line.h"

using namespace panda;

namespace
{
   VertexMap vertexmap(std::istream&, const Names&);
}

VertexMaps panda::input::implementation::vertexMaps(std::istream& stream)
{
   VertexMaps vertex_maps;
   return vertex_maps;
}

namespace
{
/* TODO: To be deleted
   /// Parses a string of the form x1-2x3+4x4-1 or similar.
   Image image(std::string token, const Names& names)
   {
      if ( token.empty() )
      {
         throw std::invalid_argument("Invalid token (is empty).");
      }
      Image result;
      std::size_t position = 0;
      while ( position != std::string::npos && position < token.size() )
      {
         const auto factor = extractFactor(token, position);
         const auto variable = extractName(token, position);
         if ( variable.empty() )
         {
            // not referring to a name => index == number of names
            result.push_back(std::make_pair(names.size(), factor));
         }
         else
         {
            const auto iterator = std::find(names.cbegin(), names.cend(), variable);
            if ( iterator == names.cend() )
            {
               throw std::invalid_argument("In \"" + token + "\" of a map, the token \"" + variable + "\" is not a variable.");
            }
            const auto index = iterator - names.cbegin();
            result.push_back(std::make_pair(index, factor));
         }
      }
      return result;
   }
*/
   VertexMap vertexMap(std::istream& stream)
   {
      VertexMap vertex_map;
      return vertex_map;
   }
}

