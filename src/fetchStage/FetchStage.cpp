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


//returns true if predicted correctly, false if we goofed.
bool checkBranchPrediction(Instruction currentInstr){
	return true;

	int nextPC = 0;
	int branchPredictedPC = 0;

	nextPC = instructionTrace.peekNextPC();
	branchPredictedPC = 0; //todo: replace this with our branch predictor
//	branchPredictedPC = branchPredict(currentInstr.PC); //something like this

	//return true if our predicted PC matches the trace's next PC
	if(branchPredictedPC == nextPC)
		return true;

	//return false otherwise. oops. we missed.
	return false;
}


//pull in n instructions
//process from strings into instruction types? or should they be in them already from the reader? derp.
//
void simulateFetchStage(std::queue<Instruction> &fetchedInstructions) {
//	cout << "FETCHING... current buff size " << fetchedInstructions.size() << endl;
//	queue<Instruction> fetchedInstructions;
	int penaltyTime = 0;
	Instruction instrToAdd;

	if(fetchStalled) {
		cout << "Fetch is stalled until instruction PC " << fetchStalledInstrPC << " has finished executing\n";
		return;
	}

	//did we previously miss a cache hit?
	//check to we're still paying any penalty
	if(cacheMissWaitTimeRemaining > 0) {
		cacheMissWaitTimeRemaining--;
		cout << "Still waiting. " << cacheMissWaitTimeRemaining << " cycles remaining.\n";
		return;
	}

	//todo: clarify if the cache hit/miss gets checked on every instr or if
	//only once per pull, since the superscalar width means you pull N instructions
	//per cycle... it makes sense you shouldn't miss the cache in the middle. right?

	//lets check to see if we get a cache hit or miss...
	penaltyTime = checkCache(instrCacheHitRate, instrCacheAccessTime);
	if(penaltyTime > 0) { //failed l1 instruction cache hit
		cout << "Cache hit missed. Checking level 2 to see how bad the penalty is.\n";
		cacheMissWaitTimeRemaining += penaltyTime;

		//check level 2 and hope we don't miss
		penaltyTime = checkCache(level2CacheHitRate, level2CacheAccessTime);

		if(penaltyTime > 0)
			cacheMissWaitTimeRemaining +=  penaltyTime;

		return;
	}
	else
		cacheMissWaitTimeRemaining = 0; //successfull hit the cache, so just reset the wait time to be safe..

	if(!instructionTrace.isTraceOpen())
		instructionTrace.openTrace(::inputTraceFile);

	cout << "Fetching  " << ::superScalarFactor << " instuctions\n";

	//loop through the remaining available spots in the queue... i.e. if we only got to move
	//2 of 4 into decode due to stalls in dispatch, we only add 2 instructions... right? or do we
	//pause all together? todo: figure this out
	for(int i = fetchedInstructions.size(); i < ::superScalarFactor; i++) {
		instrToAdd = instructionTrace.getNextInstruction();

		cout<< "Fetched: ";
		instrToAdd.Print();

		fetchedInstructions.push(instrToAdd);
		instructionCount++;

		cout << "Size: " << fetchedInstructions.size() << endl;
		//do branch checks here..
		if(instrToAdd.IsBranchOrJump()){
			if(!checkBranchPrediction(instrToAdd)){
				fetchStalledInstrPC = instrToAdd.PC;
				fetchStalled = true; //flip this to false once this mis-predicted branch finishes execution
			}
		}
	}

	return;
}
