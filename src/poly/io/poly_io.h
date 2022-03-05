/** @file
  Interface for input-output operations on multi-variable polynomials.

  @author Tymofii Vedmedenko
  @copyright University of Warsaw
  @date 2021
*/

#ifndef POLYNOMIALS_POLY_IO_H
#define POLYNOMIALS_POLY_IO_H

#include "../poly.h"

/**
 * Reads polynomial to @p p from standard input.
 * @param[in] p : polynomial
 * @return Successful read?
 */
bool ReadPoly(Poly* p);

/**
 * Prints polynomial.
 * @param[in] p : polynomial.
 */
void PolyPrint(const Poly *p);

#endif //POLYNOMIALS_POLY_IO_H
