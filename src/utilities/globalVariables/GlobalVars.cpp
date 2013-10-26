/*
 * GlobalVars.cpp
 *
 *  Created on: Oct 26, 2013
 *      Author: brian
 */
#include "GlobalVars.h"

int superScalarSize;
TraceReader instructionTrace;

void setSuperScalarSize(int size) {
	superScalarSize = size;
}

void setInstructionTrace(char *file) {
	instructionTrace.openTrace(file);
}
