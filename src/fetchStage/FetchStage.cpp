/*
 * FetchStage.cpp
 *
 *  Created on: Oct 26, 2013
 *      Author: brian
 */

#include "FetchStage.h"
#include "GlobalVars.h"
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

//int superScalarSize = 0;
int fetchedInstructionCount = 0;
//Instruction *fetchedInstructions;

using namespace std;

//pull in n instructions
//process from strings into instruction types? or should they be in them already from the reader? derp.
//
Instruction* simulateFetchCycle() {
	Instruction *fetchedInstructions;
	string lineToProcess = "";

	if(fetchedInstructions == NULL)
		fetchedInstructions = new Instruction[superScalarSize]();

	for(int i = 0; i < superScalarSize; i++) {

//		lineToProcess = instructionTrace.getNextTraceLine();
		fetchedInstructions[i] = instructionTrace.getNextInstruction();

		//Look up the condition that would cause the fetch to stall...
		//I can't remember what it is though.
//		if(something)
//			break;
	}

	return fetchedInstructions;
}

//Instruction getEmptyInstructon(){
//	Instruction ret;
//
//	ret.PC = -1;
//	ret.dest = -1;
//	ret.imm = -1;
//	ret.opCode = -1;
//	ret.src1 = -1;
//	ret.src2 = -1;
//
//	return ret;
//}

