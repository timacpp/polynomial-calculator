/** @file
  Interfejs funkcji parsujących wielomiany wielu zmiennych.

  @authors Tymofii Vedmedenko <tv433559@students.mimuw.edu.pl>
  @copyright Uniwersytet Warszawski
  @date 2021
*/

#ifndef POLYNOMIALS_POLY_PARSER_H
#define POLYNOMIALS_POLY_PARSER_H

#include "../poly.h"

Poly SubstringToPoly(const char* source, size_t from, size_t to);

#endif //POLYNOMIALS_POLY_PARSER_H
