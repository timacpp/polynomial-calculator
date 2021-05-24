/** @file
  Kalkulator wielomianów rzadkich wielu zmiennych

  @authors Tymofii Vedmedenko <tv433559@students.mimuw.edu.pl>
  @copyright Uniwersytet Warszawski
  @date 2021
*/

#include <ctype.h>
#include <stdio.h>
#include "poly_io.h"
#include "poly_stack.h"
#include "calc_processor.h"

#define IGNORE_LINE() { \
    char curChar = (char) getchar(); \
    while (curChar != EOF && curChar != '\n') \
        curChar = (char) getchar(); \
}

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

int main(void) {
    PolyStack stack;
    StackInitialize(&stack);

    int lineNumber = 1;

    while(HasNextLine()) {
        if (LineHasNoInformation()) { // Jeżeli linijka jest pusta albo zawiera komentarz,
            IGNORE_LINE(); // to możemy ją opuścić bez zwiększenia licznika lineNumber.
        } else if (LineRepresentsCommand()) {
            char* command = NULL;
            bool successfulRead = ReadCommand(&command);

            if (successfulRead)
                ProcessCommand(&stack, command, lineNumber);
            else
                PrintError(WRONG_COMMAND, lineNumber);
        } else { // Przypadek gdy linijka reprezentuje wielomian.
            Poly newPoly;
            bool successfulRead = ReadPoly(&newPoly);

            if (successfulRead)
                PushPoly(&stack, newPoly);
            else
                PrintError(WRONG_POLY, lineNumber);
        }

        lineNumber++;
    }

    StackDestroy(&stack);

    return 0;
}