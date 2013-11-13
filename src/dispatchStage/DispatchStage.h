/*
 * DispatchStage.h
 *
 *  Created on: Nov 12, 2013
 *      Author: brian
 */

#ifndef DISPATCHSTAGE_H_
#define DISPATCHSTAGE_H_

#include "GlobalVars.h"
#include "StructureDefs.h"
#include "Instruction.h"
#include <queue>

void simulateDispatchStage(std::queue<Instruction> &instrToDispatch);


#endif /* DISPATCHSTAGE_H_ */
