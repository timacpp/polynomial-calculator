/** @file
  Interfejs operacji wejściowo-wyjściowych wielomianów rzadkich wielu zmiennych

  @authors Tymofii Vedmedenko <tv433559@students.mimuw.edu.pl>
  @copyright Uniwersytet Warszawski
  @date 2021
*/

#ifndef POLYNOMIALS_POLY_IO_H
#define POLYNOMIALS_POLY_IO_H

#include "../poly.h"

/**
 * Wczytuje wielomian do przekazanego wkaźnika.
 * @param[in] p : wielomian
 * @return Czy udało się wczytać wielomian?
 */
bool ReadPoly(Poly* p);

/**
 * Wypisuje wielomian.
 * @param[in] p : wielomian
 */
void PolyPrint(const Poly *p);

#endif //POLYNOMIALS_POLY_IO_H
