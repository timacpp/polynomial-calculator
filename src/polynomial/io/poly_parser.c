/** @file
  Implementacja funkcji parsujących wielomiany wielu zmiennych.

  @authors Tymofii Vedmedenko <tv433559@students.mimuw.edu.pl>
  @copyright Uniwersytet Warszawski
  @date 2021
*/

#include <errno.h>
#include <ctype.h>
#include <stdlib.h>

#include "poly_parser.h"
#include "numeric_parser.h"

#define CHECK_NULL_PTR(p) if (!p) exit(1)

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
Poly SubstringToPoly(const char* source, size_t from, size_t to) {
    assert(from <= to);

    // W przypadku gdy wcześniej parsowanie wykryło liczbę poza zakresem,
    // kończymy rekurencję drogą zwracania zerowego wielomianu.
    if (errno)
        return PolyZero();

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