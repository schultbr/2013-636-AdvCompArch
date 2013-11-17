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

void copyToRRF(FU_Element entry) {
    int reorder_tag, rename_tag;

    reorder_tag = entry.reorder;
    rename_tag = rob[reorder_tag].rename;
    rrf[rename_tag].data = entry.result;    //write result to RRF
    rrf[rename_tag].valid = 1;              //set RRF valid bit
    rob[reorder_tag].finished = 1;          //set ROB finished bit
}

void simulateExecuteStage() {
    int next_tag = 0;
    bool done = false;

    DEBUG_COUT << "Execute:\t" << "Execute Stage\n";

// ----------------------------------------------------------------------------------------------
// ------------------------- integer addition & logic FU - 1 cycle  -----------------------------
// ----------------------------------------------------------------------------------------------
    for (unsigned i = 0; i < fu_add.size(); i++) {
        DEBUG_COUT << "Execute:\t" << "Checking each add FU\n";
        if (fu_add[i].count == 1)    //move data to Rename Register File
            copyToRRF(fu_add[i]);

        if (fu_add[i].count > 0)     //decrement cycle count
            fu_add[i].count--;
    }

// ----------------------------------------------------------------------------------------------
// ------------------------- integer mult & div FU - 3 cycles -----------------------------------
// ----------------------------------------------------------------------------------------------
    for (unsigned j = 0; j < fu_mult.size(); j++) {
        DEBUG_COUT << "Execute:\t" << "Checking each mult FU\n";
        if (fu_mult[j].count == 1)   //move data to Rename Register File
            copyToRRF(fu_mult[j]);

        if (fu_mult[j].count > 0)
            fu_mult[j].count--;
    }

// ----------------------------------------------------------------------------------------------
// ------------------------- floating point FU - 5 cycles ---------------------------------------
// ----------------------------------------------------------------------------------------------
    for (unsigned k = 0; k < fu_fp.size(); k++) {
        DEBUG_COUT << "Execute:\t" << "Checking each fp FU\n";
        if (fu_fp[k].count == 1)     //move data to Rename Register File
            copyToRRF(fu_fp[k]);

        if (fu_fp[k].count > 0)
            fu_fp[k].count--;
    }

// ----------------------------------------------------------------------------------------------
// ------------------------- memory FU - 2 cycles -----------------------------------------------
// ----------------------------------------------------------------------------------------------
    for (unsigned m = 0; m < fu_mem.size(); m++) {
        DEBUG_COUT << "Execute:\t" << "Checking each mem FU\n";
        //treat mem read and mem writes the same, both access mem during execute
        if (fu_mem[m].count == 1)                                                      //access memory
            fu_mem[m].count = checkCache(level1CacheHitRate, level1CacheAccessTime);   //add any cache miss penalty
        //***** hitrate is INT in checkCache but FLOAT in globalVars  ********

        if (fu_mem[m].count > 1)
            fu_mem[m].count--;
    }

// ----------------------------------------------------------------------------------------------
// ------------------------- branch FU - 1 cycle ------------------------------------------------
// ----------------------------------------------------------------------------------------------
    if (fu_br.count == 1)     	//active instr in FU
    {
        DEBUG_COUT << "Execute:\t" << "Checking the branch FU\n";
        fu_br.count = 0;    	//set finished
        next_tag = fu_br.reorder;

        if (fetchStalled == true && fetchStalledInstrPC == fu_br.PC)    //if mispredicted
            fetchStalled = false;                                       //stop stalling Fetch Stage

        if (fu_br.BRoutcome == 1)                               	//branch is taken
            branchPredictor.updatePredictorWithResults(fu_br);   	//update Prediction Table & BTB

        //if prediction was not correct, Fetch is stalled to simulate "flushing" so there will be no new instrs
        //in the ROB that need flushed
        if (fu_br.BRoutcome == fu_br.BRprediction)  //check if prediction was correct
        {
            //when branch resolves, set instructions in ROB valid up until next branch
            while (!done)	//because of trace file, all instructions will end up being valid, no flushing from ROB
            {
                if (next_tag == (int) rob.size() - 1)
                    next_tag = 0;
                else
                    next_tag++;

                rob[next_tag].valid = 1;

                if (rob[next_tag].code == BRANCH || next_tag == robTail)
                    done = true;
            }
        }

        if (fu_br.reorder == unresolvedBranchRobIndex)	//check if ROB has additional unresolved branches
            anyUnresolvedBranches = false;		//used in Dispatch to set new ROB entries valid or invalid

    }
}

/* 	how do we know outcome of branch execution?  is this value stored with instr during Fetch, along with the prediction?
 if (fu_br.BRoutcome == 1)		//branch is taken
 {
 branchPredictor.updatePredictorWithResults(fu_br)	//increment state machine
 if (prediction == 1 || prediction == 0)	//this variable if global may have changed, need a way to track the prediction made for specific instr
 //we could just look up the prediction table again, but would need to attach the hash addr to the instr
 {
 //prediction wrong, flush instr
 }
 else
 {
 //prediction right, make ROB entries valid
 }
 }

 else if (branch == false)	//branch not taken
 {
 branch_predictor.dec_state(hashAddr)	//decrement state machine
 if (prediction == 2 || prediction == 3)
 {
 //prediction wrong, flush instr
 }
 else
 {
 //prediction right, make ROB entries valid
 }
 }
 */

