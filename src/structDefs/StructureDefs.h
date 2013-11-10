/*
 * StructureDefs.h
 *
 *  Created on: Oct 22, 2013
 *      Author: brian
 */

#ifndef STRUCTUREDEFS_H_
#define STRUCTUREDEFS_H_


enum OpcodeType{ADD_SUB_I, MULT_DIV_I, BRANCH, JUMP, LOAD, STORE, FLOATING_POINT, LOGICAL, NOP };



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
