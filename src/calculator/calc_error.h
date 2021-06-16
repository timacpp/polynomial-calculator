/** @file
  Intefejs managera błędów kalkulatora wielomianów wielu zmiennych.

  @authors Tymofii Vedmedenko <tv433559@students.mimuw.edu.pl>
  @copyright Uniwersytet Warszawski
  @date 2021
*/

#ifndef POLYNOMIALS_CALC_ERROR_H
#define POLYNOMIALS_CALC_ERROR_H

/** To jest typ uogólniający możliwe błędy operacyjne kalkulatora. */
typedef enum CalcError {
    WRONG_POLY,
    WRONG_COMMAND,
    WRONG_AT_VALUE,
    STACK_UNDERFLOW,
    WRONG_DEG_VARIABLE,
} CalcError;


void PrintError(CalcError error, int line);

#endif //POLYNOMIALS_CALC_ERROR_H
