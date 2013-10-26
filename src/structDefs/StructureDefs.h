/*
 * StructureDefs.h
 *
 *  Created on: Oct 22, 2013
 *      Author: brian
 */

#ifndef STRUCTUREDEFS_H_
#define STRUCTUREDEFS_H_


//define any generic structures here
struct Instruction {
	int PC;
	int opCode;
	int src1;
	int src2;
	int imm;
	int dest;

	Instruction() {
		PC = -1;
		dest = -1;
		imm = -1;
		opCode = -1;
		src1 = -1;
		src2 = -1;
	}
};

#endif /* STRUCTUREDEFS_H_ */
