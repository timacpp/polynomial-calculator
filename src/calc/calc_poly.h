/** @file
  Interface of a calculator polynomial processor.

  @author Tymofii Vedmedenko
  @copyright University of Warsaw
  @date 2021
*/

#ifndef POLYNOMIALS_CALC_POLY_H
#define POLYNOMIALS_CALC_POLY_H

#include "../poly/poly_stack.h"

/**
 * Signals calc in order to read a line representing a poly.
 * In case of successful parsing, the poly is placed on @p stack,
 * otherwise a custom calc error will be displayed.
 * @param[in] stack : stack with polynomials
 * @param[in] lineNumber : current ordinal of a line
 */
void ProcessPolyInput(PolyStack* stack, int lineNumber);

#endif //POLYNOMIALS_CALC_POLY_H
