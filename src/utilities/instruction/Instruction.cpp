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
//    destReg = "";
    imm = -1;
    opCodeStr = "";
    opCodeType = -1;
    src1 = -1;
//    src1Reg = "";
    src2 = -1;
//    src2Reg = "";
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
    cout << "dest:\t" << dest <<  endl;//"\t destStr:\t" << destReg << endl;
    cout << "imm:\t" << imm << endl;
    cout << "op:\t" << opCode << endl;
    cout << "src1:\t" << src1 << endl;//"\t src1Str:\t" << src1Reg << endl;
    cout << "src2:\t" << src2 << endl;//"\t src2Str:\t" << src2Reg << endl;
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
	cout << "DECODING " << instructionLine << endl;
	istringstream splitLine(instructionLine);
	vector<string> tokens;
	copy(istream_iterator<string>(splitLine),
		istream_iterator<string>(),
		back_inserter<vector<string> >(tokens));

//		PC = atoi(tokens[0].c_str()); // PC is decoded in fetch stage
	opCodeStr = tokens[0];
	opCode = opcodeTypeMap[opCodeStr]; // get the opcode type for FU routing later
	DecodeRegisters(tokens[1]);
}

int Instruction::GetRegisterIndexFromName(std::string regName){
	int retVal = 0;
	int charPos = 0;
	int indexOffset = 0;
	string numberStr;

	cout << "Translating " << regName << " to a vector index" << endl;

	if(regName.length() == 0)
		return 0;

	charPos = regName.find('r');
	if(charPos == string::npos) {
		charPos = regName.find('f');
		if(charPos == string::npos)
			return -1; //we didnt find a r or f in the string... wat

		indexOffset = 32;
	}

	numberStr = regName.substr(charPos);
	cout << "Found reg " << regName << " to be #" << numberStr << endl;

	retVal = atoi(numberStr.c_str());

	cout << "Return val is " << retVal << " plus " << indexOffset << endl;

	retVal += indexOffset;

	return retVal;
}


//void Instruction::DecodeRegisters(vector<string> tokens){
void Instruction::DecodeRegisters(std::string regStr){
	vector<string> regs;
	std::stringstream ss(regStr);
	char tempChar;
	std::stringstream *token = new std::stringstream();
	while(ss >> tempChar) {
//		cout << "Adding " << tempChar << "\t";
		*token << tempChar;
		if(ss.peek() == ',' ||
				ss.peek() == '(' ) {
			cout << "Found " << token->str() << endl;
			regs.push_back(token->str());
			ss.ignore();
			delete token;
			token = new std::stringstream();
		}
	}
	cout << "Found " << token->str() << endl;
	regs.push_back(token->str());

	cout << "Total of " << regs.size() << " tokens\n";

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

//	cout << "Switching " << instructionTypeMap[opCodeStr] << endl;
	switch(instructionTypeMap[opCodeStr]) {
	case 1: //imm only
		imm = atoi(regs[0].c_str());
		break;
	case 2: // s1 only
//		src1 = atoi(regs[0].c_str());
		src1Reg = regs[0];
		break;
	case 3: //s1 & dest
//		src1 = atoi(regs[0].c_str());
		src1Reg = regs[0];
//		dest = atoi(regs[1].c_str());
		destReg = regs[1];
		break;
	case 4: //s1 & imm
//		src1 = atoi(regs[0].c_str());
		src1Reg = regs[0];
		imm = atoi(regs[1].c_str());
		break;
	case 5: // s1 & imm & dest
//		src1 = atoi(regs[0].c_str());
		src1Reg = regs[0];
		imm = atoi(regs[1].c_str());
//		dest = atoi(regs[2].c_str());
		destReg = regs[2];
		break;
	case 6: //s1 & s2 & imm
//		src1 = atoi(regs[0].c_str());
		src1Reg = regs[0];
//		src2 = atoi(regs[1].c_str());
		src2Reg = regs[1];
		imm = atoi(regs[2].c_str());
		break;
	case 7://s1 & s2 & dest
//		src1 = atoi(regs[0].c_str());
		src1Reg = regs[0];
//		src2 = atoi(regs[1].c_str());
		src2Reg = regs[1];
//		dest = atoi(regs[2].c_str());
		destReg = regs[2];
		break;
	case 8: //imm & dest
		imm = atoi(regs[0].c_str());
//		dest = atoi(regs[1].c_str());
		destReg = regs[1];
		break;
	case 9: //s1,s2, dest= HI_LO
//		src1 = atoi(regs[0].c_str());
		src1Reg = regs[0];
//		src2 = atoi(regs[1].c_str());
		src2Reg = regs[1];
		dest = regHILO;
		break;
	case 10: //s1= HI_LO, dest
		src1 = regHILO;
//		dest = atoi(regs[0].c_str());
		destReg = regs[0];
		break;
	case 11: //s1= FCC, imm
		src1 = regFCC;
		imm = atoi(regs[0].c_str());
		break;
	case 12: //s1, dest= FCC
//		src1 = atoi(regs[0].c_str());
		src1Reg = regs[0];
		dest = regFCC;
		break;
	default:
		break;
	}
//
//	src1 = GetRegisterIndexFromName(src1Reg);
//	src2 = GetRegisterIndexFromName(src2Reg);
//	dest = GetRegisterIndexFromName(destReg);
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
