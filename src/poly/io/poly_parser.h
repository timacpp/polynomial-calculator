/** @file
  Interface for polynomial parsing functions.

  @author Tymofii Vedmedenko
  @copyright University of Warsaw
  @date 2021
*/

#ifndef POLYNOMIALS_POLY_PARSER_H
#define POLYNOMIALS_POLY_PARSER_H

#include "../poly.h"

/**
 * Parses a range [@p from, @p to) of characters from @p source onto polynomial.
 * @param[in] source : word
 * @param[in] from : including start
 * @param[in] to : excluding end
 * @return parsed polynomial
 */
Poly SubstringToPoly(const char* source, size_t from, size_t to);

#endif //POLYNOMIALS_POLY_PARSER_H
