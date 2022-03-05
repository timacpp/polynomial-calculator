/** @file
  Implementation of input-output operations on multi-variable polynomials.

  @author Tymofii Vedmedenko
  @copyright University of Warsaw
  @date 2021
*/

#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

#include "poly_io.h"
#include "poly_parser.h"

#define CHECK_NULL_PTR(p) if (!p) exit(1)

/**
 * Prints monomial.
 * @param[in] m :  monomial
 */
static void MonoPrint(const Mono* m) {
    printf("(");
    PolyPrint(&m->p);
    printf(",%d)", m->exp);
}

void PolyPrint(const Poly* p) {
    if (PolyIsCoeff(p)) {
        printf("%ld", p->coeff);
        return;
    }

    for (size_t curMonoID = 0; curMonoID < p->size; curMonoID++) {
        MonoPrint(&p->arr[curMonoID]);

        // Wypisujemy plus jeżeli dalej jeszcze występuje jednomian.
        if (curMonoID < p->size - 1)
            printf("+");
    }
}

/**
 * Checks whether a correct input line can contain character @p cur after @p prev.
 * @param[in] cur : current character
 * @param[in] prev : previous character
 * @param[in] lastNonDigit : last non-digit character
 * @return Can @p cur be after @p prev?
 */
static bool IsValidPolyCharAfter(char cur, char prev, char lastNonDigit) {
    // Zmienna sprawdza czy prev wskazuje na cyfrę współczynnika, ponieważ
    // po wystąpieniu '(' lub '-' następa cyfra jest częścią współczynnika
    bool prevCoeff = (lastNonDigit == '(' || lastNonDigit == '-');

    // Sprawdzamy wszystkie możliwe wartości cur uwzgłędniając poprzedni znak.
    if (isdigit(prev))
        return isdigit(cur) || (prevCoeff ? cur == ',' : cur == ')');

    switch (prev) {
        case '(':
            return (isdigit(cur) || cur == '-') || cur == '(';
        case ')':
            return cur == ',' || cur == '+';
        case ',':
            return isdigit(cur) && !prevCoeff;
        case '-':
            return isdigit(cur) && prevCoeff;
        case '+':
            return cur == '(';
        default:
            return false;
    }
}

/**
 * Checks whether @p stringPoly represents a correct non-constant polynomial.
 * @param[in] stringPoly : sequence of characters
 * @param[in] length : lengths of the sequence
 * @return Does @p stringPoly represent a correct non-constant polynomial?
 */
static bool IsCorrectNonCoeffPoly(const char* stringPoly, size_t length) {
    // Każdy wielomian niestały ma nawiasy z dwóch stron.
    if (stringPoly[0] != '(' || stringPoly[length - 1] != ')')
        return false;

    int parenthesis = 1; // stringPoly[0] == '('
    char lastNonDigit = '(';

    for (size_t i = 1; i < length; i++) {
        char curChar = stringPoly[i];
        char prevChar = stringPoly[i - 1];

        if (!IsValidPolyCharAfter(curChar, prevChar, lastNonDigit))
            return false;

        if (curChar == '(')
            parenthesis++;
        else if (curChar == ')')
            parenthesis--;

        if (!isdigit(curChar))
            lastNonDigit = curChar;
    }

    return parenthesis == 0;
}

/**
 * Checks whether @p stringPoly represents a correct constant polynomial.
 * @param[in] stringPoly : sequence of characters
 * @param[in] length : lengths of the sequence
 * @return Does @p stringPoly represent a correct constant polynomial?
 */
static bool IsCorrectCoeffPoly(const char* stringPoly, size_t length) {
    // Jak wielomian się składa tylko z minusu, to nie jest poprawny.
    if (stringPoly[0] == '-' && length == 1)
        return false;

    for (size_t i = 0; i < length; i++) {
        // Każdy wielomian stały zawiera albo minus (tylko na początku) albo cyfry.
        bool validChar = isdigit(stringPoly[i]) || (i == 0 && stringPoly[i] == '-');

        if (!validChar)
            return false;
    }

    return true;
}

/**
 * Checks whether @p stringPoly represents a correct polynomial.
 * @param[in] stringPoly : sequence of characters
 * @param[in] length : lengths of the sequence
 * @return Does @p stringPoly represent a correct polynomial?
 */
static bool IsCorrectPoly(const char* stringPoly, size_t length) {
    if (IsCorrectCoeffPoly(stringPoly, length))
        return true;
    return IsCorrectNonCoeffPoly(stringPoly, length);
}

static bool IsValidPolyChar(int curChar) {
    return curChar == '(' || curChar == ')' || curChar == ',' ||
           curChar == '+' || curChar == '-' || isdigit(curChar);
}

/**
 * Reads a line representing a polynomial to @p buffer.
 * @param[in] buffer: buffer for unparsed polynomial.
 * @return Was the read successful?
 */
static bool ReadStringPoly(char** buffer) {
    int curChar = getchar();
    bool validChars = true;
    size_t size = 0, capacity = 1;

    while (curChar != EOF && curChar != '\n') {
        validChars &= IsValidPolyChar(curChar);
        if (validChars) {
            if (size + 1 == capacity) {
                capacity = 2 * capacity + 1;
                *buffer = realloc(*buffer, capacity * sizeof(char));
                CHECK_NULL_PTR(*buffer);
            }

            (*buffer)[size++] = (char) curChar;
        }
        curChar = getchar();
    }

    if (validChars)
        (*buffer)[size] = '\0';

    return validChars;
}

bool ReadPoly(Poly* p) {
    char* buffer = NULL;
    bool successfulRead = ReadStringPoly(&buffer);

    if (!successfulRead) {
        free(buffer);
        return false;
    }

    size_t size = strlen(buffer);
    bool correctPolyInput = IsCorrectPoly(buffer, size);

    if (correctPolyInput)
        *p = SubstringToPoly(buffer, 0, size);

    // SubstringToPoly może zmienić errno.
    bool successfulParsing = (errno == 0);

    if (!successfulParsing)
        PolyDestroy(p);

    free(buffer);

    return correctPolyInput && successfulParsing; // SuccessfulRead tutaj jest zawsze prawdą.
}
