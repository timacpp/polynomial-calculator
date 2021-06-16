/** @file
  Intefejs procesora     komend kalkulatora wielomianów wielu zmiennych.

  @authors Tymofii Vedmedenko <tv433559@students.mimuw.edu.pl>
  @copyright Uniwersytet Warszawski
  @date 2021
*/

#ifndef POLYNOMIALS_CALC_COMMAND_H
#define POLYNOMIALS_CALC_COMMAND_H

#include "../polynomial/structures/poly_stack.h"

/**
 * Daje sygnal kalkulatorowi do wczytania linijki zawierającej komendę.
 * W przypadku wczytania poprawnej komendy zostanie wykonanie podane
 * polecenie nad stosem @p stack, a w przeciwnym przypadku będzie
 * wypisany błąd na odpowiednie wyjście.
 * @param[in] stack: stos z wielomianami
 * @param[in] lineNumber : numer linijki, która będzie wczytana
 */
void ProcessCommandInput(PolyStack* stack, int lineNumber);

#endif //POLYNOMIALS_CALC_COMMAND_H

