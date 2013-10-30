/*
 * GlobalVars.cpp
 *
 *  Created on: Oct 26, 2013
 *      Author: brian
 */
#include "GlobalVars.h"

int superScalarSize;
TraceReader instructionTrace;

//these setters are totally unnecessary but, meh. theyre already here.
//kinda the right thing to do... sometimes
void setSuperScalarSize(int size) {
	superScalarSize = size;
}

void setInstructionTrace(char *file) {
	instructionTrace.openTrace(file);
}
