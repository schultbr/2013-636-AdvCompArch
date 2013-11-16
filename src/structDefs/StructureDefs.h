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
	OpcodeType	code;		//opcode
	//bool	issued;			//out of RS, has been issued
	//int 	PC;			//PC

	ROB_Element() {			//constructor
		busy = false;
		finished = false;
		valid = false;
		rename = -1;
		code = NOP;	
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
//also, FU element
struct RS_Element {
	bool busy;
	bool valid1;
	bool valid2;
	bool ready;
	int op1;
	int op2;
	int reorder;
	short PTaddr;			//Prediction Table address
	bool BRoutcome;			//Branch Outcome
	bool BRprediction;		//Branch Prediction
	int PC;
	int BTaddr;			//Branch Target Address

	OpcodeType	code;		//opcode

	RS_Element() {
		busy = false;
		valid1 = false;
		valid2 = false;
		ready = false;
		op1 = -1;
		op2	= -1;
		reorder	= -1;
		code = NOP;
		PTaddr = -1;
		BRoutcome = 0;
		BRprediction = 0;
		PC = -1;
		BTaddr = -1;
	}
};

//Functional Unit slot
struct FU_Element {

	int count;
	int op1;
	int op2;
	int reorder;
	int result;
	OpcodeType	code;		//opcode
	short PTaddr;			//Prediction Table address
	bool BRoutcome;			//Branch Outcome
	bool BRprediction;		//Branch Prediction
	int PC;
	int BTaddr;			//Branch Target Address

	FU_Element() {
		count = 0;
		op1 = -1;
		op2	= -1;
		reorder	= -1;
		code = NOP;
		result = 0;
		PTaddr = -1;
		BRoutcome = 0;
		BRprediction = 0;
		PC = -1;
		BTaddr = -1;
	}
};

struct BTB_Element {
    int instrPC;
    int targetPC;
//    bool lastPredictedTaken; //not needed? i dont think

    BTB_Element() {
        instrPC = -1;
        targetPC = -1;
    }

};

#endif /* STRUCTUREDEFS_H_ */
