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
#define IGNORE_LINE() { \
    char curChar = (char) getchar(); \
    while (curChar != EOF && curChar != '\n') \
        curChar = (char) getchar(); \
}

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

static Poly SubstringToPoly(const char* source, size_t from, size_t to);

/**
 * Przetwarza na jednomian wycinek napisu reprezentujący poprawny wielomian.
 * W przypadku niemożliwości sparsowania wykładnika jednomianu spowodowaną
 * niezgodnością zakresów typów danych zwracany jest zerowy jednomian
 * oraz wartość errno ustawia się na ERANGE.
 * @param[in] source : napis
 * @param[in] from : początek wycinku (włączając)
 * @param[in] to : koniec wycinku (nie włączająć)
 * @return Sparsowany jednomian
 */
static Mono SubstringToMono(const char* source, size_t from, size_t to) {
    // Pierwza cyfra wykładnika jest conajwyżej w indeksie (to - 1)
    // z powodu że cały jednomian się znajduje w przedziale [from, to)
    size_t expStartIdx = to - 1;

    // Zmięjszamy expStartIdx dopóki on nie wskazuje na pierwszą cyfrę wykładnika.
    // Uwaga: można nie sprawdzać wyjście poza tablicę, bo koma musi wystąpić.
    while (source[expStartIdx - 1] != ',')
        expStartIdx--;

    const int base = 10; // Baza konwersacja wykładnika za pomocą strtol.
    size_t expLength = to - expStartIdx; // Długość wykładnika zwiększona o jeden.

    char* expString = malloc(expLength * sizeof(char));
    CHECK_NULL_PTR(expString);

    memcpy(expString, &source[expStartIdx], expLength - 1);
    expString[expLength - 1] = '\0'; // Dodajemy null character dla strtol.

    long parsedExp = (poly_exp_t) strtol(expString, NULL, base);
    free(expString);

    return (Mono) {
        .exp = (poly_exp_t) parsedExp,
        .p = SubstringToPoly(source, from + 1, expStartIdx - 1)
    };
}

/**
 * Przetwarza na niestały wielomian wycinek napisu reprezentujący poprawny wielomian.
 * @param[in] source : napis
 * @param[in] from : początek wycinku (włączając)
 * @param[in] to : koniec wycinku (nie włączająć)
 * @return Sparsowany niestały wielomian
 */
static Poly SubstringToNonCoeffPoly(const char* source, size_t from, size_t to) {
    Mono* monos = NULL;
    size_t size = 0, capacity = 0;
    size_t openedParenthesis = 0, monoLeftIdx = 0;

    for (size_t i = from; i < to; i++) {
        char curChar = source[i];

        if (curChar == '(') { // Jeśli nawias otwierający
            if (openedParenthesis == 0) // jest na zerowym poziomie rekurencji
                monoLeftIdx = i; // to i jest początkiem jednomianu.

            openedParenthesis++;
        } else if (curChar == ')') {
            openedParenthesis--;

            if (openedParenthesis == 0) {
                if (size == capacity) {
                    capacity = 2 * capacity + 1;
                    monos = realloc(monos, capacity * sizeof(Mono));
                    CHECK_NULL_PTR(monos);
                }
                // Jak nawias zamykający jest na zerowym poziomie, to [monoLeftIdx, i + 1) reprezentuje mono.
                monos[size++] = SubstringToMono(source, monoLeftIdx, i + 1);
            }
        }
    }

    Poly resPoly = PolyAddMonos(size, monos);
    free(monos);

    return resPoly;
}


/**
 * Przetwarza na stały wielomian wycinek napisu reprezentujący poprawny wielomian.
 * W przypadku niemożliwości sparsowania współczynnika wielomianu spowodowaną niezgodnością
 * zakresów typów danych zwracany jest zerowy wielomian oraz wartość errno ustawia się na ERANGE.
 * @param[in] source : napis
 * @param[in] from : początek wycinku (włączając)
 * @param[in] to : koniec wycinku (nie włączająć)
 * @return Sparsowany stały wielomian
 */
static Poly SubstringToCoeffPoly(const char* source, size_t from, size_t to) {
    const int base = 10; // Baza konwersacji dla strtol.
    const size_t newLength = to - from + 1; // Długość współczynnika zwiększone o jeden.

    char* coeffString = malloc(newLength * sizeof(char));
    CHECK_NULL_PTR(coeffString);

    memcpy(coeffString, &source[from], newLength - 1);
    coeffString[newLength - 1] = '\0'; // Null character dla strtol.

    poly_coeff_t parsedCoeff = strtol(coeffString, NULL, base);
    free(coeffString);

    return PolyFromCoeff(parsedCoeff);
}

/**
 * Sprawdza czy wycinek napisu reprezentujący poprawny wielomian odpowiada wielomianowi stałemu.
 * @param[in] source : napis
 * @param[in] from : początek wycinku (włączając)
 * @param[in] to : koniec wycinku (nie włączająć)
 * @return Czy wycinek napisu reprezentuje wielomian stały?
 */
static bool SubstringIsNumber(const char* source, size_t from, size_t to) {
    // Uwaga: Aby dowiedzieć że wielomian jest stały, wystarczy sprawdzić pierwszy i ostatni
    // symbol wycinku napisu, ponieważ wiemy że source reprezentuje poprawny wielomian.
    return (isdigit(source[from]) || source[from] == '-') && isdigit(source[to - 1]);
}

/**
 * Przetwarza na wielomian wycinek napisu reprezentujący poprawny wielomian.
 * @param[in] source : napis
 * @param[in] from : początek wycinku (włączając)
 * @param[in] to : koniec wycinku (nie włączająć)
 * @return Sparsowany wielomian
 */
static Poly SubstringToPoly(const char* source, size_t from, size_t to) {
    assert(from <= to);

    // Jeżeli pierwszy i ostatni symbol na zakresie [from, to) są cyframi
    // z możliwością wiodącego znaku minus, to source reprezentuje stały
    // wielomian, dlatego możemy odrazu go sparsować bez rekurencji.
    if (SubstringIsNumber(source, from, to))
        return SubstringToCoeffPoly(source, from, to);

    // W przeciwnym przypadku parsujemy rekurencyjnie.
    return SubstringToNonCoeffPoly(source, from, to);
}

/**
 * Sprawdza, czy po pewnym znaku wielomiana może występować następny.
 * @param[in] cur : znak do sprawdzeni
 * @param[in] prev : poprzedni znak
 * @param[in] lastNonDigit : ostatni niecyfrowy znak
 * @return Czy może znak wielomianu wyprzedzać poprzedni?
 */
static bool IsValidPolyCharAfter(char cur, char prev, char lastNonDigit) {
    // Zmienna wskazuje czy cur wskazuje na cyfre współczynnika.
    bool curCoeff = (lastNonDigit == '(' || lastNonDigit == '-' || lastNonDigit == (char) 0);

    // Sprawdzamy wszystkie możliwe wartości cur uwzgłęniając poprzedni symbol.
    if (isdigit(prev))
        return isdigit(cur) || (curCoeff ? cur == ',' : cur == ')');

    switch (prev) {
        case '(':
            return ((isdigit(cur) || cur == '-') && curCoeff) || cur == '(';
        case ')':
            return cur == ')' || cur == ',' || cur == '+';
        case ',':
            return isdigit(cur) && !curCoeff;
        case '-':
            return isdigit(cur) && curCoeff;
        case '+':
            return cur == '(';
        default:
            return false;
    }
}

/**
 * Wczytuje wielomian w napisowej postaci do przekazanego wskaźnika.
 * W przypadku uznania nielegalnego znaku wielomianu albo jego niedozwolonego
 * umieścienia funkcja zwraca identyfikator false i zwalnia pamięć buffora.
 * W przeciwnym przypadku, tylko jest zwracana wartość true.
 * @param[in] buffer : napis do wczytania wielomianu
 * @return Czy udało się wczytać poprawny wielomian?
 */
static bool ReadStringParsedPoly(char** buffer) {
    size_t size = 0, capacity = 0;
    char curChar = (char) getchar();
    char lastNonDigit = (char) 0;
    bool allDigits = isdigit(curChar);

    if (curChar != '(' && !allDigits)
        return false;

    while (curChar != EOF && curChar != '\n') {
        if (size > 0) {
            char prevChar = (*buffer)[size - 1];
            if (!IsValidPolyCharAfter(curChar, prevChar, lastNonDigit))
                return false;
        }
        if (size == capacity) {
            capacity = 2 * capacity + 1;
            *buffer = realloc(*buffer, capacity * sizeof(char));
            CHECK_NULL_PTR(*buffer);
        }

        (*buffer)[size++] = curChar;
        allDigits = isdigit(curChar);
        if (!isdigit(curChar))
            lastNonDigit = curChar;
        curChar = (char) getchar();
    }

    return allDigits || (*buffer)[size - 1] == ')';
}

bool ReadPoly(Poly* p) {
    char* buffer = NULL;
    bool successfulRead = ReadStringParsedPoly(&buffer);

    if (!successfulRead) {
        free(buffer);
        IGNORE_LINE();
        return false;
    }

    *p = SubstringToPoly(buffer, 0, strlen(buffer));
    bool successfulParsing = (errno != ERANGE && errno != EINVAL);

    if (!successfulParsing) {
        PolyDestroy(p);
        return false;
    }

    return true;
}
