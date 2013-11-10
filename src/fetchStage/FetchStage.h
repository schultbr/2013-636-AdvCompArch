/*
 * FetchStage.h
 *
 *  Created on: Oct 26, 2013
 *      Author: brian
 */

#ifndef FETCHSTAGE_H_
#define FETCHSTAGE_H_

#include "Instruction.h"
#include <queue>

//extern int superScalarSize;

void simulateFetchCycle(std::queue<Instruction> &fetchedInstructions);

#endif /* FETCHSTAGE_H_ */
