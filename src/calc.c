/** @file
  Kalkulator wielomianów rzadkich wielu zmiennych.

  @authors Tymofii Vedmedenko <tv433559@students.mimuw.edu.pl>
  @copyright Uniwersytet Warszawski
  @date 2021
*/

#include <ctype.h>
#include <stdio.h>
#include "calc_processor.h"

char PeekCharacter() {
    char nextChar = (char) getchar();
    ungetc(nextChar, stdin);
    return nextChar;
}

bool HasNextLine() {
    char peek = PeekCharacter();
    return peek != EOF;
}

bool LineHasNoInformation() {
    char peek = PeekCharacter();
    return peek == '#' || peek == '\n';
}

bool NextCharacterIsLineEnd() {
    char peek = PeekCharacter();
    return peek == EOF || peek == '\n';
}

bool LineRepresentsCommand() {
    char peek = PeekCharacter();
    return isalpha(peek);
}

void LineIgnore() {
    char curChar = (char) getchar();
    while (curChar != EOF && curChar != '\n')
        curChar = (char) getchar();
}

int main(void) {
    PolyStack stack;
    StackInitialize(&stack);

    int lineNumber = 1;

    while(HasNextLine()) {
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