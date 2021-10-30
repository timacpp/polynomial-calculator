/** @file
  Interface of a calculator polynomial processor.

  @date 2021
*/

#ifndef POLYNOMIALS_CALC_POLY_H
#define POLYNOMIALS_CALC_POLY_H

#include "../polynomial/poly_stack.h"

/**
 * Signals calculator in order to read a line representing a polynomial.
 * In case of successful parsing, the polynomial is placed on @p stack,
 * otherwise a custom calculator error will be displayed.
 * @param[in] stack : stack with polynomials
 * @param[in] lineNumber : current ordinal of a line
 */
void ProcessPolyInput(PolyStack* stack, int lineNumber);

#endif //POLYNOMIALS_CALC_POLY_H
