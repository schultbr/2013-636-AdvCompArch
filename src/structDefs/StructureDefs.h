/*
 * StructureDefs.h
 *
 *  Created on: Oct 29, 2013
 *      Author: jason cain
 */

#ifndef STRUCTUREDEFS_H_
#define STRUCTUREDEFS_H_


enum OpcodeType{ADD_SUB_I, MULT_DIV_I, BRANCH, JUMP, LOAD, STORE, FLOATING_POINT, LOGICAL, NOP };

class Branch_Predictor{
  public:
  	short 	shiftReg;
  	int 	predictionTable[1024];

	Branch_Predictor();				//constructor
	void	shift_left(bool bit);
	short 	hash (int pc);
	int	get_bp (int hashAddr);
	void 	inc_state(int hashAddr);
	void 	dec_state(int hashAddr);
};

//Reorder Buffer slot
class ROB_Element {
  public:
	bool		busy;				//busy bit, this entry in use
	bool		finished;			//out of FU, has finished execution
	bool		valid;				//instr after a br are speculative, valid=0 by default
	int		rename;				//Rename Register File tag
	OpcodeType	OP;				//opcode
	//bool		issued;				//out of RS, has been issued
	//int 		PC;				//PC

	ROB_Element();					//constructor
};

//Architecture Register File slot
class ARF_Element {
public:
	bool 	busy;
	int	data;
	int	rename;

	ARF_Element();
};

//Rename Register File slot
class RRF_Element {
public:
	bool		busy;
	bool		valid;
	int		data;
	int		dest;

	RRF_Element();
};

//Reservation Station slot
class RS_Element {
public:
	bool		busy;
	bool		valid1;
	bool		valid2;
	bool		ready;
	int		op1;
	int		op2;
	int		reorder;
	RS_Element();
};



//define any generic structures here
//struct Instruction {
//	int PC;
//	int opCode;
//	int src1;
//	int src2;
//	int imm;
//	int dest;
//
//	Instruction() {
//		PC = -1;
//		dest = -1;
//		imm = -1;
//		opCode = -1;
//		src1 = -1;
//		src2 = -1;
//	}
//
//	void ToString() {
//	    printf("PC:\t%d\n", PC);
//	    printf("dest:\t%d\n", dest);
//	    printf("imm:\t%d\n", imm);
//	    printf("op:\t%d\n", opCode);
//	    printf("src1:\t%d\n", src1);
//	    printf("src2:\t%d\n", src2);
//	}
//};

#endif /* STRUCTUREDEFS_H_ */
