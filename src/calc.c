/** @file
  Kalkulator wielomianów rzadkich wielu zmiennych.

  @authors Tymofii Vedmedenko <tv433559@students.mimuw.edu.pl>
  @copyright Uniwersytet Warszawski
  @date 2021
*/

#include <ctype.h>
#include <stdio.h>

#include "calculator/calc_poly.h"
#include "calculator/calc_command.h"

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
        else // Linijka reprezentuje wielomian.
            ProcessPolyInput(&stack, lineNumber);

        lineNumber++;
    }

    StackDestroy(&stack);

    return 0;
}