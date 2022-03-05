/** @file
  Multi-variable polynomials calculator.

  @author Tymofii Vedmedenko
  @copyright University of Warsaw
  @date 2021
*/

#include <ctype.h>
#include <stdio.h>

#include "calc/calc_poly.h"
#include "calc/calc_command.h"

int PeekCharacter() {
    int nextChar = getchar();
    ungetc(nextChar, stdin);
    return nextChar;
}

bool HasNextLine() {
    int peek = PeekCharacter();
    return peek != EOF;
}

bool LineHasNoInformation() {
    int peek = PeekCharacter();
    return peek == '#' || peek == '\n';
}

bool LineRepresentsCommand() {
    int peek = PeekCharacter();
    return isalpha(peek);
}

void LineIgnore() {
    int curChar = getchar();
    while (curChar != EOF && curChar != '\n')
        curChar = getchar();
}

int main(void) {
    PolyStack stack;
    StackInitialize(&stack);

    int lineNumber = 1;

    while (HasNextLine()) {
        if (LineHasNoInformation())
            LineIgnore();
        else if (LineRepresentsCommand())
            ProcessCommandInput(&stack, lineNumber);
        else // Line represents a poly.
            ProcessPolyInput(&stack, lineNumber);

        lineNumber++;
    }

    StackDestroy(&stack);

    return 0;
}