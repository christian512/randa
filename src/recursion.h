
//-------------------------------------------------------------------------------//
// Author: Stefan Lörwald, Universität Heidelberg                                //
// License: CC BY-NC 4.0 http://creativecommons.org/licenses/by-nc/4.0/legalcode //
//-------------------------------------------------------------------------------//

#pragma once

namespace panda
{
   namespace recursion
   {
      /// Returns the maximum number of recursions to be made
      int maxNumRecursions(int, char**);
      /// Returns the minimum number of vertices to allow a recursive call
      int minNumVertices(int, char**);

   }
}

