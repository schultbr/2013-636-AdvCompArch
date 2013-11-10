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
	cout << "Decoding " << instuctionsToDecode.size() << " instructions\n";

	if(instuctionsToDecode.size() == 0)
		return;

	//loop through each instruction, decode it and add it to the next stage buffer...
	//might not hit all N entries if the destination buffer could not be emptied due to dispatch stall
	for(size_t i = decodedInstructions.size() ; i < instuctionsToDecode.size() && i < superScalarFactor; i++)	{
		instuctionsToDecode.front().DecodeInstructionString();

		cout << "Decoded: ";
		instuctionsToDecode.front().Print();

		decodedInstructions.push(instuctionsToDecode.front());
		instuctionsToDecode.pop();
	}
}

