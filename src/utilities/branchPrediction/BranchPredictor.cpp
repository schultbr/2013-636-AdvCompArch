/*
 * BranchPredictor.cpp
 *
 *  Created on: Nov 12, 2013
 *      Author: brian
 */

#include "BranchPredictor.h"
#include "GlobalVars.h"
#include "Instruction.h"

BranchPredictor::BranchPredictor() {
	shiftReg 	= 0x0000;
	std::fill(predictionTable, predictionTable + 1024, 1);  //set initial prediction to '01', Not Taken
}

BranchPredictor::~BranchPredictor() {
}

bool BranchPredictor::getPredictionForInstruction(Instruction &instrToPredict){
	instrToPredict.branchPredictorAddress = hash(instrToPredict.PC);

	int prediction = predictionTable[instrToPredict.branchPredictorAddress];

	if(prediction == 2 || prediction == 3) //predict this branch is taken
		return true;
	else
		return false; //predict not taken.

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

void BranchPredictor::updatePredictionWithResults(Instruction &executedInstr){
	if(executedInstr.WasPredictionCorrect()) {
		//update the state machine with the current results
		inc_state(executedInstr.branchPredictorAddress);
	}
	else {
		dec_state(executedInstr.branchPredictorAddress);
	}
}


//////////////////////////////////////////////////////////////
//								//
//		Private functions. Not needed by anyone else	//
//								//
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
	short x = (short)pc;
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





//pseudo code for inside Fetch Stage
/*
if instr.offset != 0		//check if br instr
{
  	predictAddr = hash(shiftreg, instr.PC)	//do we add PC to struct instr?  when br is exec if wrong we will need the PC to calc new fetch addr
  	prediction = predicitonTable[predictAddr];
  	if (prediction == 2 || prediction == 3)	//predict Taken
	{
    		if (next_instr.PC == instr.PC+8)		//Trace not Taken
			//don't bring in instr, this simulates flushing later
  		else if (prediction == 0 || prediction == 1)//predict Not Taken
		{
    			if (next_instr.PC != instr.PC=8)		//Trace Taken
				//don't bring in instr
  			else
				//prediction correct, bring in next instr
		}
	}
}


//pseudo code for inside Execute Stage

if (branch == true)		//branch is taken
{
	branch_predictor.inc_state(hashAddr)	//increment state machine
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
