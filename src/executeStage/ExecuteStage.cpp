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
    for (size_t i = 0; i < targetFUs->size(); i++) {

        if (targetFUs->at(i).count > 0) {
            return false;
        }
    }
    return true;
}

void markROBFinished(int robTag) {
    rob[robTag].finished = true;
}

void copyToRRF(FU_Element entry) {
    int reorder_tag, rename_tag;

    reorder_tag = entry.reorder;
    rename_tag = rob[reorder_tag].rename;

    //prevent things from invalidly writing to rrf... if they shouldn't be. fixing segfault.
    if(rename_tag >= 0 && rename_tag < ::renameTableEntries) {
        rrf[rename_tag].data = entry.result;    	//write result to RRF
        rrf[rename_tag].valid = true;              	//set RRF valid bit
    }

    markROBFinished(reorder_tag); 		//set ROB finished bit
}

void simulateExecuteStage() {
    int next_tag = 0;
    int reorder_tag = 0;
    bool done = false;

    if (!isExecuteFinished && isIssueFinished && checkForFinished(&fu_add) && checkForFinished(&fu_mult) && checkForFinished(&fu_mem) && checkForFinished(&fu_fp) && fu_br.count == 0) {
        cout << "Execute is now finished" << endl;
        isExecuteFinished = true;
        return;
    }
    else if (isExecuteFinished)
        return;

// ----------------------------------------------------------------------------------------------
// ------------------------- integer addition & logic FU - 1 cycle  -----------------------------
// ----------------------------------------------------------------------------------------------
    for (unsigned i = 0; i < fu_add.size(); i++) {
        if (fu_add[i].count == 1) {     //move data to Rename Register File
            copyToRRF(fu_add[i]);
            fu_add_inUse--;
        }

        if (fu_add[i].count > 0) {  //decrement cycle count
            fu_add[i].count--;
        }
    }

// ----------------------------------------------------------------------------------------------
// ------------------------- integer mult & div FU - 3 cycles -----------------------------------
// ----------------------------------------------------------------------------------------------
    for (unsigned j = 0; j < fu_mult.size(); j++) {
        if (fu_mult[j].count == 1) {    //move data to Rename Register File
            copyToRRF(fu_mult[j]);
            fu_mult_inUse--;
        }

        if (fu_mult[j].count > 0) {
            fu_mult[j].count--;
        }
    }

// ----------------------------------------------------------------------------------------------
// ------------------------- floating point FU - 5 cycles ---------------------------------------
// ----------------------------------------------------------------------------------------------
    for (unsigned k = 0; k < fu_fp.size(); k++) {
        if (fu_fp[k].count == 1) {  //move data to Rename Register File
            copyToRRF(fu_fp[k]);
            fu_fp_inUse--;
        }

        if (fu_fp[k].count > 0) {
            fu_fp[k].count--;
        }
    }

// ----------------------------------------------------------------------------------------------
// ------------------------- memory FU - 2 cycles -----------------------------------------------
// ----------------------------------------------------------------------------------------------
    int clockCount = 0;
    int clockCountTotal = 0;

    for (unsigned m = 0; m < fu_mem.size(); m++) {
        clockCount = 0;
        clockCountTotal = 0;

        //treat mem read and mem writes the same, both access mem during execute
        if (fu_mem[m].isFirstClock) { //access memory on first clock
            fu_mem[m].isFirstClock = false;

            clockCount = checkCache(::level1CacheHitRate, ::level2CacheAccessTime);   //add any cache miss penalty

            clockCountTotal = clockCount + ::level1CacheAccessTime; //we always have level 1 access time;

            if (clockCount > 0) {   //if we missed level 1 cache, see if we miss level 2
                clockCount = checkCache(level1CacheHitRate, ::systemMemoryAccessTime);
                clockCountTotal += clockCount; //add the result... either systemMemoryAccessTime or 0
            }
            fu_mem[m].count = clockCountTotal;
        }

        else {    //we already accessed cache or calculated miss penalty
            reorder_tag = fu_mem[m].reorder;
            if (fu_mem[m].count == 1) {
                fu_mem_inUse--;
                if (rob[reorder_tag].code == LOAD) {
                    copyToRRF(fu_mem[m]); 	//LOAD data into RRF and mark ROB finished
                }
                else if (rob[reorder_tag].code == STORE) {
                    markROBFinished(fu_mem[m].reorder);	//just mark ROB finished
                }
            }

            if (fu_mem[m].count > 0) {
                fu_mem[m].count--;
            }
        }
    }

// ----------------------------------------------------------------------------------------------
// ------------------------- branch FU - 1 cycle ------------------------------------------------
// ----------------------------------------------------------------------------------------------
    if (fu_br.count == 1) {     //active instr in FU

        fu_br.count = 0;    	//set finished

        //update ROB to say that we're done:
        markROBFinished(fu_br.reorder);
        fu_br_inUse--;

        if (rob[fu_br.reorder].code == BRANCH) {    //as opposed to JUMP which are already marked as finished in ROB
            next_tag = fu_br.reorder;

            //if prediction was not correct, Fetch is stalled to simulate "flushing"
            //so there will be no new instrs in the ROB that need flushed
            if (fetchStalled == true && fetchStalledInstrPC == fu_br.PC) {    //if mispredicted
                fetchStalled = false;                                       //stop stalling Fetch Stage
                fetchStalledInstrPC = -1;
            }

            branchPredictor.updatePredictorWithResults(fu_br);   	//update Prediction Table & BTB regardless if branch was taken or not

            //check if prediction was correct
            //when branch resolves, set instructions in ROB valid up until next branch
            //because of trace file, all instructions will end up being valid, no flushing from ROB
            while (!done) {
                if (next_tag == (int) rob.size() - 1)
                    next_tag = 0;
                else
                    next_tag++;

                if (next_tag != robTail)
                    rob[next_tag].valid = true;

                if (rob[next_tag].code == BRANCH || next_tag == robTail)
                    done = true;
            }


            if (fu_br.reorder == unresolvedBranchRobIndex){     //check if ROB has additional unresolved branches
                anyUnresolvedBranches = false;		//used in Dispatch to set new ROB entries valid or invalid
            }
        }
    }
}

