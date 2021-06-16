/** @file
  Implementacja funkcji parsujących liczbowe wartości.

  @authors Tymofii Vedmedenko <tv433559@students.mimuw.edu.pl>
  @copyright Uniwersytet Warszawski
  @date 2021
*/

#include <stdlib.h>
#include <limits.h>
#include <errno.h>
#include <string.h>

#include "numeric_parser.h"

#define CHECK_NULL_PTR(p) if (!p) exit(1)

poly_exp_t SubstringToExp(const char* source, size_t from, size_t to) {
    const int base = 10;
    size_t expLength = to - from;

    char* expString = malloc((expLength + 1) * sizeof(char));
    CHECK_NULL_PTR(expString);

    // Kopiujemy zawartość pamięci reprezentującą wykładnik.
    memcpy(expString, &source[from], expLength * sizeof(char));
    expString[expLength] = '\0';

    size_t parsedExp = strtoul(expString, NULL, base);
    free(expString);

    // Samodzielnie ustawiamy errno na ERRANGE jak wykładnik jest poza górnym zakresem.
    if (parsedExp > INT_MAX)
        errno = ERANGE;

    return (errno ? 0 : (poly_exp_t) parsedExp);
}

poly_coeff_t SubstringToCoeff(const char* source, size_t from, size_t to) {
    const int base = 10;
    const size_t coeffLength = to - from;

    // Kopiujemy zawartość pamięci reprezentującą wpółczynnik.
    char* coeffString = malloc((coeffLength + 1) * sizeof(char));
    CHECK_NULL_PTR(coeffString);

    memcpy(coeffString, &source[from], coeffLength);
    coeffString[coeffLength] = '\0';

    poly_coeff_t parsedCoeff = strtol(coeffString, NULL, base);
    free(coeffString);

    return (errno ? 0 : parsedCoeff);
}
