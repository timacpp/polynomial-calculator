#ifndef POLYNOMIALS_NUMERIC_PARSER_H
#define POLYNOMIALS_NUMERIC_PARSER_H

/** @file
  Interfejs funkcji parsujących liczbowe wartości.

  @authors Tymofii Vedmedenko <tv433559@students.mimuw.edu.pl>
  @copyright Uniwersytet Warszawski
  @date 2021
*/

#include "../poly.h"

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

#endif //POLYNOMIALS_NUMERIC_PARSER_H
