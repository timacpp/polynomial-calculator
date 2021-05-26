/** @file
  Implementacja procesora kalkulatora wielomianów rzadkich wielu zmiennych

  @authors Tymofii Vedmedenko <tv433559@students.mimuw.edu.pl>
  @copyright Uniwersytet Warszawski
  @date 2021
*/

#include "calc_processor.h"
#include "poly_io.h"
#include <errno.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>

#define CHECK_NULL_PTR(p) if (!p) exit(1)

/** To jest typ uogólniający możliwe błędy operacyjne kalkulatora. */
typedef enum CalcError {
    WRONG_POLY,
    WRONG_COMMAND,
    WRONG_AT_VALUE,
    STACK_UNDERFLOW,
    WRONG_DEG_VARIABLE,
} CalcError;

/**
 * Wypisuje błąd na wyjście błędów.
 * @param[in] error : błąd
 * @param[in] line : numer linijki
 */
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
            break;
    }
}

static void ProcessZeroCommand(PolyStack* stack) {
    PushPoly(stack, PolyZero());
}

static void ProcessIsCoeffCommand(PolyStack* stack, int lineNumber) {
    if (stack->size < 1) {
        PrintError(STACK_UNDERFLOW, lineNumber);
        return;
    }

    Poly top = TopPoly(stack);
    bool topIsCoeff = PolyIsCoeff(&top);
    printf("%d\n", topIsCoeff);
}

static void ProcessIsZeroCommand(PolyStack* stack, int lineNumber) {
    if (stack->size < 1) {
        PrintError(STACK_UNDERFLOW, lineNumber);
        return;
    }

    Poly top = TopPoly(stack);
    bool topIsZero = PolyIsZero(&top);
    printf("%d\n",topIsZero);
}

static void ProcessCloneCommand(PolyStack* stack, int lineNumber) {
    if (stack->size < 1) {
        PrintError(STACK_UNDERFLOW, lineNumber);
        return;
    }

    Poly top = TopPoly(stack);
    PushPoly(stack, PolyClone(&top));
}

static void ProcessAddCommand(PolyStack* stack, int lineNumber) {
    if (stack->size < 2) {
        PrintError(STACK_UNDERFLOW, lineNumber);
        return;
    }

    Poly firstTop = PopPoly(stack);
    Poly secondTop = PopPoly(stack);

    Poly topSum = PolyAdd(&firstTop, &secondTop);
    PushPoly(stack, topSum);

    PolyDestroy(&firstTop);
    PolyDestroy(&secondTop);
}

static void ProcessMulCommand(PolyStack* stack, int lineNumber) {
    if (stack->size < 2) {
        PrintError(STACK_UNDERFLOW, lineNumber);
        return;
    }

    Poly firstTop = PopPoly(stack);
    Poly secondTop = PopPoly(stack);

    Poly topMul = PolyMul(&firstTop, &secondTop);
    PushPoly(stack, topMul);

    PolyDestroy(&firstTop);
    PolyDestroy(&secondTop);
}

static void ProcessNegCommand(PolyStack* stack, int lineNumber) {
    if (stack->size < 1) {
        PrintError(STACK_UNDERFLOW, lineNumber);
        return;
    }

    Poly top = PopPoly(stack);
    PushPoly(stack, PolyNeg(&top));
    PolyDestroy(&top);
}

static void ProcessSubCommand(PolyStack* stack, int lineNumber) {
    if (stack->size < 2) {
        PrintError(STACK_UNDERFLOW, lineNumber);
        return;
    }

    Poly firstTop = PopPoly(stack);
    Poly secondTop = PopPoly(stack);

    Poly topSub = PolySub(&firstTop, &secondTop);
    PushPoly(stack, topSub);

    PolyDestroy(&firstTop);
    PolyDestroy(&secondTop);
}

static void ProcessIsEqCommand(PolyStack* stack, int lineNumber) {
    if (stack->size < 2) {
        PrintError(STACK_UNDERFLOW, lineNumber);
        return;
    }

    Poly firstTop = PopPoly(stack);
    Poly secondTop = TopPoly(stack);

    bool topIsEq = PolyIsEq(&firstTop, &secondTop);
    printf("%d\n", topIsEq);

    PushPoly(stack, firstTop);
}

static void ProcessDegCommand(PolyStack* stack, int lineNumber) {
    if (stack->size < 1) {
        PrintError(STACK_UNDERFLOW, lineNumber);
        return;
    }

    Poly top = TopPoly(stack);
    poly_exp_t topDeg = PolyDeg(&top);
    printf("%d\n", topDeg);
}

static bool IsValidArgument(const char* source, size_t from, size_t to) {
    // Konieczny warunek poprawnego argumentu:
    // spacja przed nim, niezerowa długość, nie składa się tylko z minusu.
    if (source[from - 1] != ' ' || to - from == 0 || (source[from] == '-' && to - from == 1))
        return false;

    // Dostateczny warunek poprawnego argumentu:
    // składa się tylko z cyfr lub początkowego minusu.
    for (size_t i = from; i < to; i++) {
        bool isValidChar = (i == from && source[i] == '-') || isdigit(source[i]);
        if (!isValidChar)
            return false;
    }

    return true;
}

static void ProcessDegByCommand(PolyStack* stack, char* command, int lineNumber) {
    poly_exp_t degByIdx;
    const size_t nameLength = 6; // strlen("DEG_BY");
    const size_t commandLength = strlen(command);
    bool correctDelimeter = (nameLength == commandLength || isblank(command[nameLength]));
    bool correctArgument = correctDelimeter && IsValidArgument(command, nameLength + 1, commandLength);

    if (correctArgument && correctDelimeter)
        degByIdx = SubstringToExp(command, nameLength + 1, commandLength);

    if (!correctDelimeter) {
        PrintError(WRONG_COMMAND, lineNumber);
        return;
    } else if (!correctArgument || errno) {
        PrintError(WRONG_DEG_VARIABLE, lineNumber);
        return;
    } else if (stack->size < 1) {
        PrintError(STACK_UNDERFLOW, lineNumber);
        return;
    }

    Poly top = TopPoly(stack);
    poly_exp_t topDegBy = PolyDegBy(&top, (poly_exp_t) degByIdx);
    printf("%d\n", topDegBy);
}

static void ProcessAtCommand(PolyStack* stack, char* command, int lineNumber) {
    poly_coeff_t valueForAt;
    const size_t nameLength = 2; // strlen("AT");
    const size_t commandLength = strlen(command);
    bool correctDelimeter = (nameLength == commandLength || isblank(command[nameLength]));
    bool correctArgument = correctDelimeter && IsValidArgument(command, nameLength + 1, commandLength);

    if (correctArgument)
        valueForAt = SubstringToCoeff(command, nameLength + 1, commandLength);

    if (!correctDelimeter) {
        PrintError(WRONG_COMMAND, lineNumber);
        return;
    } else if (!correctArgument|| errno) {
        PrintError(WRONG_AT_VALUE, lineNumber);
        return;
    } else if (stack->size < 1) {
        PrintError(STACK_UNDERFLOW, lineNumber);
        return;
    }

    Poly top = PopPoly(stack);
    Poly topAt = PolyAt(&top, (poly_exp_t) valueForAt);

    PushPoly(stack, topAt);
    PolyDestroy(&top);
}

static void ProcessPrintCommand(PolyStack* stack, int lineNumber) {
    if (stack->size < 1) {
        PrintError(STACK_UNDERFLOW, lineNumber);
        return;
    }

    Poly top = TopPoly(stack);
    PolyPrint(&top);
    printf("\n");
}

static void ProcessPopCommand(PolyStack* stack, int lineNumber) {
    if (stack->size < 1) {
        PrintError(STACK_UNDERFLOW, lineNumber);
        return;
    }

    Poly top = PopPoly(stack);
    PolyDestroy(&top);
}

/**
 * Przetwarza komendę będącym napisem na działania na stosie wielomianów.
 * W przypadku nie istniejącej komendy wypisa błąd o niepoprawnej komendzie.
 * @param[in] stack : stos wielomianów
 * @param[in] command : komenda
 * @param[in] lineNumber : numer linijki
 */
static void ProcessCommand(PolyStack* stack, char* command, int lineNumber) {
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
    else if (strncmp(command, "DEG_BY", 6) == 0) // strlen("DEG_BY") == 6
        ProcessDegByCommand(stack, command, lineNumber);
    else if (strncmp(command, "AT", 2) == 0) // strlen("AT") == 2
        ProcessAtCommand(stack, command, lineNumber);
    else if (strcmp(command, "PRINT") == 0)
        ProcessPrintCommand(stack, lineNumber);
    else if (strcmp(command, "POP") == 0)
        ProcessPopCommand(stack, lineNumber);
    else
        PrintError(WRONG_COMMAND, lineNumber);
}

/**
 * Wczytuje komendę reprezentowaną jako napis do wartości przekazanego
 * wzkaźnika. Jeżeli podczas wczytania wystąpi nielegalny symbol '\0'
 * albo zastanie ukryta za długa komenda, to funkcja zwraca falsz.
 * W przeciwnym przypadku zwracana jest wartość true.
 * @param[in] command : wskaźnik do wczytania komendy
 * @return Czy udało się wczytać komendę?
 */
bool ReadCommand(char** command) {
    size_t size = 0, capacity = 1;
    char curChar = (char) getchar();
    bool validChars = true;

    while (curChar != EOF && curChar != '\n') {
        validChars &= (curChar != '\0');

        if (validChars) {
            if (size + 1 == capacity) {
                capacity = 2 * capacity + 1;
                *command = realloc(*command, capacity * sizeof(char));
                CHECK_NULL_PTR(*command);
            }

            (*command)[size++] = curChar;
        }
        curChar = (char) getchar();
    }

    if (validChars)
        (*command)[size] = '\0';

    return validChars;
}

void ProcessCommandInput(PolyStack* stack, int lineNumber) {
    // Funkcje wywołane w tym bloku mogą zmieniać stan errno, dlatego
    // przed wykonaniem nowej komędy ustawiamy go na początkową wartość.
    errno = 0;

    char* command = NULL;
    bool successfulRead = ReadCommand(&command);

    if (successfulRead)
        ProcessCommand(stack, command, lineNumber);
    else
        PrintError(WRONG_COMMAND, lineNumber);

    free(command);
}

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