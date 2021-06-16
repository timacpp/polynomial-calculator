/** @file
  Interfejs stosu wielomianów rzadkich wielu zmiennych

  @authors Tymofii Vedmedenko <tv433559@students.mimuw.edu.pl>
  @copyright Uniwersytet Warszawski
  @date 2021
*/

#include "poly_stack.h"
#include <stdlib.h>

#define CHECK_NULL_PTR(p) if (!p) exit(1)

void StackInitialize(PolyStack* stack) {
    stack->size = 0;
    stack->capacity = 1;
    stack->content = NULL;
}

void PushPoly(PolyStack* stack, Poly p) {
    if (stack->size + 1 == stack->capacity) {
        stack->capacity = 2 * stack->capacity + 1;
        stack->content = realloc(stack->content, stack->capacity * sizeof(Poly));
        CHECK_NULL_PTR(stack->content);
    }

    stack->content[stack->size++] = p;
}

Poly TopPoly(PolyStack* stack) {
    assert(stack->size > 0);
    return stack->content[stack->size - 1];
}

Poly PopPoly(PolyStack* stack) {
    assert(stack->size > 0);

    Poly top = TopPoly(stack);
    stack->size--;

    return top;
}

void StackDestroy(PolyStack* stack) {
    while (stack->size > 0) {
        Poly top = stack->content[stack->size - 1];
        PolyDestroy(&top);
        stack->size--;
    }

    free(stack->content);
}