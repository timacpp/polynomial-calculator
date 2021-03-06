/** @file
  Implementation of a calculator polynomial processor.

  @author Tymofii Vedmedenko
  @copyright University of Warsaw
  @date 2021
*/


#include "errno.h"

#include "calc_poly.h"
#include "calc_error.h"
#include "../poly/io/poly_io.h"

void ProcessPolyInput(PolyStack* stack, int lineNumber) {
    errno = 0; /* The state could have been changed during the parsing. */

    Poly newPoly;
    bool successfulRead = ReadPoly(&newPoly);

    if (successfulRead) {
        PushPoly(stack, newPoly);
    } else {
        PrintError(WRONG_POLY, lineNumber);
    }
}