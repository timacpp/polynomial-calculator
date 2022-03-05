/** @file
  Interface for numeric parsing functions.

  @author Tymofii Vedmedenko
  @copyright University of Warsaw
  @date 2021
*/

#ifndef POLYNOMIALS_NUMERIC_PARSER_H
#define POLYNOMIALS_NUMERIC_PARSER_H

#include "../poly.h"

/**
 * Parses a range [@p from, @p to) of characters from @p source onto DegBy/PolyCompose argument.
 * In case of an size_t type overflow, errno is modified and zero is returned.
 * @param[in] source : word
 * @param[in] from : including start
 * @param[in] to : excluding end
 * @return parsed parameter
 */
size_t SubstringToParameter(const char* source, size_t from, size_t to);

/**
 * Parses a range [@p from, @p to) of characters from @p source onto polynomial exponent.
 * In case of an poly_exp_t type overflow, errno is modified and zero is returned.
 * @param[in] source : word
 * @param[in] from : including start
 * @param[in] to : excluding end
 * @return parsed coefficient
 */
poly_exp_t SubstringToExp(const char* source, size_t from, size_t to);

/**
 * Parses a range [@p from, @p to) of characters from @p source onto polynomial coefficient.
 * In case of an poly_coeff_t type overflow, errno is modified and zero is returned.
 * @param[in] source : word
 * @param[in] from : including start
 * @param[in] to : excluding end
 * @return parsed coefficient
 */
poly_coeff_t SubstringToCoeff(const char* source, size_t from, size_t to);

#endif //POLYNOMIALS_NUMERIC_PARSER_H
