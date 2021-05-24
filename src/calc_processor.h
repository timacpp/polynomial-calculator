/** @file
  Interfejs procesora kalkulatora wielomianów rzadkich wielu zmiennych

  @authors Tymofii Vedmedenko <tv433559@students.mimuw.edu.pl>
  @copyright Uniwersytet Warszawski
  @date 2021
*/

#ifndef POLYNOMIALS_CALC_PROCESSOR_H
#define POLYNOMIALS_CALC_PROCESSOR_H

#include "poly_stack.h"

/** To jest typ uogólniający możliwe błędy operacyjne kalkulatora. */
typedef enum CalcError {
    WRONG_POLY,
    WRONG_COMMAND,
    WRONG_AT_VALUE,
    STACK_UNDERFLOW,
    WRONG_DEG_VARIABLE,
} CalcError;

/**
 * Wypisuje błąd na wyjście błędów
 * @param[in] error : błąd
 * @param[in] line : numer linijki
 */
void PrintError(CalcError error, int line);

/**
 * Wczytuje komendę reprezentowaną jako napis do wartości przekazanego
 * wzkaźnika. Jeżeli podczas wczytania wystąpi nielegalny symbol komendy
 * albo zostanie uznany niepoprawny porządek, to funkcja zwraca indykator
 * false i zwalnia wykorzystany buffor. W przeciwnym przypadku zwraca true.
 * @param[in] command : wskaźnik do wczytania
 * @return Czy udało się wczytać komendę?
 */
bool ReadCommand(char** command);

/**
 * Przetwarza komendę będącym napisem na działania na stosie wielomianów.
 * W przypadku nie istniejącej komendy, nie poprawnego parametru lub braku
 * wystarczającej liczby wielomianów na stosie wypisany będzie odpowiedni błąd.
 * @param[in] stack : stos wielomianów
 * @param[in] command : komenda
 * @param[in] lineNumber : numer linijki
 */
void ProcessCommand(PolyStack* stack, const char* command, int lineNumber);

#endif //POLYNOMIALS_CALC_PROCESSOR_H
