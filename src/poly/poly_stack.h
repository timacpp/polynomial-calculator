/** @file
  Interface of multi-variable polynomial stack.

  @author Tymofii Vedmedenko
  @copyright University of Warsaw
  @date 2021
*/

#ifndef POLYNOMIALS_POLY_STACK_H
#define POLYNOMIALS_POLY_STACK_H

#include "poly.h"

/** Type representing a poly stack. */
typedef struct PolyStack {
    size_t size;
    size_t capacity;
    Poly*  content;
} PolyStack;


/**
 * Initializes the starting state of a stack.
 * @param[in] stack : stack to initialize
 */
void StackInitialize(PolyStack* stack);

/**
 * Pushes a poly to a stack.
 * @param[in] stack : stack
 * @param[in] p : poly
 */
void PushPoly(PolyStack* stack, Poly p);

/**
 * Returns a plain copy of a top poly from a stack
 * @param[in] stack : stack
 * @return Polynomial from top
 */
Poly TopPoly(PolyStack* stack);

/**
 * Returns a top poly and removes it from stack without deleting.
 * @param[in] stack : stack
 * @return Polynomial from top
 */
Poly PopPoly(PolyStack* stack);

/**
 * Clears allocated for a stack memory.
 * @param[in] stack : stack
 */
void StackDestroy(PolyStack* stack);

#endif //POLYNOMIALS_POLY_STACK_H
