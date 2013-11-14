/*
 * StructureDefs.cpp
 *
 *  Created on: Oct 29, 2013
 *      Author: jason cain
 */

#include "StructureDefs.h"

#include <stdlib.h>
#include <iostream>
#include <string.h>

  ROB_Element::ROB_Element()	//head & tail variables need created in main to point to ROB vector index
  {
	busy 		= 0;
	finished 	= 0;
	valid 		= 0;
	rename		= -1;
	OP		= 0;	//is OP a string, an int for each OP, or an int for the OP "type"
  }

  ARF_Element::ARF_Element() 
  {
	busy 		= 0;
	data 		= -1;
	rename	 	= -1;
  }

  RRF_Element::RRF_Element() 
  {
	busy 		= 0;
	valid		= 0;
	data		= 0;
	dest		= 0;
  }

  RS_Element::RS_Element() 
  {
	busy 		= 0;
	valid1	 	= 0;
	valid2		= 0;
	ready	 	= 0;
	op1		= -1;
	op2		= -1;
	reorder		= -1;
  }

  /* ------------------------------ Branch Prediction ------------------------------ */
  Branch_Predictor::Branch_Predictor()
  {
    	shiftReg 	= 0x0000;
	std::fill(predictionTable, predictionTable + 1024, 1);  //set initial prediction to '01', Not Taken
  }

  //shift function
  void Branch_Predictor::shift_left(bool bit)
  {
    	shiftReg = shiftReg << 1;			//shift 0 into lsb
    	shiftReg = shiftReg & 0x03FF; 			//only use lower 10 bits
    	if (bit == 1)
      	shiftReg = shiftReg | 0x0001;			//shift 1 into lsb
    
    	//return shift;
  }

  //hash function
  //if hash is not typecast or returns an addr greater than 10 bits, the table value will be out of range and may come back 0 or crazy value
  short Branch_Predictor::hash (int pc)	
  {
    	short hash = 0;
    	short x = (short)pc;
    	x = x & 0x03FF;		//only use lower 10 bits
    	hash = shiftReg ^ x;	//bitwise XOR

    	return hash;
  }

  //return Prediction from Table
  int Branch_Predictor::get_bp (int hashAddr)
  {
    	int x = predictionTable[hashAddr];

	return x;
  }
  
  //increment Prediction State
  void Branch_Predictor::inc_state (int hashAddr)
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
    	}
  }

  //decrement Prediction State
  void Branch_Predictor::dec_state(int hashAddr)
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
