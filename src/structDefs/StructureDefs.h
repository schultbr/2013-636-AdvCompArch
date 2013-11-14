/*
 * StructureDefs.h
 *
 *  Created on: Oct 29, 2013
 *      Author: jason cain
 */

#ifndef STRUCTUREDEFS_H_
#define STRUCTUREDEFS_H_


enum OpcodeType{ADD_SUB_I, MULT_DIV_I, BRANCH, JUMP, LOAD, STORE, FLOATING_POINT, LOGICAL, NOP };

//is this type needed? would it be helpful in issue?
enum ReservationStationType{INTEGER_RS, FLOATING_POINT_RS, MEMORY_RS, BRANCH_RS, NO_RS};

//the following can just be structs with constructors... a quick-and-dirty class,
//essentially, since that's all we needed anyways.
//Reorder Buffer slot
struct ROB_Element {
	bool busy;		//busy bit, this entry in use
	bool finished;	//out of FU, has finished execution
	bool valid;		//instr after a br are speculative, valid=0 by default
	int	rename;		//Rename Register File tag
	std::string	OP;	//opcode
	OpcodeType opCode; //opcode enum (for dispatching)
	//bool	issued;	//out of RS, has been issued
	//int 	PC;		//PC

	ROB_Element(){				//constructor
		busy = false;
		finished = false;
		valid = false;
		rename = -1;
		OP = "NOP";	//is OP a string, an int for each OP, or an int for the OP "type"
		opCode = NOP;
	}
};

//Architecture Register File slot
struct ARF_Element {
	bool busy;
	int	data;
	int	rename;

	ARF_Element() {
		busy = false;
		data = -1;
		rename = -1;
	}
};

//Rename Register File slot
struct RRF_Element {
	bool busy;
	bool valid;
	int	data;
	int	dest;

	RRF_Element() {
		busy = false;
		valid = false;
		data = 0;
		dest = 0;
	}
};

//Reservation Station slot
struct RS_Element {
	bool busy;
	bool valid1;
	bool valid2;
	bool ready;
	int op1;
	int op2;
	int	reorder;
	int cpuCyclesRemaining;

	RS_Element() {
		busy = false;
		valid1 = false;
		valid2 = false;
		ready = false;
		op1 = -1;
		op2	= -1;
		reorder	= -1;
		cpuCyclesRemaining = -1;
	}
};




#endif /* STRUCTUREDEFS_H_ */
