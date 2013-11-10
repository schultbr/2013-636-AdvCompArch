/*
 * DecodeStage.h
 *
 *  Created on: Oct 26, 2013
 *      Author: brian
 */

#ifndef DECODESTAGE_H_
#define DECODESTAGE_H_

#include "Instruction.h"
#include <queue>


void simulateDecodeStage(std::queue<Instruction> &instuctionsToDecode, std::queue<Instruction> &decodedInstructions);

#endif /* DECODESTAGE_H_ */
