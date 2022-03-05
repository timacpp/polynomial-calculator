/** @file
  Implementation of a calculator command processor.

  @author Tymofii Vedmedenko
  @copyright University of Warsaw
  @date 2021
*/

#ifndef POLYNOMIALS_CALC_COMMAND_H
#define POLYNOMIALS_CALC_COMMAND_H

#include "../poly/poly_stack.h"

/**
 * Signals calc in order to read a line representing a command.
 * In case of successful parsing command is processed on @p stack,
 * otherwise a custom calculator error will be displayed.
 * @param[in] stack: stack with polynomials
 * @param[in] lineNumber : current ordinal of a line
 */
void ProcessCommandInput(PolyStack* stack, int lineNumber);

#endif //POLYNOMIALS_CALC_COMMAND_H

