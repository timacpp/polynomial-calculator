/** @file
  Implementation of numeric parsing function.

  @author Tymofii Vedmedenko
  @copyright University of Warsaw
  @date 2021
*/

#include <stdlib.h>
#include <limits.h>
#include <errno.h>
#include <string.h>

#include "numeric_parser.h"

#define CHECK_NULL_PTR(p) if (!p) exit(1)

static char* ExtractSubstring(const char* source, size_t from, size_t to) {
    const int numberLength = (int) (to - from);

    if (numberLength < 0)
        return NULL;

    char* numberString = malloc((numberLength + 1) * sizeof(char));
    CHECK_NULL_PTR(numberString);

    memcpy(numberString, &source[from], numberLength * sizeof(char));
    numberString[numberLength] = '\0';

    return numberString;
}

static size_t StringToULL(const char* numberString, size_t max) {
    if (!numberString) {
        errno = EINVAL;
        return 0;
    }

    const int base = 10;
    size_t parsedNumber = strtoull(numberString, NULL, base);

    if (numberString[0] == '-' || parsedNumber > max)
        errno = ERANGE;

    return parsedNumber;
}

static long StringToLong(char* numberString) {
    if (!numberString) {
        errno = EINVAL;
        return 0;
    }

    const int base = 10;
    long parsedNumber = strtol(numberString, NULL, base);

    return parsedNumber;
}

size_t SubstringToParameter(const char* source, size_t from, size_t to) {
    char* stringVariable = ExtractSubstring(source, from, to);
    size_t parsedVariable = StringToULL(stringVariable, ULLONG_MAX);

    if (stringVariable)
        free(stringVariable);

    return parsedVariable;
}

poly_exp_t SubstringToExp(const char* source, size_t from, size_t to) {
    char* stringExp = ExtractSubstring(source, from, to);
    poly_exp_t parsedExp = (poly_exp_t) StringToULL(stringExp, INT_MAX);

    if (stringExp)
        free(stringExp);

    return parsedExp;
}

poly_coeff_t SubstringToCoeff(const char* source, size_t from, size_t to) {
    char* stringCoeff = ExtractSubstring(source, from, to);
    poly_coeff_t parsedCoeff = StringToLong(stringCoeff);

    if (stringCoeff)
        free(stringCoeff);

    return parsedCoeff;
}
