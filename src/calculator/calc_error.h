/** @file
  Interface of calculator error managing.

  @date 2021
*/

#ifndef POLYNOMIALS_CALC_ERROR_H
#define POLYNOMIALS_CALC_ERROR_H

/** Enumeration of possible calculator errors */
typedef enum CalcError {
    WRONG_POLY,
    WRONG_COMMAND,
    WRONG_AT_VALUE,
    STACK_UNDERFLOW,
    WRONG_DEG_VARIABLE,
    WRONG_COMPOSE_PARAMETER
} CalcError;

/**
 * Displays an error on stderr.
 * @param[in] error : error to print.
 * @param[in] line : line ordinal where error occurred.
 */
void PrintError(CalcError error, int line);

#endif //POLYNOMIALS_CALC_ERROR_H
