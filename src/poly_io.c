/** @file
  Implementacja operacji wejściowo-wyjściowych wielomianów rzadkich wielu zmiennych

  @authors Tymofii Vedmedenko <tv433559@students.mimuw.edu.pl>
  @copyright Uniwersytet Warszawski
  @date 2021
*/

#include "poly_io.h"
#include <errno.h>
#include <stdio.h>
#include <limits.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

#define CHECK_NULL_PTR(p) if (!p) exit(1)

/**
 * Wypisuje jednomian.
 * @param[in] m : jednomian
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

static Poly SubstringToPoly(const char* source, size_t from, size_t to);

/**
 * Parsuje spójny pocjąg napisu reprezentujący
 * poprawny wielomian na jednomian.
 * @param[in] source : napis
 * @param[in] from : początek podciągu (włączając)
 * @param[in] to : koniec podciągu (nie włączająć)
 * @return Sparsowany jednomian
 */
static Mono SubstringToMono(const char* source, size_t from, size_t to) {
    size_t expStartIdx = to - 1; // Pierwza cyfra wykładnika jest conajwyżej w indeksie (to - 1).

    // Zmięjszamy expStartIdx dopóki on nie wskazuje na pierwszą cyfrę wykładnika.
    while (source[expStartIdx - 1] != ',')
        expStartIdx--;

    // Znając indeks początku wykładnika, wpółczynnik jest w zakresie [from + 1, expStartIdx - 1).
    return (Mono) {
        .exp = SubstringToExp(source, expStartIdx, to),
        .p = SubstringToPoly(source, from + 1, expStartIdx - 1)
    };
}

/**
 * Daje następny indeks znaku w @p source po @p pos, który
 * się znajduje na zerowym stopniu zagłębenia w wielomianie.
 * Zakładamy, że @p source odpowiada poprawnemu wielomianowi.
 * @param[in] source : napis
 * @param[in] from : początek wycinku (włączając)
 * @param[in] to : koniec wycinku (nie włączająć)
 * @return Sparsowany jednomian
 */
static size_t FindParenthesisBalancePosition(const char* source, size_t pos) {
    size_t depth = 0; // Stopień zagłębienia.

    do {
        if (source[pos] == '(')
            depth++;
        else if (source[pos] == ')')
            depth--;
        pos++;
    } while (depth > 0); // Dopóki nie jesteś znów na zerowym stopniu.

    return pos; // source[pos - 1] == ')'
}

/**
 * Przetwarza spójny podciąg napisu reprezentujący
 * poprawnemy wielomian na niestały wielomian.
 * @param[in] source : napis
 * @param[in] from : początek podciągu (włączając)
 * @param[in] to : koniec podciągu (nie włączająć)
 * @return Sparsowany niestały wielomian
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

        // W zakresie [monoStartIdx, monoEndIdx) się znajduje jednomian.
        monos[size++] = SubstringToMono(source, monoStartIdx, monoEndIdx);
        i = monoEndIdx; // Zmieniamy obecny indeks to końca jednomianu.
    }

    Poly resPoly = PolyAddMonos(size, monos);
    free(monos);

    return resPoly;
}


/**
 * Sprawdza czy spójny podciąg napisu reprezentujący
 * poprawny wielomian odpowiada wielomianowi stałemu.
 * @param[in] source : napis
 * @param[in] from : początek podciągu (włączając)
 * @param[in] to : koniec podciągu (nie włączająć)
 * @return Czy wycinek napisu reprezentuje wielomian stały?
 */
static bool SubstringIsNumber(const char* source, size_t from, size_t to) {
    // Aby dowiedzieć że wielomian jest stały, wystarczy sprawdzić pierwszy i ostatni
    // symbol podciągu napisu, ponieważ wiemy że source reprezentuje poprawny wielomian.
    return (isdigit(source[from]) || source[from] == '-') && isdigit(source[to - 1]);
}

/**
 * Przetwarza spójny podciąg reprezentujący
 * poprawny wielomian na wielomian.
 * @param[in] source : napis
 * @param[in] from : początek podciągu (włączając)
 * @param[in] to : koniec podciągu (nie włączająć)
 * @return Sparsowany wielomian
 */
static Poly SubstringToPoly(const char* source, size_t from, size_t to) {
    assert(from <= to);

    // Jeżeli pierwszy i ostatni symbol na zakresie [from, to) są cyframi
    // z możliwością wiodącego znaku minus, to source reprezentuje stały
    // wielomian, dlatego możemy odrazu go sparsować bez rekurencji.
    if (SubstringIsNumber(source, from, to)) {
        poly_coeff_t coeff = SubstringToCoeff(source, from, to);
        return PolyFromCoeff(coeff);
    }

    // W przeciwnym przypadku parsujemy rekurencyjnie.
    return SubstringToNonCoeffPoly(source, from, to);
}

/**
 * Sprawdza, czy po pewnym znaku wielomianu może występować następny.
 * @param[in] cur : znak do sprawdzenia
 * @param[in] prev : poprzedni znak
 * @param[in] lastNonDigit : ostatni niecyfrowy znak
 * @return Czy może znak @p cur występować po @p prev?
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
 * Sprawdza, czy napis reprezentuje poprawny niestały wielomian.
 * @param[in] stringPoly: napis
 * @param[in] length : długość napisu
 * @return Czy może @p stringPoly reprezentuje poprawny niestały wielomian?
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
 * Sprawdza, czy napis reprezentuje poprawny stały wielomian.
 * @param[in] stringPoly: napis
 * @param[in] length : długość napisu
 * @return Czy może @p stringPoly reprezentuje poprawny stały wielomian?
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
 * Sprawdza, czy napis reprezentuje poprawny wielomian.
 * @param[in] stringPoly: napis
 * @param[in] length : długość napisu
 * @return Czy może @p stringPoly reprezentuje poprawny wielomian?
 */
static bool IsCorrectPoly(const char* stringPoly, size_t length) {
    if (IsCorrectCoeffPoly(stringPoly, length))
        return true;
    return IsCorrectNonCoeffPoly(stringPoly, length);
}

static bool IsValidPolyChar(char curChar) {
    return curChar == '(' || curChar == ')' || curChar == ',' ||
           curChar == '+' || curChar == '-' || isdigit(curChar);
}

/**
 * Wczytuje napis reprezentujący wielomian
 * do przekazywanego wskaźnika na buffor.
 * @param[in] buffer: buffor do wczytania wielomianu
 * @return Czy udało się wczytać wielomian?
 */
static bool ReadStringPoly(char** buffer) {
    size_t size = 0, capacity = 1;
    char curChar = (char) getchar();
    bool validChars = true;

    while (curChar != EOF && curChar != '\n') {
        validChars &= IsValidPolyChar(curChar);
        if (validChars) {
            if (size + 1 == capacity) {
                capacity = 2 * capacity + 1;
                *buffer = realloc(*buffer, capacity * sizeof(char));
                CHECK_NULL_PTR(*buffer);
            }

            (*buffer)[size++] = curChar;
        }
        curChar = (char) getchar();
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

    free(buffer);
    if (!successfulParsing)
        PolyDestroy(p);

    return correctPolyInput && successfulParsing; // SuccessfulRead tutaj jest zawsze prawdą.
}
