/** @file
  Implementation of a calculator command processor.

  @author Tymofii Vedmedenko
  @copyright University of Warsaw
  @date 2021
*/

#include <errno.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "calc_error.h"
#include "calc_command.h"
#include "../poly/io/poly_io.h"
#include "../poly/io/numeric_parser.h"

#define CHECK_NULL_PTR(p) if (!p) exit(1)

static void ProcessZeroCommand(PolyStack* stack) {
    PushPoly(stack, PolyZero());
}

static void ProcessIsCoeffCommand(PolyStack* stack, int lineNumber) {
    if (stack->size < 1) {
        PrintError(STACK_UNDERFLOW, lineNumber);
        return;
    }

    Poly top = TopPoly(stack);
    printf("%d\n", PolyIsCoeff(&top));
}

static void ProcessIsZeroCommand(PolyStack* stack, int lineNumber) {
    if (stack->size < 1) {
        PrintError(STACK_UNDERFLOW, lineNumber);
        return;
    }

    Poly top = TopPoly(stack);
    printf("%d\n", PolyIsZero(&top));
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
    PushPoly(stack, PolyAdd(&firstTop, &secondTop));

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
    PushPoly(stack, PolyMul(&firstTop, &secondTop));

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
    PushPoly(stack, PolySub(&firstTop, &secondTop));

    PolyDestroy(&firstTop);
    PolyDestroy(&secondTop);
}

static void ProcessIsEqCommand(PolyStack* stack, int lineNumber) {
    if (stack->size < 2) {
        PrintError(STACK_UNDERFLOW, lineNumber);
        return;
    }

    Poly firstTop = PopPoly(stack);
    Poly secondTop = PopPoly(stack);
    printf("%d\n", PolyIsEq(&firstTop, &secondTop));

    PushPoly(stack, secondTop);
    PushPoly(stack, firstTop);
}

static void ProcessDegCommand(PolyStack* stack, int lineNumber) {
    if (stack->size < 1) {
        PrintError(STACK_UNDERFLOW, lineNumber);
        return;
    }

    Poly top = TopPoly(stack);
    printf("%d\n", PolyDeg(&top));
}

static bool CommandValidArgument(const char* source, size_t from) {
    size_t to = strlen(source);

    bool illegalCornerValues = source[from - 1] != ' ' || to - from == 0
            || (to - from == 1 && source[from] == '-');

    if (errno || illegalCornerValues)
        return false;

    for (size_t i = from; i < to; i++) {
        bool isValidChar = (i == from && source[i] == '-') || isdigit(source[i]);
        if (!isValidChar)
            return false;
    }

    return true;
}

static bool CommandValidDelimeter(const char* command, size_t nameLength) {
    return nameLength == strlen(command) || isblank(command[nameLength]);
}

static void ProcessDegByCommand(PolyStack* stack, char* command, int lineNumber) {
    const size_t nameLength = 6; // strlen("DEG_BY");
    const size_t commandLength = strlen(command);
    size_t degByIdx = SubstringToParameter(command, nameLength + 1, commandLength);

    if (!CommandValidDelimeter(command, nameLength)) {
        PrintError(WRONG_COMMAND, lineNumber);
        return;
    } else if (!CommandValidArgument(command, nameLength + 1)) {
        PrintError(WRONG_DEG_VARIABLE, lineNumber);
        return;
    } else if (stack->size < 1) {
        PrintError(STACK_UNDERFLOW, lineNumber);
        return;
    }

    Poly top = TopPoly(stack);
    poly_exp_t topDegBy = PolyDegBy(&top, degByIdx);
    printf("%d\n", topDegBy);
}

static void ProcessAtCommand(PolyStack* stack, char* command, int lineNumber) {
    const size_t nameLength = 2; // strlen("AT");
    const size_t commandLength = strlen(command);
    poly_coeff_t valueForAt = SubstringToCoeff(command, nameLength + 1, commandLength);

    if (!CommandValidDelimeter(command, nameLength)) {
        PrintError(WRONG_COMMAND, lineNumber);
        return;
    } else if (!CommandValidArgument(command, nameLength + 1)) {
        PrintError(WRONG_AT_VALUE, lineNumber);
        return;
    } else if (stack->size < 1) {
        PrintError(STACK_UNDERFLOW, lineNumber);
        return;
    }

    Poly top = PopPoly(stack);
    Poly topAt = PolyAt(&top, valueForAt);

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

static void ProcessComposeCommand(PolyStack* stack, char* command, int lineNumber) {
    const size_t nameLength = 7; // strlen("COMPOSE");
    const size_t commandLength = strlen(command);
    size_t composeDepth = SubstringToParameter(command, nameLength + 1, commandLength);

    if (!CommandValidDelimeter(command, nameLength)) {
        PrintError(WRONG_COMMAND, lineNumber);
        return;
    } else if (!CommandValidArgument(command, nameLength + 1)) {
        PrintError(WRONG_COMPOSE_PARAMETER, lineNumber);
        return;
    } else if (stack->size == 0 || stack->size - 1 < composeDepth) {
        PrintError(STACK_UNDERFLOW, lineNumber);
        return;
    }

    Poly topPoly = PopPoly(stack), toCompose[composeDepth];

    for (size_t i = 0; i < composeDepth; i++)
        toCompose[composeDepth - i - 1] = PopPoly(stack);
    PushPoly(stack, PolyCompose(&topPoly, composeDepth, toCompose));

    PolyDestroy(&topPoly);
    for (size_t i = 0; i < composeDepth; i++)
        PolyDestroy(&toCompose[i]);
}

/**
 * Performs a command based on its' name. If the command does not exist,
 * the custom calc error will be displayed.
 * @param[in] stack : stack wit polynomials
 * @param[in] command : name of a command to process
 * @param[in] lineNumber : ordinal of a line
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
    else if (strncmp(command, "DEG_BY", 6) == 0) // 6 == strlen("DEG_BY")
        ProcessDegByCommand(stack, command, lineNumber);
    else if (strncmp(command, "AT", 2) == 0) // 2 == strlen("AT")
        ProcessAtCommand(stack, command, lineNumber);
    else if (strcmp(command, "PRINT") == 0)
        ProcessPrintCommand(stack, lineNumber);
    else if (strcmp(command, "POP") == 0)
        ProcessPopCommand(stack, lineNumber);
    else if (strncmp(command, "COMPOSE", 7) == 0) // 7 == strlen("COMPOSE")
        ProcessComposeCommand(stack, command, lineNumber);
    else
        PrintError(WRONG_COMMAND, lineNumber);
}

/**
 * Reads a command name to a @p command. Returns true if input is legal.
 * @param[in] command : destination for reading
 * @return Is input legal?
 */
static bool ReadCommand(char** command) {
    int curChar = getchar();
    bool validChars = true;
    size_t size = 0, capacity = 1;

    while (curChar != EOF && curChar != '\n') {
        validChars &= (curChar != '\0');

        if (validChars) {
            if (size + 1 == capacity) {
                capacity = 2 * capacity + 1;
                *command = realloc(*command, capacity * sizeof(char));
                CHECK_NULL_PTR(*command);
            }

            (*command)[size++] = (char) curChar;
        }
        curChar =  getchar();
    }

    if (validChars)
        (*command)[size] = '\0';

    return validChars;
}

void ProcessCommandInput(PolyStack* stack, int lineNumber) {
    errno = 0; /* The state could have been changed during the parsing. */

    char* command = NULL;
    bool successfulRead = ReadCommand(&command);

    if (successfulRead) {
        ProcessCommand(stack, command, lineNumber);
    } else {
        PrintError(WRONG_COMMAND, lineNumber);
    }

    free(command);
}