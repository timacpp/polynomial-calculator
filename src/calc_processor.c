/** @file
  Implementacja procesora kalkulatora wielomianów rzadkich wielu zmiennych

  @authors Tymofii Vedmedenko <tv433559@students.mimuw.edu.pl>
  @copyright Uniwersytet Warszawski
  @date 2021
*/

#include "calc_processor.h"
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#define CHECK_NULL_PTR(p) if (!p) exit(1)

void PrintError(CalcError error, int line) {
    switch (error) {
        case WRONG_POLY:
            fprintf(stderr, "ERROR %d WRONG POLY\n", line);
            break;
        case WRONG_COMMAND:
            fprintf(stderr, "ERROR %d WRONG COMMAND\n", line);
            break;
        case WRONG_AT_VALUE:
            fprintf(stderr, "ERROR %d AT WRONG VALUE\n", line);
            break;
        case STACK_UNDERFLOW:
            fprintf(stderr, "ERROR %d STACK UNDERFLOW\n", line);
            break;
        case WRONG_DEG_VARIABLE:
            fprintf(stderr, "ERROR %d DEG BY WRONG VARIABLE\n", line);
        default:
            assert(false); // Ten fragment kodu nigdy nie wystąpi.
    }
}

/**
 * Sprawdza zgodność znaku z reprezentacją komend.
 * Jeżeli symbol wystąpił przed spacją, to musi on być
 * wielką literą angielską, podkreślnikiem lub spacją.
 * W przeciwnym przypadku musi być cyfrą albo minusem.
 * @param[in] curChar : znak
 * @param[in] spaceReached : czy została wczytana spacja?
 * @return Czy znak jest poprawną częścią komendy?
 */
static bool IsValidCommandChar(char curChar, bool spaceReached) {
    if (spaceReached)
        return isdigit(curChar) || curChar == '-';

    return (isalpha(curChar) && isupper(curChar)) || isspace(curChar) || curChar == '_';
}

bool ReadCommand(char** command) {
    size_t size = 0, capacity = 0;
    char curChar = (char) getchar();
    bool spaceReached = false; // Prawda wtw gdy spacja została już wczytana.

    while (curChar != EOF && curChar != '\n') {
        // W przypadku niedozwolonego symbolu lub je kolejności, zwalniamy
        // pamięć i zwaracamy falsz - stdin zawiera niepoprawny wielomian.
        if (!IsValidCommandChar(curChar, spaceReached)) {
            if (*command)
                free(*command);

            return false;
        }

        if (size == capacity) {
            capacity = 2 * capacity + 1;
            *command = realloc(*command, capacity * sizeof(char));
            CHECK_NULL_PTR(*command);
        }

        (*command)[size++] = curChar;
        spaceReached |= isspace(curChar); // Kwantyfikator egzystencyjalny.
        curChar = (char) getchar();
    }

    return true;
}

static void ProcessZeroCommand(PolyStack* stack) {
    PushPoly(stack, PolyZero());
}

static void ProcessIsCoeffCommand(PolyStack* stack, int lineNumber) {
    if (stack->size < 1) {
        PrintError(STACK_UNDERFLOW, lineNumber);
        return;
    }

    bool topIsCoeff = PolyIsCoeff(TopPoly(stack));
    PushPoly(stack, PolyFromCoeff(topIsCoeff));
}

static void ProcessIsZeroCommand(PolyStack* stack, int lineNumber) {
    if (stack->size < 1) {
        PrintError(STACK_UNDERFLOW, lineNumber);
        return;
    }

    bool topIsZero = PolyIsCoeff(TopPoly(stack));
    PushPoly(stack, PolyFromCoeff(topIsZero));
}

static void ProcessCloneCommand(PolyStack* stack, int lineNumber) {
    if (stack->size < 1) {
        PrintError(STACK_UNDERFLOW, lineNumber);
        return;
    }

    Poly topClone = PolyClone(TopPoly(stack));
    PushPoly(stack, topClone);
}

static void ProcessAddCommand(PolyStack* stack, int lineNumber) {
    if (stack->size < 2) {
        PrintError(STACK_UNDERFLOW, lineNumber);
        return;
    }

    Poly* firstTop = TopPoly(stack);
    PopPoly(stack);

    Poly* secondTop = TopPoly(stack);
    PopPoly(stack);

    Poly topSum = PolyAdd(firstTop, secondTop);
    PushPoly(stack, topSum);

    PolyDestroy(firstTop);
    PolyDestroy(secondTop);
}

static void ProcessMulCommand(PolyStack* stack, int lineNumber) {
    if (stack->size < 2) {
        PrintError(STACK_UNDERFLOW, lineNumber);
        return;
    }

    Poly firstTop = PolyClone(TopPoly(stack));
    PopPoly(stack);

    Poly* secondTop = TopPoly(stack);
    PopPoly(stack);

    //Poly topMul = PolyMul(firstTop, secondTop);

    PopPoly(stack);
    PopPoly(stack);

    //PushPoly(stack, topMul);
}

static void ProcessNegCommand(PolyStack* stack, int lineNumber) {
    if (stack->size < 1) {
        PrintError(STACK_UNDERFLOW, lineNumber);
        return;
    }

    Poly topNeg = PolyNeg(TopPoly(stack));
    PopPoly(stack);

    PushPoly(stack, topNeg);
}

static void ProcessSubCommand(PolyStack* stack, int lineNumber) {
    if (stack->size < 2) {
        PrintError(STACK_UNDERFLOW, lineNumber);
        return;
    }

    Poly firstTopClone = PolyClone(TopPoly(stack));
    PopPoly(stack);

    Poly secondTopClone = PolyClone(TopPoly(stack));
    PopPoly(stack);

    PushPoly(stack, PolySub(&firstTopClone, &secondTopClone));

    PolyDestroy(&firstTopClone);
    PolyDestroy(&secondTopClone);
}

static void ProcessIsEqCommand(PolyStack* stack, int lineNumber) {}
static void ProcessDegCommand(PolyStack* stack, int lineNumber) {}
static void ProcessDegByCommand(PolyStack* stack, const char* command, int lineNumber) {}
static void ProcessAtCommand(PolyStack* stack, const char* command, int lineNumber) {}
static void ProcessPrintCommand(PolyStack* stack, int lineNumber) {}
static void ProcessPopCommand(PolyStack* stack, int lineNumber) {}

void ProcessCommand(PolyStack* stack, const char* command, int lineNumber) {
    if (strcmp(command, "ZERO") == 0)
        ProcessZeroCommand(stack);
    else if (strcmp(command, "IS_COEFF") == 0)
        ProcessIsCoeffCommand(stack, lineNumber);
    else if (strcmp(command, "IS_ZERO") == 0)
        ProcessIsZeroCommand(stack, lineNumber);
    else if (strcmp(command, "CLONE") == 0)
        ProcessCloneCommand(stack, lineNumber);
    else if (strcmp(command, "ADD") == 0)
        ProcessAddCommand(stack, lineNumber);
    else if (strcmp(command, "MUL") == 0)
        ProcessMulCommand(stack, lineNumber);
    else if (strcmp(command, "NEG") == 0)
        ProcessNegCommand(stack, lineNumber);
    else if (strcmp(command, "SUB") == 0)
        ProcessSubCommand(stack, lineNumber);
    else if (strcmp(command, "IS_EQ") == 0)
        ProcessIsEqCommand(stack, lineNumber);
    else if (strcmp(command, "DEG") == 0)
        ProcessDegCommand(stack, lineNumber);
    else if (strncmp(command, "DEG_BY", strlen("DEG_BY\0")) == 0)
        ProcessDegByCommand(stack, command, lineNumber);
    else if (strncmp(command, "AT", strlen("AT\0")) == 0)
        ProcessAtCommand(stack, command, lineNumber);
    else if (strcmp(command, "PRINT") == 0)
        ProcessPrintCommand(stack, lineNumber);
    else if (strcmp(command, "POP") == 0)
        ProcessPopCommand(stack, lineNumber);
    else
        PrintError(WRONG_COMMAND, lineNumber);
}