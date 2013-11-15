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

bool isResumeNextCycle = true;

int cacheMissWaitTimeRemaining = 0;

struct FetchPipelineItem{
	int cyclesUntilReturned;
	std::queue<Instruction> instructions;
};


std::vector<FetchPipelineItem> instructionsInPipeline;

//returns true if predicted correctly, false if we goofed.
bool checkBranchPrediction(Instruction &currentInstr){
	int actualNextPC = instructionTrace.peekNextPC();;
	int normalNextPC = currentInstr.PC + 8;

	bool isPredictionTaken = branchPredictor.getPredictionForInstruction(currentInstr);

	if(isPredictionTaken) {
		//return true if our predicted PC matches the trace's next PC
		if(normalNextPC == actualNextPC) {
			cout << "PC " << currentInstr.PC << " was mis-predicted. Stalling fetch until it's done!\n";
			currentInstr.SetWasBranchTaken(false);
			return false; //MISPREDICTION
		}
		else {
			cout << "PC " << currentInstr.PC << " was predicted correctly. Proceeding!\n";
			currentInstr.SetWasBranchTaken(true);
			return true; //CORRECT PREDICTION
		}
	}
	else {
		if(normalNextPC == actualNextPC) {
			cout << "PC " << currentInstr.PC << " was predicted correctly. Proceeding!\n";
			currentInstr.SetWasBranchTaken(true);
			return true; //CORRECT PREDICTION
		}
		else {
			cout << "PC " << currentInstr.PC << " was mis-predicted. Stalling fetch until it's done!\n";
			currentInstr.SetWasBranchTaken(false);
			return false; //MISPREDICTION
		}
	}

//	cout << "PC " << currentInstr.PC << " wasn't even found in the table...Gonna call this a miss\n";

	//return false otherwise. oops. we missed. somehow.
	return false;
}

void grabNextInstructionGroup() {
	//	cout << "FETCHING... current buff size " << fetchedInstructions.size() << endl;
	FetchPipelineItem currentFetchedItem;

	int penaltyTime = 0;
	Instruction instrToAdd;

	currentFetchedItem.cyclesUntilReturned = ::level1CacheAccessTime;

	//lets check to see if we get a cache hit or miss...

	if(!instructionTrace.isTraceOpen())
		instructionTrace.openTrace(::inputTraceFile);

//	cout << "Fetching  " << ::superScalarFactor << " instuctions\n";

	//loop through the remaining available spots in the queue... i.e. if we only got to move
	//2 of 4 into decode due to stalls in dispatch, we only add 2 instructions... right? or do we
	//pause all together? todo: figure this out
	//per discussion in class 11/14/2013 -> pipeline buffers fill up if dispatch fills up. if at the
	//start we see that nothing has moved out... we could not decrement the counter and that stay the same? right?

	for(int i = 0; i < ::superScalarFactor; i++) {
		penaltyTime = checkCache(instrCacheHitRate, instrCacheAccessTime);
		if(penaltyTime > 0) { //failed l1 instruction cache hit
			cout << "Cache hit missed. Checking level 2 to see how bad the penalty is.\n";
			cacheMissWaitTimeRemaining += penaltyTime;

			//check level 2 and hope we don't miss
			penaltyTime = checkCache(level2CacheHitRate, level2CacheAccessTime);

			if(penaltyTime > 0)
				cacheMissWaitTimeRemaining +=  penaltyTime;

			// the instruction needs to have the same wait time, even though the stage is monitoring it
			// so that when the time is up, we have instructions ready to go, but not before then.
			// this allows us to flush any previously buffered sets while the failing one has to wait before it's flushed
			// similarly, the cacheMissWaitTimeRemaining allows us to 'stall' the whole cycle from pulling in more
			// sets.
			currentFetchedItem.cyclesUntilReturned = cacheMissWaitTimeRemaining;
			instructionsInPipeline.push_back(currentFetchedItem);
			return;
		}

		instrToAdd = instructionTrace.getNextInstruction();

//		cout<< "Fetched: ";
//		instrToAdd.Print();

		currentFetchedItem.instructions.push(instrToAdd);
		instructionCount++;

//		cout << "Size: " << currentFetchedItem.instructions.size() << endl;
		//do branch checks here..
		if(instrToAdd.IsBranch()){
			if(!checkBranchPrediction(instrToAdd)){
				fetchStalledInstrPC = instrToAdd.PC;
				fetchStalled = true; //flip this to false once this mis-predicted branch finishes execution
				instructionsInPipeline.push_back(currentFetchedItem);
				return;
			}
		}
	}

	//got all N instructions, add it to the list to start decrementing it for cache latency purposes.
	instructionsInPipeline.push_back(currentFetchedItem);
	return;
}

void decrementAllPipelineInstructions(std::queue<Instruction> &fetchedInstructions) {

//	bool hasReturnSet = false;

	for(size_t i = 0; i < instructionsInPipeline.size(); i++) {
		if(instructionsInPipeline[i].cyclesUntilReturned > 0) //make sure we don't go negative
			instructionsInPipeline[i].cyclesUntilReturned--;

		cout << "Buffered instruction set " << i << " (PC " << instructionsInPipeline[i].instructions.front().PC << ") has " << instructionsInPipeline[i].cyclesUntilReturned <<
				" cycles before being flushed\n";
	}

	//flush the front instruction if it's time is up.
	if(instructionsInPipeline[0].cyclesUntilReturned == 0) {
		fetchedInstructions = instructionsInPipeline[0].instructions;
		instructionsInPipeline.erase(instructionsInPipeline.begin()); // if we've got a return value, pop it off the front.
	}

}


//pull in n instructions
//process from strings into instruction types? or should they be in them already from the reader? derp.
//
void simulateFetchStage(std::queue<Instruction> &fetchedInstructions) {

	if(fetchStalled) {
		cout << "Fetch is stalled until instruction PC " << fetchStalledInstrPC << " has finished executing\n";
		cacheMissWaitTimeRemaining = 0;
		isResumeNextCycle = false;
		return;
	}

	if(!fetchStalled && !isResumeNextCycle) {
	    isResumeNextCycle = true;
	    return;
	}

	//did we previously miss a cache hit?
	//check to we're still paying any penalty
	if(cacheMissWaitTimeRemaining > 0) {
		cacheMissWaitTimeRemaining--;
		cout << "Still waiting. " << cacheMissWaitTimeRemaining << " cycles remaining before fetching again.\n";
//		return;
	}
	else
		//add another instruction set to the group, if we can.
		grabNextInstructionGroup();

	if(instructionsInPipeline.size() > 0) {
	    //move through and decrement all instruction groups. If one hits 0 cycles remaining,
	    decrementAllPipelineInstructions(fetchedInstructions);
	}

	return;
}
