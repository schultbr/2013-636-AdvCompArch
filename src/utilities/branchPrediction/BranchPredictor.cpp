/*
 * BranchPredictor.cpp
 *
 *  Created on: Nov 12, 2013
 *      Author: brian
 */

#include "BranchPredictor.h"
#include "GlobalVars.h"
#include <unistd.h>
#include <stdio.h>
#include <iostream>
#include <stdlib.h>

using namespace std;

BranchPredictor::BranchPredictor() {
	shiftReg 	= 0x0000;
	std::fill(predictionTable, predictionTable + 1024, 1);  //set initial prediction to '01', Not Taken
	btbInsertIndex = 0;
	branchPredictionCount = 0;
	predictionMissCount = 0;

}

BranchPredictor::~BranchPredictor() {
}

void BranchPredictor::incrementPredictionMissCount() {
    predictionMissCount++;
}

void BranchPredictor::resizeBTB(int size) {
    btb.resize(size);
}

//returns true if we were able to predict it at all, and sets predictedTargetPC to either currPC+8 or TargetPC(offset)
//          then, fetch can check predictedTargetPC with the real next PC to determine if we hit or miss th prediction
//returns false if btb didn't have an entry, so we have to call that a mis-predict.
bool BranchPredictor::getPredictionForInstruction(Instruction &instrToPredict){
	instrToPredict.branchPredictorTableAddress = hash(instrToPredict.PC);

	int predictionTableState = predictionTable[instrToPredict.branchPredictorTableAddress];
	instrToPredict.wasBranchPredictedAsTaken = false;

	branchPredictionCount++;

	if(predictionTableState == 2 || predictionTableState == 3) //predict this branch is taken
	    instrToPredict.wasBranchPredictedAsTaken = true;

	instrToPredict.predictedTargetPC = -1;

	//else, well, it's already false
	if(instrToPredict.predictedTargetPC) {
	    for(size_t i = 0; i < btb.size(); i++) {
	        if(btb[i].instrPC == instrToPredict.PC) {
	            instrToPredict.predictedTargetPC = btb[i].targetPC;
	            return true;
	        }
	    }

	    if(instrToPredict.predictedTargetPC == -1){
//	        instrToPredict.predictedTargetPC = instrToPredict.PC + 8;
	        return false;
	    }

	}
	else {
	    instrToPredict.predictedTargetPC = instrToPredict.PC + 8;
	    return true;
	}

	return false; //if we got here, stuff is super screwed up.

//  	predictAddr = hash(shiftreg, instr.PC)	//do we add PC to struct instr?  when br is exec if wrong we will need the PC to calc new fetch addr
//  	prediction = predicitonTable[predictAddr];
//  	if (prediction == 2 || prediction == 3)	//predict Taken
//	{
//    		if (next_instr.PC == instr.PC+8)		//Trace not Taken
//			//don't bring in instr, this simulates flushing later
//  		else if (prediction == 0 || prediction == 1)//predict Not Taken
//		{
//    			if (next_instr.PC != instr.PC=8)		//Trace Taken
//				//don't bring in instr
//  			else
//				//prediction correct, bring in next instr
//		}
//	}
}

//void BranchPredictor::updatePredictionWithResults(Instruction &executedInstr){
void BranchPredictor::updatePredictorWithResults(FU_Element entry){
    DEBUG_COUT << "Predictor:\t Updating predictor with branch execution results" << endl;
	if(entry.BRoutcome) {
		//update the state machine with the current results
		inc_state(entry.PTaddr);
	}
	else {
		dec_state(entry.PTaddr);
	}

	updateBTBRecord(entry.PC, entry.BTaddr, entry.BRoutcome);
}


//////////////////////////////////////////////////////////////
//							                            	//
//		Private functions. Not needed by anyone else	    //
//								                            //
//////////////////////////////////////////////////////////////
//shift function
void BranchPredictor::shift_left(bool bit)
{
	shiftReg = shiftReg << 1;			//shift 0 into lsb
	shiftReg = shiftReg & 0x03FF; 			//only use lower 10 bits
	if (bit == 1)
	    shiftReg = shiftReg | 0x0001;			//shift 1 into lsb
}

//hash function
//if hash is not typecast or returns an addr greater than 10 bits, the table value will be out of range and may come back 0 or crazy value
short BranchPredictor::hash (int pc)
{
	short hash = 0;
	int y = pc >> 3; //per 11/14/2013 class, ignore lowest 3 bits due to being multiples of 8.. they'd be the same each time.
	short x = (short)y;
	x = x & 0x03FF;		//only use lower 10 bits
	hash = shiftReg ^ x;	//bitwise XOR

	return hash;
}

//return Prediction from Table
int BranchPredictor::get_bp (int hashAddr)
{
	return predictionTable[hashAddr];
}

//increment Prediction State
void BranchPredictor::inc_state (int hashAddr)
{
	int x = predictionTable[hashAddr];
	switch(x)			//increment state machine
	{
		case 0:
			predictionTable[hashAddr] = 1;
			break;
		case 1:
			predictionTable[hashAddr] = 2;
			break;
		case 2:
			predictionTable[hashAddr] = 3;
			break;
		case 3:
			predictionTable[hashAddr] = 3;
			break;
		default:
			break; //should we bother with a default?
	}
}

//decrement Prediction State
void BranchPredictor::dec_state(int hashAddr)
{
	int y = predictionTable[hashAddr];
	switch(y)			//decrement state machine
	{
		case 0:
			predictionTable[hashAddr] = 0;
			break;
		case 1:
			predictionTable[hashAddr] = 0;
			break;
		case 2:
			predictionTable[hashAddr] = 1;
			break;
		case 3:
			predictionTable[hashAddr] = 2;
			break;
		default:
			break; //should we bother with a default?
	}
}


void BranchPredictor::updateBTBRecord(int instrPC, int brachTarget, bool wasTaken) {
    for(size_t i = 0; i < btb.size(); i++) {
        if(btb[i].instrPC == instrPC) {
//            btb[i].lastPredictedTaken = wasTaken;
            btb[i].targetPC = brachTarget;
            return;
        }
    }

    btb[btbInsertIndex].instrPC = instrPC;
//    btb[btbInsertIndex].lastPredictedTaken = wasTaken;
    btb[btbInsertIndex].targetPC = brachTarget;
    btbInsertIndex++; //next entry

    if(btbInsertIndex == (int)btb.size()) {
        btbInsertIndex = 0; //if we reached the max size, wrap around to 0;
    }


}
