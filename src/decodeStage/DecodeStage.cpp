/*
 * DecodeStage.cpp
 *
 *  Created on: Oct 26, 2013
 *      Author: brian
 */

#include "DecodeStage.h"
#include "GlobalVars.h"
#include <unistd.h>
#include <stdio.h>
#include <iostream>
#include <stdlib.h>

using namespace std;

void simulateDecodeStage(std::queue<Instruction> &instuctionsToDecode, std::queue<Instruction> &decodedInstructions) {
    DEBUG_COUT << "Decoding " << instuctionsToDecode.size() << " instructions\n";

	if(instuctionsToDecode.size() == 0)
		return;

	//loop through each instruction, decode it and add it to the next stage buffer...
	//might not hit all N entries if the destination buffer could not be emptied due to dispatch stall
	//while the decoded instructions is < superScalarSize && instructions to decode > 0... decode stuff.
	while(instuctionsToDecode.size() > 0 &&
	        decodedInstructions.size() < (size_t)::superScalarFactor) {

		instuctionsToDecode.front().DecodeInstructionString();

		DEBUG_COUT << "Decoded: ";
		instuctionsToDecode.front().Print();

		decodedInstructions.push(instuctionsToDecode.front());
		instuctionsToDecode.pop();
	}
}

