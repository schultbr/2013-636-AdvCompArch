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
	int opCode;
	int src1;
	int src2;
	int imm;
	int dest;
};

#endif /* STRUCTUREDEFS_H_ */
