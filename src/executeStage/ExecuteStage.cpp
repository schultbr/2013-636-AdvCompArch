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

void copyToRRF(FU_Element entry){
    int reorder_tag, rename_tag;

    reorder_tag = entry.reorder;
    rename_tag = rob[reorder_tag].rename;
    rrf[rename_tag].data = entry.result;    //write result to RRF
    rrf[rename_tag].valid = 1;              //set RRF valid bit
    rob[reorder_tag].finished = 1;          //set ROB finished bit
}

void simulateExecuteStage()
{
	int next_tag = 0;
	bool done = false;

	cout << "Execute Stage\n";
	
	//integer addition/logic - 1 cycle
	for(unsigned i=0; i < fu_add.size(); i++)
	{
	    if(fu_add[i].count == 1)    //move data to Rename Register File
	        copyToRRF(fu_add[i]);

		if(fu_add[i].count > 0)     //decrement cycle count
		    fu_add[i].count--;
	}

	//integer multiply/divide - 3 cycles
	for(unsigned j=0; j< fu_mult.size(); j++)
	{
        if(fu_mult[j].count == 1)   //move data to Rename Register File
            copyToRRF(fu_mult[j]);

        if(fu_mult[j].count > 0)
            fu_mult[j].count--;
	}

	//floating point FU - 5 cycles
	for(unsigned k=0; k< fu_fp.size(); k++)
	{
        if(fu_fp[k].count == 1)     //move data to Rename Register File
            copyToRRF(fu_fp[k]);

	    if(fu_fp[k].count > 0)
	        fu_fp[k].count--;
	}

	//memory FU - 2 cycles
	for(unsigned m=0; m< fu_mem.size(); m++)
	{
        //treat mem read and mem writes the same, both access mem during execute
	    if(fu_mem[m].count == 1)                                                      //access memory
	      fu_mem[m].count  = checkCache(level1CacheHitRate, level1CacheAccessTime);   //add any cache miss penalty
	    //***** hitrate is INT in checkCache but FLOAT in globalVars  ********

	    if(fu_mem[m].count > 1)
	        fu_mem[m].count--;
	}

	//branch FU - 1 cycle 
	if(fu_br.count == 1)     //active instr in FU
	{
	    	fu_br.count = 0;    //set finished

		if (fetchStalled == true && fetchStalledInstrPC == fu_br.PC)    //if mispredicted
		fetchStalled = false;                                       //stop stalling Fetch Stage

		if (fu_br.BRoutcome == 1)                               //branch is taken
		    branchPredictor.updatePredictorWithResults(fu_br);   //update Prediction Table & BTB

		//if (fu_br.BRoutcome == fu_br.BRprediction)  //pretend to check if prediction was correct
		next_tag = fu_br.reorder;

		//when branch resolves, set instructions in ROB valid up until next branch
		while(!done)	//because of trace file, all instructions will end up being valid, no flushing from ROB
		{
			if (next_tag == (int)rob.size()-1)
				next_tag = 0;
			else
				next_tag++;

			rob[next_tag].valid = 1;

			if (rob[next_tag].code == BRANCH || next_tag == robTail)
				done = true;
		}
		//I do need to know prediction value, to set ROB entries valid if prediction was correct
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


