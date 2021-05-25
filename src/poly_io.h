/** @file
  Interfejs operacji wejściowo-wyjściowych wielomianów rzadkich wielu zmiennych

  @authors Tymofii Vedmedenko <tv433559@students.mimuw.edu.pl>
  @copyright Uniwersytet Warszawski
  @date 2021
*/

#ifndef POLYNOMIALS_POLY_IO_H
#define POLYNOMIALS_POLY_IO_H

#include "poly.h"

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

/**
 * Parsuje spójny podciąg napisu na wykładnik wielomianu.
 * W przypadku gdy podciąg odpowiada liczbie poza zakresu
 * typu poly_exp_t, to ustawia odpowiedni errno i zwraca 0.
 * @param[in] source : napis
 * @param[in] from : początek podciągu (włączając)
 * @param[in] to : koniec podciągu (nie włączająć)
 * @return Sparsowany wykładnik
 */
poly_exp_t SubstringToExp(const char* source, size_t from, size_t to);

/**
 * Parsuje spójny pociąg napisu na współczynnik wielomianu.
 * W przypadku gdy podciag odpowiada liczbie poza zakresu
 * typu poly_coeff_t, to ustawia odpowiedni errno i zwraca 0.
 * @param[in] source : napis
 * @param[in] from : początek podciągu (włączając)
 * @param[in] to : koniec podciągu (nie włączająć)
 * @return Sparsowany wykładnik
 */
poly_coeff_t SubstringToCoeff(const char* source, size_t from, size_t to);

#endif //POLYNOMIALS_POLY_IO_H
