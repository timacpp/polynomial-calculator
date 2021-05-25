/** @file
  Implementacja stosu wielomianów rzadkich wielu zmiennych

  @authors Tymofii Vedmedenko <tv433559@students.mimuw.edu.pl>
  @copyright Uniwersytet Warszawski
  @date 2021
*/

#ifndef POLYNOMIALS_POLY_STACK_H
#define POLYNOMIALS_POLY_STACK_H

#include "poly.h"

/** To jest typ reprezentujący stos wielomianów. */
typedef struct PolyStack {
    size_t size; ///< rozmiar stosu
    size_t capacity; ///< pojemność stosu
    Poly*  content; ///< zawartość stosu
} PolyStack;


/**
 * Inicjalizuje początkowe atrybuty stosu.
 * @param[in] stack : stos
 */
void StackInitialize(PolyStack* stack);

/**
 * Wstawia na wierzchołek stosu
 * wielomian przekazywanyna własność.
 * @param[in] stack : stos
 * @param[in] p : wielomian
 */
void PushPoly(PolyStack* stack, Poly p);


/**
 * Zwraca nie na własność wielomian z wierzchołku.
 * @param[in] stack : stos
 * @return Wielomian z wierzchołku
 */
Poly TopPoly(PolyStack* stack);

/**
 * Zwraca na własność wielomian z wierzcholka i usuwa go ze stosu.
 * @param[in] stack : stos
 * @return Wielomian z wierzchołku
 */

Poly PopPoly(PolyStack* stack);

/**
 * Usuwa stos z pamięci.
 * @param[in] stack : stos
 */
void StackDestroy(PolyStack* stack);


#endif //POLYNOMIALS_POLY_STACK_H
