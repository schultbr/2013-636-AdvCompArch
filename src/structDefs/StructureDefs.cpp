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
	code		= NOP;
  }

  FU_Element::RS_Element() 
  {
	busy 		= 0;
	op1		= -1;
	op2		= -1;
	reorder		= -1;
	code		= NOP;
	count		= 0;
  }
