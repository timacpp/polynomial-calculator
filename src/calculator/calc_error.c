/** @file
  Implementacja managera błędów kalkulatora wielomianów wielu zmiennych.

  @authors Tymofii Vedmedenko <tv433559@students.mimuw.edu.pl>
  @copyright Uniwersytet Warszawski
  @date 2021
*/

#include <stdio.h>

#include "calc_error.h"

/**
 * Wypisuje błąd na wyjście błędów.
 * @param[in] error : błąd
 * @param[in] line : numer linijki
 */
void PrintError(CalcError error, int line) {
    switch (error) {
        case WRONG_POLY:
            fprintf(stderr, "ERROR %d WRONG POLY\n", line);
            break;
        case WRONG_COMMAND:
            fprintf(stderr, "ERROR %d WRONG COMMAND\n", line);
            break;
        case WRONG_AT_VALUE:
            fprintf(stderr, "ERROR %d AT WRONG VALUE\n", line);
            break;
        case STACK_UNDERFLOW:
            fprintf(stderr, "ERROR %d STACK UNDERFLOW\n", line);
            break;
        case WRONG_DEG_VARIABLE:
            fprintf(stderr, "ERROR %d DEG BY WRONG VARIABLE\n", line);
            break;
        case WRONG_COMPOSE_PARAMETER:
            fprintf(stderr, "ERROR %d COMPOSE WRONG PARAMETER\n", line);
            break;
    }
}