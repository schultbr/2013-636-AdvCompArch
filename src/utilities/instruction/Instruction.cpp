/*
 * Instruction.cpp
 *
 *  Created on: Oct 29, 2013
 *      Author: brian
 */

#include "Instruction.h"
#include "GlobalVars.h"
#include <iostream>
#include <string>
#include <sstream>
#include <algorithm>
#include <iterator>

using namespace std;

std::map<std::string, OpcodeType> Instruction::opcodeTypeMap;
std::map<std::string, int> Instruction::instructionTypeMap;

Instruction::Instruction() {
    PC = -1;
    dest = -1;
    imm = -1;
    opCodeStr = "";
    opCodeType = -1;
    src1 = -1;
    src2 = -1;
    offset = 0;
    isBranchOrJump = false;
    branchPredictorAddress = -1;
    opCode = NOP;

    if(opcodeTypeMap.size() == 0 && instructionTypeMap.size() == 0)
    	FillMaps();
}

Instruction::Instruction(string line) {
    //init everything to -1
	PC = -1;
    dest = -1;
    imm = -1;
    opCodeStr = "";
    opCodeType = -1;
    src1 = -1;
    src2 = -1;
    offset = 0;
    branchPredictorAddress = -1;
    isBranchOrJump = false;
    opCode = NOP; //initial opCode. Gets set in decode.

    if(opcodeTypeMap.size() == 0 && instructionTypeMap.size() == 0)
		FillMaps();

    SplitPCandString(line);
}

Instruction::~Instruction() {
    // TODO Auto-generated destructor stub
}

bool Instruction::IsBranchOrJump(){
	return isBranchOrJump;
}

void Instruction::Print() {
    cout << "PC:\t" << PC << endl;
    cout << "dest:\t" << dest << endl;
    cout << "imm:\t" << imm << endl;
    cout << "op:\t" << opCode << endl;
    cout << "src1:\t" << src1 << endl;
    cout << "src2:\t" << src2 << endl;
}


string Instruction::ToString() {
    ostringstream stream;

    stream << "PC:\t" << PC << endl;
    stream << "dest:\t" << dest << endl;
    stream << "imm:\t" << imm << endl;
    stream << "op:\t" << opCode << endl;
    stream << "src1:\t" << src1 << endl;
    stream << "src2:\t" << src2 << endl;

    return stream.str();
}

//not reaaaally used...
int Instruction::OpcodeTypeLookup(string code) {

	return opcodeTypeMap[code];
}

//not reaaaally used...
int Instruction::InstructionTypeLookup(string code) {

	return instructionTypeMap[code];
}

void Instruction::SplitPCandString(string line) {
	string::size_type spacePos;
	string pcStr;
	string instStr;

	spacePos = line.find_first_of(" ");
	if(spacePos == string::npos) //if we mess this up, everythings screwed up.
		exit(-1);

	pcStr =line.substr(0,spacePos);

	instructionLine = line.substr(spacePos+1);

	//partial decode to know if we need to hit a predictor
	if(instructionLine.find("br") != string::npos)
		isBranchOrJump = true;

	PC = atoi(pcStr.c_str());

}

void Instruction::DecodeInstructionString() {
	istringstream splitLine(instructionLine);
		vector<string> tokens;
		copy(istream_iterator<string>(splitLine),
			istream_iterator<string>(),
			back_inserter<vector<string> >(tokens));

//		PC = atoi(tokens[0].c_str()); // PC is decoded in fetch stage
		opCodeStr = tokens[0];
		opCode = opcodeTypeMap[opCodeStr]; // get the opcode type for FU routing later
		DecodeRegisters(tokens);
}


void Instruction::DecodeRegisters(vector<string> tokens){

	/* Opcode input instruction types
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
	switch(instructionTypeMap[opCodeStr]) {
	case 1: //imm only
		imm = atoi(tokens[1].c_str());
		break;
	case 2: // s1 only
		src1 = atoi(tokens[1].c_str());
		break;
	case 3: //s1 & dest
		src1 = atoi(tokens[1].c_str());
		dest = atoi(tokens[2].c_str());
		break;
	case 4: //s1 & imm
		src1 = atoi(tokens[1].c_str());
		imm = atoi(tokens[2].c_str());
		break;
	case 5: // s1 & imm & dest
		src1 = atoi(tokens[1].c_str());
		imm = atoi(tokens[2].c_str());
		dest = atoi(tokens[3].c_str());
		break;
	case 6: //s1 & s2 & imm
		src1 = atoi(tokens[1].c_str());
		src2 = atoi(tokens[2].c_str());
		imm = atoi(tokens[3].c_str());
		break;
	case 7://s1 & s2 & dest
		src1 = atoi(tokens[1].c_str());
		src2 = atoi(tokens[2].c_str());
		dest = atoi(tokens[3].c_str());
		break;
	case 8: //imm & dest
		imm = atoi(tokens[1].c_str());
		dest = atoi(tokens[2].c_str());
		break;
	case 9: //s1,s2, dest= HI_LO
		src1 = atoi(tokens[1].c_str());
		src2 = atoi(tokens[2].c_str());
		dest = regHILO;
		break;
	case 10: //s1= HI_LO, dest
		src1 = regHILO;
		dest = atoi(tokens[1].c_str());
		break;
	case 11: //s1= FCC, imm
		src1 = regFCC;
		imm = atoi(tokens[1].c_str());
		break;
	case 12: //s1, dest= FCC
		src1 = atoi(tokens[1].c_str());
		dest = regFCC;
		break;
	default:
		break;
	}


}

void Instruction::FillMaps() {
//	instructionMap.insert()
	instructionTypeMap["j"] = 1;
	instructionTypeMap["jal"] = 1;
	instructionTypeMap["jr"] = 1;
	instructionTypeMap["jalr"] = 3;
	instructionTypeMap["beq"] = 7;
	instructionTypeMap["bne"] = 7;
	instructionTypeMap["blez"] = 4;
	instructionTypeMap["bgtz"] = 4;
	instructionTypeMap["bltz"] = 4;
	instructionTypeMap["bgez"] = 4;
	instructionTypeMap["bc1f"] = 8;
	instructionTypeMap["bc1t"] = 8;

	instructionTypeMap["lb"] = 5;
	instructionTypeMap["lbu"] = 5;
	instructionTypeMap["lh"] = 5;
	instructionTypeMap["lhu"] = 5;
	instructionTypeMap["lw"] = 5;
	instructionTypeMap["l.s"] = 5;
	instructionTypeMap["l.d"] = 5;

	instructionTypeMap["sb"] = 6;
	instructionTypeMap["sh"] = 6;
	instructionTypeMap["sw"] = 6;
	instructionTypeMap["s.s"] = 6;
	instructionTypeMap["s.d"] = 6;

	instructionTypeMap["add"] = 7;
	instructionTypeMap["addi"] = 5;
	instructionTypeMap["addu"] = 7;
	instructionTypeMap["addiu"] = 5;
	instructionTypeMap["sub"] = 7;
	instructionTypeMap["subu"] = 7;
	instructionTypeMap["mult"] = 9;
	instructionTypeMap["div"] = 9;
	instructionTypeMap["divu"] = 9;
	instructionTypeMap["mfhi"] = 10;
	instructionTypeMap["mflo"] = 10;
	instructionTypeMap["lui"] = 8;
	instructionTypeMap["mfc1"] = 3;
	instructionTypeMap["dmfc1"] = 3;
	instructionTypeMap["mtc1"] = 3;
	instructionTypeMap["dmtc1"] = 3;

	instructionTypeMap["and"] = 7;
	instructionTypeMap["andi"] = 5;
	instructionTypeMap["or"] = 7;
	instructionTypeMap["ori"] = 5;
	instructionTypeMap["xor"] = 7;
	instructionTypeMap["xori"] = 5;
	instructionTypeMap["nor"] = 7;
	instructionTypeMap["sll"] = 5;
	instructionTypeMap["sllv"] = 7;
	instructionTypeMap["srl"] = 5;
	instructionTypeMap["srlv"] = 7;
	instructionTypeMap["sra"] = 5;
	instructionTypeMap["srav"] = 7;
	instructionTypeMap["slt"] = 7;
	instructionTypeMap["slti"] = 5;
	instructionTypeMap["sltu"] = 7;
	instructionTypeMap["sltiu"] = 5;

	instructionTypeMap["add.s"] = 7;
	instructionTypeMap["add.d"] = 7;
	instructionTypeMap["sub.s"] = 7;
	instructionTypeMap["sub.d"] = 7;
	instructionTypeMap["mul.s"] = 7;
	instructionTypeMap["mul.d"] = 7;
	instructionTypeMap["div.d"] = 7;
	instructionTypeMap["mov.d"] = 3;
	instructionTypeMap["neg.d"] = 3;
	instructionTypeMap["cvt.s.d"] = 3;
	instructionTypeMap["cvt.s.w"] = 3;
	instructionTypeMap["cvt.d.s"] = 3;
	instructionTypeMap["cvt.d.w"] = 3;
	instructionTypeMap["cvt.w.d"] = 3;
	instructionTypeMap["c.eq.d"] = 12;
	instructionTypeMap["c.lt.d"] = 12;
	instructionTypeMap["c.le.d"] = 12;
	instructionTypeMap["sqrt.d"] = 3;


	opcodeTypeMap["j"] = JUMP;
	opcodeTypeMap["jal"] = JUMP;
	opcodeTypeMap["jr"] = JUMP;
	opcodeTypeMap["jalr"] = JUMP;
	opcodeTypeMap["beq"] = BRANCH;
	opcodeTypeMap["bne"] = BRANCH;
	opcodeTypeMap["blez"] = BRANCH;
	opcodeTypeMap["bgtz"] = BRANCH;
	opcodeTypeMap["bltz"] = BRANCH;
	opcodeTypeMap["bgez"] = BRANCH;
	opcodeTypeMap["bc1f"] = BRANCH;
	opcodeTypeMap["bc1t"] = BRANCH;

	opcodeTypeMap["lb"] = LOAD;
	opcodeTypeMap["lbu"] = LOAD;
	opcodeTypeMap["lh"] = LOAD;
	opcodeTypeMap["lhu"] = LOAD;
	opcodeTypeMap["lw"] = LOAD;
	opcodeTypeMap["l.s"] = LOAD;
	opcodeTypeMap["l.d"] = LOAD;

	opcodeTypeMap["sb"] = STORE;
	opcodeTypeMap["sh"] = STORE;
	opcodeTypeMap["sw"] = STORE;
	opcodeTypeMap["s.s"] = STORE;
	opcodeTypeMap["s.d"] = STORE;

	opcodeTypeMap["add"] = ADD_SUB_I;
	opcodeTypeMap["addi"] = ADD_SUB_I;
	opcodeTypeMap["addu"] = ADD_SUB_I;
	opcodeTypeMap["addiu"] = ADD_SUB_I;
	opcodeTypeMap["sub"] = ADD_SUB_I;
	opcodeTypeMap["subu"] = ADD_SUB_I;
	opcodeTypeMap["mult"] = MULT_DIV_I;
	opcodeTypeMap["div"] = MULT_DIV_I;
	opcodeTypeMap["divu"] = MULT_DIV_I;
	opcodeTypeMap["mfhi"] = ADD_SUB_I; //todo: is this a mult? I have no idea. documentation doesn't mention it
	opcodeTypeMap["mflo"] = ADD_SUB_I; //todo: is this a mult? I have no idea. documentation doesn't mention it
	opcodeTypeMap["lui"] = ADD_SUB_I; //todo: is this a mult? I have no idea. documentation doesn't mention it
	opcodeTypeMap["mfc1"] = ADD_SUB_I; //todo: is this a mult? I have no idea. documentation doesn't mention it
	opcodeTypeMap["dmfc1"] = ADD_SUB_I; //todo: is this a mult? I have no idea. documentation doesn't mention it
	opcodeTypeMap["mtc1"] = ADD_SUB_I; //todo: is this a mult? I have no idea. documentation doesn't mention it
	opcodeTypeMap["dmtc1"] = ADD_SUB_I; //todo: is this a mult? I have no idea. documentation doesn't mention it

	opcodeTypeMap["and"] = LOGICAL;
	opcodeTypeMap["andi"] = LOGICAL;
	opcodeTypeMap["or"] = LOGICAL;
	opcodeTypeMap["ori"] = LOGICAL;
	opcodeTypeMap["xor"] = LOGICAL;
	opcodeTypeMap["xori"] = LOGICAL;
	opcodeTypeMap["nor"] = LOGICAL;
	opcodeTypeMap["sll"] = LOGICAL;
	opcodeTypeMap["sllv"] = LOGICAL;
	opcodeTypeMap["srl"] = LOGICAL;
	opcodeTypeMap["srlv"] = LOGICAL;
	opcodeTypeMap["sra"] = LOGICAL;
	opcodeTypeMap["srav"] = LOGICAL;
	opcodeTypeMap["slt"] = LOGICAL;
	opcodeTypeMap["slti"] = LOGICAL;
	opcodeTypeMap["sltu"] = LOGICAL;
	opcodeTypeMap["sltiu"] = LOGICAL;

	opcodeTypeMap["add.s"] = FLOATING_POINT;
	opcodeTypeMap["add.d"] = FLOATING_POINT;
	opcodeTypeMap["sub.s"] = FLOATING_POINT;
	opcodeTypeMap["sub.d"] = FLOATING_POINT;
	opcodeTypeMap["mul.s"] = FLOATING_POINT;
	opcodeTypeMap["mul.d"] = FLOATING_POINT;
	opcodeTypeMap["div.d"] = FLOATING_POINT;
	opcodeTypeMap["mov.d"] = FLOATING_POINT;
	opcodeTypeMap["neg.d"] = FLOATING_POINT;
	opcodeTypeMap["cvt.s.d"] = FLOATING_POINT;
	opcodeTypeMap["cvt.s.w"] = FLOATING_POINT;
	opcodeTypeMap["cvt.d.s"] = FLOATING_POINT;
	opcodeTypeMap["cvt.d.w"] = FLOATING_POINT;
	opcodeTypeMap["cvt.w.d"] = FLOATING_POINT;
	opcodeTypeMap["c.eq.d"] = FLOATING_POINT;
	opcodeTypeMap["c.lt.d"] = FLOATING_POINT;
	opcodeTypeMap["c.le.d"] = FLOATING_POINT;
	opcodeTypeMap["sqrt.d"] = FLOATING_POINT;

}





//#define		j	1
//#define		jal	1
//#define		jr	1
//#define		jalr	3
//#define		beq	7
//#define		bne	7
//#define		blez	4
//#define		bgtz	4
//#define		bltz	4
//#define		bgez	4
//#define		bc1f	8
//#define		bc1t	8
//
//#define		lb	5
//#define		lbu	5
//#define		lh	5
//#define		lhu	5
//#define		lw	5
////#define		l.s	5
////#define		l.d	5
//
//#define		sb	6
//#define		sh	6
//#define		sw	6
////#define		s.s	6
////#define		s.d	6
//
//#define		add	7
//#define		addi	5
//#define		addu	7
//#define		addiu	5
//#define		sub	7
//#define		subu	7
//#define		mult	9
//#define		div	9
//#define		divu	9
//#define		mfhi	10
//#define		mflo	10
//#define		lui	8
//#define		mfc1	3
//#define		dmfc1	3
//#define		mtc1	3
//#define		dmtc1	3
//
//#define		and	7
//#define		andi	5
//#define		or	7
//#define		ori	5
//#define		xor	7
//#define		xori	5
//#define		nor	7
//#define		sll	5
//#define		sllv	7
//#define		srl	5
//#define		srlv	7
//#define		sra	5
//#define		srav	7
//#define		slt	7
//#define		slti	5
//#define		sltu	7
//#define		sltiu	5
//
////#define		add.s	all floating point use .s/.d
//
//
//#define R0	0
//#define R1	1
//#define R2	2
//#define R3	3
//#define R4	4
//#define R5	5
//#define R6	6
//#define R7	7
//#define R8	8
//#define R9	9
//
//#define R10	10
//#define R11	11
//#define R12	12
//#define R13	13
//#define R14	14
//#define R15	15
//#define R16	16
//#define R17	17
//#define R18	18
//#define R19	19
//
//#define R20	20
//#define R21	21
//#define R22	22
//#define R23	23
//#define R24	24
//#define R25	25
//#define R26	26
//#define R27	27
//#define R28	28
//#define R29	29
//
//#define R30	30
//#define R31	31
//
//#define F0	32
//#define F1	33
//#define F2	34
//#define F3	35
//#define F4	36
//#define F5	37
//#define F6	38
//#define F7	39
//#define F8	40
//#define F9	41
//
//#define F10	42
//#define F11	43
//#define F12	44
//#define F13	45
//#define F14	46
//#define F15	47
//#define F16	48
//#define F17	49
//#define F18	50
//#define F19	51
//
//#define F20	52
//#define F21	53
//#define F22	54
//#define F23	55
//#define F24	56
//#define F25	57
//#define F26	58
//#define F27	59
//#define F28	60
//#define F29	61
//
//#define F30	62
//
//#define HI_LO	63
//#define FCC	64
