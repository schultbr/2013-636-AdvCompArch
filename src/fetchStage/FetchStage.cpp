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

using namespace std;

int fetchedInstructionCount = 0;
TraceReader instructionTrace;

int cacheMissWaitTimeRemaining = 0;

//pull in n instructions
//process from strings into instruction types? or should they be in them already from the reader? derp.
//
std::queue<Instruction> simulateFetchCycle() {
	queue<Instruction> fetchedInstructions;
	int cacheHitRoll = 0;

	//did we previously miss a cache hit?
	//check to we're still paying any penalty
	if(cacheMissWaitTimeRemaining > 0) {
		cacheMissWaitTimeRemaining--;
		return fetchedInstructions;
	}

	if(!instructionTrace.isTraceOpen())
		instructionTrace.openTrace(inputTrace);

	for(int i = 0; i < ::superScalarFactor; i++) {
		//check to see if we get a hit in instruction cache...
		cacheHitRoll = rand() % 100;
		if(cacheHitRoll <= instrCacheHitRate ) { //successful cache hit

			cacheMissWaitTimeRemaining = 0; //successfull hit the cache, so just reset the wait time to be safe..

			fetchedInstructions.push(instructionTrace.getNextInstruction());
		}
		else { //failed level 1 instruction cache hit... try again
			cacheMissWaitTimeRemaining += instrCacheAccessTime;

			//check level 2 and hope we don't miss
			cacheHitRoll = rand() % 100;
			if(cacheHitRoll > level2CacheHitRate ) {
				//we missed the second cache hit.. wait the full amount
				cacheMissWaitTimeRemaining += level2CacheAccessTime;
			}

			return fetchedInstructions;
		}
	}

	return fetchedInstructions;
}
