/*
 * StructureDefs.h
 *
 *  Created on: Oct 29, 2013
 *      Author: jason cain
 */

#ifndef STRUCTUREDEFS_H_
#define STRUCTUREDEFS_H_


enum OpcodeType{ADD_SUB_I, MULT_DIV_I, BRANCH, JUMP, LOAD, STORE, FLOATING_POINT, LOGICAL, NOP };

//moved this guy to a class to better shape what this represents, and hide
//some of the nitty-gritty (shifting and hashing ops) from the other
//pieces. should just be a black box, basically.

//class Branch_Predictor{
//  public:
//  	short 	shiftReg;
//  	int 	predictionTable[1024];
//
//	Branch_Predictor();				//constructor
//	void	shift_left(bool bit);
//	short 	hash (int pc);
//	int	get_bp (int hashAddr);
//	void 	inc_state(int hashAddr);
//	void 	dec_state(int hashAddr);
//};


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
		busy = 0;
		finished = 0;
		valid = 0;
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
		busy = 0;
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
		busy = 0;
		valid = 0;
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
	int reorder;
	short PTaddr;
	bool BRoutcome;
	int PC;
	int BTaddr;
	OpcodeType code; //opcode

	RS_Element() {
		busy = 0;
		valid1 = 0;
		valid2 = 0;
		ready = 0;
		op1 = -1;
		op2	= -1;
		reorder	= -1;
		code =  NOP;
		PTaddr = 0;
		BRoutcome = 0;
		PC = 0;
		BTaddr = 0;
	}
};

//Functional Unit slot
struct FU_Element {
	bool ready;
	int count;
	int op1;
	int op2;
	int	reorder;
    short PTaddr;
    bool BRoutcome;
    int PC;
    int BTaddr;
	OpcodeType code; //opcode

	FU_Element() {
		count = 0;
		ready = 0;
		op1 = -1;
		op2	= -1;
		reorder	= -1;
		code = NOP;
        PTaddr = 0;
        BRoutcome = 0;
        PC = 0;
        BTaddr = 0;
	}
};

struct BTB_Element {
    int instrPC;
    int targetPC;
    bool lastPredictedTaken;

    BTB_Element() {
        instrPC = -1;
        targetPC = -1;
    }

};

#endif /* STRUCTUREDEFS_H_ */
