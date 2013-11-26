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

struct FetchPipelineItem {
    int cyclesUntilReturned;
    std::queue<Instruction> instructions;
};

std::vector<FetchPipelineItem> instructionsInPipeline;

//returns true if predicted correctly, false if we goofed.
bool checkBranchPrediction(Instruction &currentInstr) {
    int actualNextPC = instructionTrace.peekNextPC();

    //did our branch predictor say to take the branch *AND* was it found in the btb, then true. Otherwise false.
    currentInstr.wasBranchPredictedAsTaken = branchPredictor.getPredictionForInstruction(currentInstr);

    //did the trace actually take the prediction?
    currentInstr.wasBranchActuallyTaken = !((currentInstr.PC + 8) == actualNextPC);

    //did we predict taken _and_ did the branch actually get taken? -OR-
    //did we predict NT and the branch actually wasn't taken??  Correct! Return true.
    if((currentInstr.wasBranchPredictedAsTaken && currentInstr.wasBranchActuallyTaken) ||
            (!(currentInstr.wasBranchPredictedAsTaken) && !(currentInstr.wasBranchActuallyTaken))){
        return true; //Correct
    }
    else {
        branchPredictor.incrementPredictionMissCount();
        return false; //MISSED
    }
}

void grabNextInstructionGroup() {
    FetchPipelineItem currentFetchedItem;

    bool isPredictionCorrect = false;

    if (endOfTraceReached) {
        return;
    }

    int penaltyTime = 0;
    Instruction instrToAdd;

    if (!instructionTrace.isTraceOpen()) {
        if(instructionTrace.openTrace(::inputTraceFile) < 1) {
            return; //cant be opened... sheesh.
        }
    }

    //loop through the remaining available spots in the queue... i.e. if we only got to move
    //2 of 4 into decode due to stalls in dispatch, we only add 2 instructions...
    //per discussion in class 11/14/2013 -> pipeline buffers fill up if dispatch fills up.
    for (int i = 0; i < ::superScalarFactor; i++) {
        //lets check to see if we get a cache hit or miss...
        currentFetchedItem.cyclesUntilReturned = ::level1CacheAccessTime; //we always have instrCacheAccessTime
        penaltyTime = checkCache(instrCacheHitRate, level2CacheAccessTime);

        if (penaltyTime > 0) { //failed l1 instruction cache hit
            DEBUG_COUT("Cache hit missed. Checking level 2 to see how bad the penalty is.\n");
            cacheMissWaitTimeRemaining += penaltyTime + level1CacheAccessTime;

            //check level 2 and hope we don't miss
            penaltyTime = checkCache(level2CacheHitRate, systemMemoryAccessTime);

            if (penaltyTime > 0)
                cacheMissWaitTimeRemaining += penaltyTime;

            // the instruction needs to have the same wait time, even though the stage is monitoring it
            // so that when the time is up, we have instructions ready to go, but not before then.
            // this allows us to flush any previously buffered sets while the failing one has to wait before it's flushed
            // similarly, the cacheMissWaitTimeRemaining allows us to 'stall' the whole cycle from pulling in more
            // sets.
            currentFetchedItem.cyclesUntilReturned = cacheMissWaitTimeRemaining;
            instructionsInPipeline.push_back(currentFetchedItem);
            return;
        }

        //reset bool
        isPredictionCorrect = false;

        instrToAdd = instructionTrace.getNextInstruction();

        //we got an empty instruction due to being at the end of the file. big day.
        if (instrToAdd.getIsEOF())
            break;

        //do branch checks here..
        if (instrToAdd.IsBranch()) {
            isPredictionCorrect = checkBranchPrediction(instrToAdd);
        }

        currentFetchedItem.instructions.push(instrToAdd);
        instructionCount++;

        if (instrToAdd.IsBranch() && !isPredictionCorrect) {
            fetchStalledInstrPC = instrToAdd.PC;
            fetchStalled = true; //flip this to false once this mis-predicted branch finishes execution
            instructionsInPipeline.push_back(currentFetchedItem);
            return;
        }
    }

    //got all N instructions, add it to the list to start decrementing it for cache latency purposes.
    instructionsInPipeline.push_back(currentFetchedItem);
    return;
}

void decrementAllPipelineInstructions(std::queue<Instruction> &fetchedInstructions) {
    //for each set of instructions currently waiting on cache latency, let's decrement
    //one cycle from their counters
    for (size_t i = 0; i < instructionsInPipeline.size(); i++) {
        if (instructionsInPipeline[i].cyclesUntilReturned > 0) //make sure we don't go negative
            instructionsInPipeline[i].cyclesUntilReturned--;
    }

    string boolCheck = (instructionsInPipeline[0].cyclesUntilReturned == 0 ? "true" : "false");

    //start to flush the front instruction if it's time is up.
    if (instructionsInPipeline[0].cyclesUntilReturned == 0) {

        while ((int) fetchedInstructions.size() < ::superScalarFactor && instructionsInPipeline[0].instructions.size() > 0) {
            fetchedInstructions.push(instructionsInPipeline[0].instructions.front()); //move over one at a time, in case of partial filled downstream stages
            instructionsInPipeline[0].instructions.pop();
        }

        //if the front set of instructions is empty, get rid of them from our list
        if (instructionsInPipeline[0].instructions.size() == 0)
            instructionsInPipeline.erase(instructionsInPipeline.begin()); // if we've got a return value, pop it off the front.
    }
}

//pull in n instructions
void simulateFetchStage(std::queue<Instruction> &fetchedInstructions) {

    if (!isFetchFinished && instructionsInPipeline.size() == 0 && endOfTraceReached) {
        isFetchFinished = true;
        cout << "Fetch is now finished" << endl;
        return;
    }
    else if (isFetchFinished)
        return;

    if ((int) fetchedInstructions.size() == ::superScalarFactor) {
        DEBUG_COUT("Error: Fetch buffer full at start of stage. Dispatch must be stalled. Waiting until that's kosher\n");
        return; //we're stalled due to dispatch
    }

    //ONLY grab new instructions if:
    //  we didn't mis-predict,
    //  a mis-predicted branch didn't finish execution in this same cycle -AND-
    //  we're not waiting on the cache
    if (fetchStalled) { //we mis-predicted a branch. it hurt.
        cacheMissWaitTimeRemaining = 0;
        isResumeNextCycle = false;
        //don't return, we need to decrement still. only new instruction fetching is stalled
    }
    else if (!fetchStalled && !isResumeNextCycle) { //yay! fetch can resume next cycle!
        isResumeNextCycle = true;
        //don't return, we need to decrement still. only new instruction fetching is stalled
    }
    else if (cacheMissWaitTimeRemaining > 0) {
        //did we previously miss a cache hit?
        //check to we're still paying any penalty
        cacheMissWaitTimeRemaining--;
        DEBUG_COUT("Still waiting. " << cacheMissWaitTimeRemaining << " cycles remaining before fetching again.\n");
        //don't return, we need to decrement still. only new instruction fetching is stalled
    }
    else if ((int) fetchedInstructions.size() == 0 && instructionsInPipeline.size() == 0)	    //add another instruction set to the group, if we can. if size is > 0, we shoudln't grab more
        grabNextInstructionGroup(); //prevent us from grabbing too many instructions by subtracting buffersize from ss_size

    //always continue to grab more, unless fetched instructions is full already. then we stop.
    if (instructionsInPipeline.size() > 0) {
        DEBUG_COUT("Fetch:\t" << "Working through the cache latencies in the pipeline\n");
        //move through and decrement all instruction groups. If one hits 0 cycles remaining,
        decrementAllPipelineInstructions(fetchedInstructions);
    }

    return;
}
