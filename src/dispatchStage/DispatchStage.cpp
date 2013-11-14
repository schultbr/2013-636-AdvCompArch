/*
 * DispatchStage.cpp
 *
 *  Created on: Nov 12, 2013
 *      Author: brian
 */

#include "DispatchStage.h"

void simulateDispatchStage(std::queue<Instruction> &instrToDispatch) {

    //Dispatch takes each instruction and sends it to:
    //	The ROB
    //	The correct RS
    //	The RRF (where logical... not mem instr or branch instr)

    for (size_t i = 0; i < instrToDispatch.size(); i++) {

    }
}

