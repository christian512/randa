
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

namespace {
   VertexMap vertexmap(std::istream&);
}

VertexMaps panda::input::implementation::vertexMaps(std::istream &stream) {
   VertexMaps vertex_maps;
   std::string token;
   if (!std::getline(stream, token) || !isKeywordVertexMaps(trimWhitespace(token))) {
      throw std::invalid_argument("Cannot read vertexMaps: file is at an invalid position.");
   }
   while (stream && !isKeyword(trimWhitespace(peekLine(stream)))) {
      if (trimWhitespace(peekLine(stream)).empty()) {
         std::getline(stream, token);
         continue;
      }
      try {
         auto vertex_map = ::vertexmap(stream);
         if (!vertex_map.empty()) {
            vertex_maps.push_back(vertex_map);
         }
         skipWhitespace(stream);
         skipEndOfLine(stream);
      } catch (const std::exception &e) {
         throw std::invalid_argument("The error " + std::string(e.what()) + " occured in vertexmap #" +
                                     std::to_string(vertex_maps.size() + 1) + ".");
      }
   }
   if (stream && isKeywordEnd(firstWord(peekLine(stream)))) {
      std::getline(stream, token);
   }
   return vertex_maps;
}

namespace {
   VertexMap vertexmap(std::istream& stream) {
      using namespace input;
      VertexMap vertex_map;
      // we assume that each entry has length of number of vertices provided
      skipWhitespace(stream);
      for (int tmp; stream >> tmp; )
      {
         vertex_map.push_back(tmp);
         skipWhitespace(stream);
         if ( stream.peek() == '\n' )
         {
            break;
         }
      }
      return vertex_map;
   }
}

