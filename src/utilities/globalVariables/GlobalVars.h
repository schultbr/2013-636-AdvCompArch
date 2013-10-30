/*
 * GlobalVars.h
 *
 *  Created on: Oct 26, 2013
 *      Author: brian
 */

#ifndef GLOBALVARS_H_
#define GLOBALVARS_H_

#include "TraceReader.h"

extern TraceReader instructionTrace;
extern int superScalarSize;

void setSuperScalarSize(int size);
void setInstructionTrace(char *file);

#endif /* GLOBALVARS_H_ */
