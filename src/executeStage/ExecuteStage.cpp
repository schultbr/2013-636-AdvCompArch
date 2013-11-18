/*
 * ExecuteStage.cpp
 *
 *  Created on: Oct 29, 2013
 *      Author: jason cain
 */

#include "ExecuteStage.h"
#include "GlobalVars.h"
#include "StructureDefs.h"
#include "CacheController.h"
#include <unistd.h>
#include <stdio.h>
#include <iostream>
#include <stdlib.h>

using namespace std;

//quick sweep to see if all of our RS's are empty.
//returns true if empty
//returns false if any element is still busy
bool checkForFinished(std::vector<FU_Element> *targetFUs) {
    for(size_t i = 0; i < targetFUs->size(); i++) {
        if(targetFUs->at(i).count > 0) {
            DEBUG_COUT << "Functional unit #" << i << " has " << targetFUs->at(i).count << " remaining\n";
            return false;
        }
    }
    return true;
}

void markROBFinished(int robTag) 
{
    rob[robTag].finished = true;
}

void copyToRRF(FU_Element entry) 
{
    int reorder_tag, rename_tag;

    reorder_tag = entry.reorder;
    rename_tag = rob[reorder_tag].rename;

    DEBUG_COUT << "Execute:\t" << "Updating " << entry.PC << " in ROB/RRF.\n";
    DEBUG_COUT << "Execute:\t" << "  ROB tag " << reorder_tag << endl;
    DEBUG_COUT << "Execute:\t" << "  RRF tag " << rename_tag << endl;

    rrf[rename_tag].data = entry.result;    //write result to RRF
    rrf[rename_tag].valid = true;              //set RRF valid bit
//    rob[reorder_tag].finished = true;          //set ROB finished bit
    markROBFinished(reorder_tag); // moved this to a function since not everything that can compelete needs the rrf...
}

void simulateExecuteStage() 
{
    int next_tag = 0;
    bool done = false;

    bool chk_add = checkForFinished(&fu_add);
    bool chk_mult = checkForFinished(&fu_add);
    bool chk_mem = checkForFinished(&fu_add);
    bool chk_fp = checkForFinished(&fu_add);
    bool chk_br = fu_br.count == 0;

    if(isIssueFinished && chk_add && chk_mult && chk_mem && chk_fp && chk_br) 
    {
        cout << "Execute is now finished" << endl;
        isExecuteFinished = true;
        return;
    }
//    else {
//        cout << "Execute:\t issueFinished? " << (isIssueFinished ? "true" : "false") <<
//                " chk_add? " << (chk_add ? "true" : "false") <<
//                " chk_mult? " << (chk_mult ? "true" : "false") <<
//                " chk_mem? " << (chk_mem ? "true" : "false") <<
//                " chk_fp? " << (chk_fp ? "true" : "false") <<
//                " chk_br? " << (chk_br ? "true" : "false") <<
//                "br_size? " << fu_br.count << endl;
//    }

    DEBUG_COUT << "Execute:\t" << "Execute Stage\n";

// ----------------------------------------------------------------------------------------------
// ------------------------- integer addition & logic FU - 1 cycle  -----------------------------
// ----------------------------------------------------------------------------------------------
    for (unsigned i = 0; i < fu_add.size(); i++) 
    {
        DEBUG_COUT << "Execute:\t" << "Checking add FU " << i << " count: " << fu_add[i].count << endl;
        if (fu_add[i].count == 1) 	//move data to Rename Register File
	{   
            copyToRRF(fu_add[i]);
            DEBUG_COUT << "Execute:\t" << "Moved add inst " << fu_add[i].PC << " to rrf" << endl;
        }

        if (fu_add[i].count > 0) 	//decrement cycle count
	{     
            fu_add[i].count--;
            DEBUG_COUT << "Execute:\t" << "Decrement add inst " << fu_add[i].PC << endl;
        }
    }

// ----------------------------------------------------------------------------------------------
// ------------------------- integer mult & div FU - 3 cycles -----------------------------------
// ----------------------------------------------------------------------------------------------
    for (unsigned j = 0; j < fu_mult.size(); j++) 
    {
        DEBUG_COUT << "Execute:\t" << "Checking mult FU " << j << " count: " << fu_mult[j].count << endl;
        if (fu_mult[j].count == 1) 	//move data to Rename Register File
	{   
            copyToRRF(fu_mult[j]);
            DEBUG_COUT << "Execute:\t" << "Moved mult inst " << fu_mult[j].PC << " to rrf" << endl;
        }

        if (fu_mult[j].count > 0) 
	{
            fu_mult[j].count--;
            DEBUG_COUT << "Execute:\t" << "Decrement mult inst " << fu_mult[j].PC <<  endl;
        }
    }

// ----------------------------------------------------------------------------------------------
// ------------------------- floating point FU - 5 cycles ---------------------------------------
// ----------------------------------------------------------------------------------------------
    for (unsigned k = 0; k < fu_fp.size(); k++) 
    {
	DEBUG_COUT << "Execute:\t" << "Checking fp FU " << k << " count: " << fu_fp[k].count << endl;
	if (fu_fp[k].count == 1) 	//move data to Rename Register File
	{    
	    copyToRRF(fu_fp[k]);
	    DEBUG_COUT << "Execute:\t" << "Moved fp inst " << fu_fp[k].PC << " to rrf" << endl;
	}

	if (fu_fp[k].count > 0) 
	{
	    fu_fp[k].count--;
	    DEBUG_COUT << "Execute:\t" << "Decrement fp inst " << fu_fp[k].PC <<  endl;
	}
    }

// ----------------------------------------------------------------------------------------------
// ------------------------- memory FU - 2 cycles -----------------------------------------------
// ----------------------------------------------------------------------------------------------
    int clockCount = 0;
    int clockCountTotal = 0;
    for (unsigned m = 0; m < fu_mem.size(); m++) 
    {
        clockCount = 0;
        clockCountTotal = 0;
        DEBUG_COUT << "Execute:\t" << "Checking mem FU " << m << " count: "<< fu_mem[m].count << endl;
        //treat mem read and mem writes the same, both access mem during execute

        if (fu_mem[m].isFirstClock) //access memory on first clock
	{ 
		DEBUG_COUT << "Execute:\t" << "Determining cache latency\n";

		clockCount = checkCache(::level1CacheHitRate, ::level2CacheAccessTime);   //add any cache miss penalty

		DEBUG_COUT << "Execute:\t" << "First cache latency check ? " << (clockCount == 0? "pass" :"failed") << endl;

		clockCountTotal = clockCount + ::level1CacheAccessTime; //we always have level 1 access time;

		if(clockCount > 0) 	//if we missed level 1 cache, see if we miss level 2
		{ 
		clockCount = checkCache(level1CacheHitRate, ::systemMemoryAccessTime);
		clockCountTotal += clockCount; //add the result... either systemMemoryAccessTime or 0
		}

		fu_mem[m].count = clockCountTotal;
		fu_mem[m].isFirstClock = false;
		DEBUG_COUT << "Execute:\t" << "Cache latency will be " << fu_mem[m].count << endl;
        }
        //todo: the above count == 1 code will always stall us... we need an entry time or
        //      something, but not a check every time we hit 1

        if (fu_mem[m].count > 1) 
	{
		fu_mem[m].count--;
		DEBUG_COUT << "Execute:\t" << "Decrement mem inst " << fu_mem[m].PC << " to " << fu_mem[m].count << endl;
        }
        else if (fu_mem[m].count == 1) {
		DEBUG_COUT << "Execute:\t" << "Marking mem inst " << fu_mem[m].PC << " complete (ROBTag " << fu_mem[m].reorder << ")" << endl;
		markROBFinished(fu_mem[m].reorder);
		fu_mem[m].count = 0;
        }
    }

// ----------------------------------------------------------------------------------------------
// ------------------------- branch FU - 1 cycle ------------------------------------------------
// ----------------------------------------------------------------------------------------------
    if (fu_br.count == 1)    	//active instr in FU
    {
        DEBUG_COUT << "Execute:\t" << "Checking the branch FU\n";

        fu_br.count = 0;    	//set finished

	if (rob[fu_br.reorder].code = BRANCH)	//as opposed to JUMP which are already marked as finished in ROB
	{
		next_tag = fu_br.reorder;

		if (fetchStalled == true && fetchStalledInstrPC == fu_br.PC)    //if mispredicted
		{
		    fetchStalled = false;                                       //stop stalling Fetch Stage
		}

		if (fu_br.BRoutcome == true)                               	//branch is taken
		{
		    branchPredictor.updatePredictorWithResults(fu_br);   	//update Prediction Table & BTB
		}

		//if prediction was not correct, Fetch is stalled to simulate "flushing" 
		//so there will be no new instrs in the ROB that need flushed
		if (fu_br.BRoutcome == fu_br.BRprediction) 
		{ 
			//check if prediction was correct
			//when branch resolves, set instructions in ROB valid up until next branch
			//because of trace file, all instructions will end up being valid, no flushing from ROB
			while (!done)	
			{
				if (next_tag == (int) rob.size() - 1)
				{
				    next_tag = 0;
				}
				else
				{
				    next_tag++;
				}

				rob[next_tag].valid = true;

				if (rob[next_tag].code == BRANCH || next_tag == robTail)
				    done = true;
			}
		}

		if (fu_br.reorder == unresolvedBranchRobIndex)	//check if ROB has additional unresolved branches
		{
		    anyUnresolvedBranches = false;		//used in Dispatch to set new ROB entries valid or invalid
		}

		//update ROB to say that we're done:
		markROBFinished(fu_br.reorder);
	}
    }
}

