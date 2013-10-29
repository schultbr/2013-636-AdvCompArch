/*
 * StructureDefs.h
 *
 *  Created on: Oct 22, 2013
 *      Author: brian
 */

#ifndef STRUCTUREDEFS_H_
#define STRUCTUREDEFS_H_

/* Opcode input types
 imm only 		= 1
 s1 only 		= 2
 s1, dest		= 3
 s1, imm		= 4
 s1, imm, dest		= 5
 s1, s2, imm 		= 6
 s1, s2, dest		= 7
 imm, dest		= 8

 s1,s2, dest= HI_LO 	= 9
 s1= HI_LO, dest	= 10

 s1= FCC, imm 		= 11
 s1, dest= FCC 		= 12
*/

#define		j	1
#define		jal	1
#define		jr	1
#define		jalr	3
#define		beq	7
#define		bne	7
#define		blez	4
#define		bgtz	4
#define		bltz	4
#define		bgez	4
#define		bc1f	8
#define		bc1t	8

#define		lb	5
#define		lbu	5
#define		lh	5
#define		lhu	5
#define		lw	5
//#define		l.s	5
//#define		l.d	5

#define		sb	6
#define		sh	6
#define		sw	6
//#define		s.s	6
//#define		s.d	6

#define		add	7
#define		addi	5
#define		addu	7
#define		addiu	5
#define		sub	7
#define		subu	7
#define		mult	9
#define		div	9
#define		divu	9
#define		mfhi	10
#define		mflo	10
#define		lui	8
#define		mfc1	3
#define		dmfc1	3
#define		mtc1	3
#define		dmtc1	3

#define		and	7
#define		andi	5
#define		or	7
#define		ori	5
#define		xor	7
#define		xori	5
#define		nor	7
#define		sll	5
#define		sllv	7
#define		srl	5
#define		srlv	7
#define		sra	5
#define		srav	7
#define		slt	7
#define		slti	5
#define		sltu	7
#define		sltiu	5

//#define		add.s	all floating point use .s/.d

	
#define R0	0
#define R1	1
#define R2	2
#define R3	3
#define R4	4
#define R5	5
#define R6	6
#define R7	7
#define R8	8
#define R9	9

#define R10	10
#define R11	11
#define R12	12
#define R13	13
#define R14	14
#define R15	15
#define R16	16
#define R17	17
#define R18	18
#define R19	19

#define R20	20
#define R21	21
#define R22	22
#define R23	23
#define R24	24
#define R25	25
#define R26	26
#define R27	27
#define R28	28
#define R29	29

#define R30	30
#define R31	31

#define F0	32
#define F1	33
#define F2	34
#define F3	35
#define F4	36
#define F5	37
#define F6	38
#define F7	39
#define F8	40
#define F9	41

#define F10	42
#define F11	43
#define F12	44
#define F13	45
#define F14	46
#define F15	47
#define F16	48
#define F17	49
#define F18	50
#define F19	51

#define F20	52
#define F21	53
#define F22	54
#define F23	55
#define F24	56
#define F25	57
#define F26	58
#define F27	59
#define F28	60
#define F29	61

#define F30	62

#define HI_LO	63
#define FCC	64



//define any generic structures here
struct Instruction {
	int opCode;
	int src1;
	int src2;
	int imm;
	int dest;
};

#endif /* STRUCTUREDEFS_H_ */
