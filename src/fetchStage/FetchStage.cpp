/*
 * FetchStage.cpp
 *
 *  Created on: Oct 26, 2013
 *      Author: brian
 */

#include "FetchStage.h"
#include "TraceReader.h"
#include "GlobalVars.h"
#include "CacheController.h"
#include <unistd.h>
#include <stdio.h>
#include <iostream>
#include <stdlib.h>

using namespace std;

int fetchedInstructionCount = 0;
TraceReader instructionTrace;

int cacheMissWaitTimeRemaining = 0;

//pull in n instructions
//process from strings into instruction types? or should they be in them already from the reader? derp.
//
std::queue<Instruction> simulateFetchCycle() {
	cout << "FETCHING\n";
	queue<Instruction> fetchedInstructions;
	int penaltyTime = 0;

	//did we previously miss a cache hit?
	//check to we're still paying any penalty
	if(cacheMissWaitTimeRemaining > 0) {
		cacheMissWaitTimeRemaining--;
		cout << "Still waiting. " << cacheMissWaitTimeRemaining << " cycles remaining.\n";
		return fetchedInstructions;
	}

	if(!instructionTrace.isTraceOpen())
		instructionTrace.openTrace(::inputTraceFile);

	for(int i = 0; i < ::superScalarFactor; i++) {
		//check to see if we get a hit in instruction cache...
		penaltyTime = checkCache(instrCacheHitRate, instrCacheAccessTime);
		if(penaltyTime == 0) { //successful cache hit

			cacheMissWaitTimeRemaining = 0; //successfull hit the cache, so just reset the wait time to be safe..

			fetchedInstructions.push(instructionTrace.getNextInstruction());
		}
		else { //failed level 1 instruction cache hit... try again
			cout << "Cache hit missed. Checking level 2 to see how bad the penalty is.\n";
			cacheMissWaitTimeRemaining += penaltyTime;

			//check level 2 and hope we don't miss
			penaltyTime = checkCache(level2CacheHitRate, level2CacheAccessTime);

			if(penaltyTime > 0)
				cacheMissWaitTimeRemaining +=  penaltyTime;

			return fetchedInstructions;
		}
	}

	return fetchedInstructions;
}
