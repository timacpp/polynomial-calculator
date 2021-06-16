/** @file
  Implementacja procesora wielomianów kalkulatora wielomianów wielu zmiennych.

  @authors Tymofii Vedmedenko <tv433559@students.mimuw.edu.pl>
  @copyright Uniwersytet Warszawski
  @date 2021
*/

#include "errno.h"

#include "calc_poly.h"
#include "calc_error.h"
#include "../polynomial/poly_io.h"

void ProcessPolyInput(PolyStack* stack, int lineNumber) {
    // Funkcje wywołane w tym bloku mogą zmieniać stan errno, dlatego
    // przed wykonaniem parsowaniem ustawiamy go na początkową wartość.
    errno = 0;

    Poly newPoly;
    bool successfulRead = ReadPoly(&newPoly);

    if (successfulRead)
        PushPoly(stack, newPoly);
    else
        PrintError(WRONG_POLY, lineNumber);
}