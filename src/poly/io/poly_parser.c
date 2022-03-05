/** @file
  Implementation of polynomial parsing functions.

  @author Tymofii Vedmedenko
  @copyright University of Warsaw
  @date 2021
*/

#include <errno.h>
#include <ctype.h>
#include <stdlib.h>

#include "poly_parser.h"
#include "numeric_parser.h"

#define CHECK_NULL_PTR(p) if (!p) exit(1)

/**
 * Parses a range [@p from, @p to) of characters from @p source onto monomial.
 * @param[in] source : word
 * @param[in] from : including start
 * @param[in] to : excluding end
 * @return parsed monomial
 */
static Mono SubstringToMono(const char* source, size_t from, size_t to) {
    /* First digit of an exponent is at most in index (to - 1) */
    size_t expStartIdx = to - 1;

    /* Reducing expStartIdx until it points to a first digit of an exponent */
    while (source[expStartIdx - 1] != ',')
        expStartIdx--;

    /* Coefficient is in range [from + 1, expStartIdx - 1) */
    return (Mono) {
            .exp = SubstringToExp(source, expStartIdx, to),
            .p = SubstringToPoly(source, from + 1, expStartIdx - 1)
    };
}

/**
 * Gives index of of an end of the polynomial starting from @p pos in @p source.
 * @param[in] source : word
 * @param[in] pos : starting position of a polynomial
 * @return index of a closing parenthesis
 */
static size_t FindParenthesisBalancePosition(const char* source, size_t pos) {
    size_t depth = 0; /* Recursion depth of a polynomial */

    do {
        if (source[pos] == '(')
            depth++;
        else if (source[pos] == ')')
            depth--;
        pos++;
    } while (depth > 0);

    return pos; // source[pos - 1] == ')'
}

/**
 * Parses a range [@p from, @p to) of characters from @p source onto non-constant poly.
 * @param[in] source : word
 * @param[in] from : including start
 * @param[in] to : excluding end
 * @return parsed monomial
 */
static Poly SubstringToNonCoeffPoly(const char* source, size_t from, size_t to) {
    Mono* monos = NULL;
    size_t size = 0, capacity = 1;

    for (size_t i = from; i < to; i++) {
        size_t monoStartIdx = i;
        size_t monoEndIdx = FindParenthesisBalancePosition(source, i);

        if (size + 1 == capacity) {
            capacity = 2 * capacity + 1;
            monos = realloc(monos, capacity * sizeof(Mono));
            CHECK_NULL_PTR(monos);
        }

        /* Range [monoStartIdx, monoEndIdx) now contains monomial */
        monos[size++] = SubstringToMono(source, monoStartIdx, monoEndIdx);
        i = monoEndIdx;
    }

    Poly resPoly = PolyAddMonos(size, monos);
    free(monos);

    return resPoly;
}


/**
 * Checks whether a range [@p from, @p to) of characters from @p source represent a number.
 * @param[in] source : word
 * @param[in] from : including start
 * @param[in] to : excluding end
 * @return is range a number?
 */
static bool SubstringIsNumber(const char* source, size_t from, size_t to) {
    /* As source represents a correct polynomial, we can only check the corner values */
    return (isdigit(source[from]) || source[from] == '-') && isdigit(source[to - 1]);
}

Poly SubstringToPoly(const char* source, size_t from, size_t to) {
    assert(from <= to);

    /* In case of failed parsing zero polynomial is returned */
    if (errno)
        return PolyZero();

    if (SubstringIsNumber(source, from, to)) {
        poly_coeff_t coeff = SubstringToCoeff(source, from, to);
        return PolyFromCoeff(coeff);
    }

    /* Polynomial is non-constant, so we parse it recursively */
    return SubstringToNonCoeffPoly(source, from, to);
}