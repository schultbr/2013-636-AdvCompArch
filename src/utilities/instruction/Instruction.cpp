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

static const int fpRegIndex = 32;
static const int hiloRegIndex = 63;
static const int fccRegIndex = 64;

Instruction::Instruction() {
    PC = -2; //-2 to know when we are past the EOF
    dest = -1;
    imm = -1;
    opCodeStr = "";
    opCodeDecodeType = -1;
    src1 = -1;
    src2 = -1;
    offset = 0;
    isBranch = false;
    branchPredictorTableAddress = -1;
    opCode = NOP;
    wasBranchPredictedAsTaken = false;
    predictedTargetPC = -1;
    isEOF = false;

    if (opcodeTypeMap.size() == 0 && instructionTypeMap.size() == 0)
        FillMaps();
}

Instruction::Instruction(string line) {
    //init everything to -1
    PC = -1;
    dest = -1;
    imm = -1;
    opCodeStr = "";
    opCodeDecodeType = -1;
    src1 = -1;
    src2 = -1;
    offset = 0;
    branchPredictorTableAddress = -1;
    isBranch = false;
    wasBranchPredictedAsTaken = false;
    predictedTargetPC = -1;
    opCode = NOP; //initial opCode. Gets set in decode.
    isEOF = false;

    if (opcodeTypeMap.size() == 0 && instructionTypeMap.size() == 0)
        FillMaps();

    SplitPCandString(line);
}

Instruction::~Instruction() {
}

int Instruction::GetOpcodeRegisterType() {
    return opCodeDecodeType;
}

bool Instruction::IsBranch() {
    return isBranch;
}

void Instruction::setIsEOF(bool val) {
    isEOF = val;
}
bool Instruction::getIsEOF() {
    return isEOF;
}

//returns the ops corresponding to this instr... -1 is default and means "not used" and
// the valid will be set to true if using an immediate/offset value, or if the op isn't used.
// if the op is a reg reference, we set valid to false for the dispatch function to verify
void Instruction::TraslateToFUEntry(int &op1, bool &valid1, bool &isReg1, int &op2, bool &valid2, bool &isReg2, int &op3) {

    //default these guys out.
    op1 = -1;
    op2 = -1;
    valid1 = true;
    valid2 = true;

    isReg1 = false;
    isReg2 = false;

    /* Opcode input instruction types
     * 0 = IMM ONLY
     * 1 = SRC1 ONLY
     * 2 = SRC1 & SRC2
     * 3 = SRC1 & SRC2 & OFFSET
     * 4 = SRC1 & OFFSET
     * 5 = IMM & SRC1=FCC
     * 6 = DEST & IMM & SRC1 (in that order)
     * 7 = DEST & SRC1 & SRC2
     * 8 = DEST & SRC1 & IMM
     * 9 = SRC1 & SRC2, DEST = HI_LO
     * 10 = DEST && SRC1 = HI_LO
     * 11 = DEST & IMM
     * 12 = SRC1 & DEST
     * 13 = SRC1 & SRC2, DEST=FCC
     */
    switch (instructionTypeMap[opCodeStr]) {
        case 0: //imm only
        case 11: //DEST & IMM
            op1 = imm;
            break;

        case 1: //src1 only
        case 2: //dest & src 1
        case 10: //s1= HI_LO, dest
        case 12: //SRC1 & DEST
            op1 = src1;
            valid1 = false;
            isReg1 = true;
            break;

        case 3: //src1 & src2 & offset
            op1 = src1;
            valid1 = false;
            isReg1 = true;
            op2 = src2;
            valid2 = false;
            isReg2 = true;
            op3 = offset; //always valid and not a reg
            break;

        case 4: //src1 & offset
            op1 = src1;
            valid1 = false;
            isReg1 = true;
            op2 = offset;
            valid2 = true;
            break;

        case 5: // IMM & SRC1=FCC
        case 6: //DEST & IMM & SRC1 (in that order)
            op1 = imm;
            valid1 = true;
            op2 = src1;
            valid2 = false;
            isReg2 = true;
            break;

        case 8: //DEST & SRC1 & IMM
            op1 = src1;
            valid1 = false;
            isReg1 = true;
            op2 = imm;
            valid2 = true;
            break;

        case 7:
        case 9: //SRC1 & SRC2, DEST = HI_LO
        case 13: //SRC1 & SRC2, DEST=FCC
            op1 = src1;
            valid1 = false;
            isReg1 = true;
            op2 = src2;
            valid2 = false;
            isReg2 = true;
            break;

        default:
            break;
    }

}

void Instruction::Print() {
    DEBUG_COUT(" PC: " << PC); DEBUG_COUT_2(" dest: " << dest); //"\t destStr:\t" << destReg << endl);
    DEBUG_COUT(" imm: " << imm ); DEBUG_COUT_2(" offset: " << offset); DEBUG_COUT_2(" op: " << opCode); DEBUG_COUT_2(" src1: " << src1); //"\t src1Str:\t" << src1Reg << endl);
    DEBUG_COUT(" src2:" << src2); //"\t src2Str:\t" << src2Reg << endl);
    DEBUG_COUT(" btaddress:" << predictedTargetPC); //"\t src2Str:\t" << src2Reg << endl);
    DEBUG_COUT(endl);
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

std::string Instruction::GetOpcodeString() {
    return opCodeStr;
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
    if (spacePos == string::npos) //if we mess this up, everythings screwed up.
        exit(-1);

    pcStr = line.substr(0, spacePos);

    instructionLine = line.substr(spacePos + 1);

    //partial decode to know if we need to hit a predictor
    if (instructionLine.find("beq") != string::npos || instructionLine.find("bne") != string::npos || instructionLine.find("bl") != string::npos || instructionLine.find("bg") != string::npos
            || instructionLine.find("bc") != string::npos || instructionLine.find("beq") != string::npos)
        isBranch = true;

    PC = atoi(pcStr.c_str());

}

void Instruction::DecodeInstructionString() {
    DEBUG_COUT("DECODING " << instructionLine << endl);
    if (instructionLine == "" || instructionLine == "nop") {
        opCodeStr = "NOP";
        opCode = NOP;
        return;
    }

    istringstream splitLine(instructionLine);
    vector<string> tokens;
    copy(istream_iterator<string>(splitLine), istream_iterator<string>(), back_inserter<vector<string> >(tokens));

//		PC = atoi(tokens[0].c_str()); // PC is decoded in fetch stage
    opCodeStr = tokens[0];
    opCode = opcodeTypeMap[opCodeStr]; // get the opcode type for FU routing later
    if (tokens.size() > 1)
        DecodeRegisters(tokens[1]);

    if(opCode == BRANCH)
        predictedTargetPC = PC + offset; //set this here so execute can use it later
}

int Instruction::GetRegisterIndexFromName(std::string regName) {
    int retVal = 0;
    unsigned int charPos = 0;
    int indexOffset = 0;
    string numberStr;

    DEBUG_COUT("For PC " << this->PC << ". Translating " << regName << " to a vector index" << endl);

    if (regName.length() == 0)
        return -1;

    if (regName == "HI_LO") {
//	    DEBUG_COUT("Found HI_LO. Returning 63\n");
        return hiloRegIndex;
    }

    if (regName == "FCC") {
//	    DEBUG_COUT("Found FCC. Returning 64\n");
        return fccRegIndex;
    }

    charPos = regName.find('r');
    if (charPos == string::npos) {
        charPos = regName.find('f');
        if (charPos == string::npos)
            return -1; //we didnt find a r or f in the string... wat

        indexOffset = fpRegIndex;
    }

    numberStr = regName.substr(charPos + 1);
    retVal = atoi(numberStr.c_str());

    DEBUG_COUT("Return val is " << retVal << " plus " << indexOffset << " giving " << retVal+indexOffset<< endl);

    retVal += indexOffset;

    return retVal;
}

void Instruction::DecodeRegisters(std::string regStr) {
    vector<string> regs;
    std::stringstream ss(regStr);
    char tempChar;
    std::stringstream *token = new std::stringstream();
    while (ss >> tempChar) {
        *token << tempChar;
        if (ss.peek() == ',' || ss.peek() == '(' || ss.peek() == ')') {
            regs.push_back(token->str());
            ss.ignore();
            delete token;
            token = new std::stringstream();
        }
    }
    if (token->str().size() > 0) {
        regs.push_back(token->str());
    }

    /* Opcode input instruction types
     * 0 = IMM ONLY
     * 1 = SRC1 ONLY
     * 2 = SRC1 & SRC2
     * 3 = SRC1 & SRC2 & OFFSET
     * 4 = SRC1 & OFFSET
     * 5 = IMM & SRC1=FCC
     * 6 = DEST & IMM & SRC1 (in that order)
     * 7 = DEST & SRC1 & SRC2
     * 8 = DEST & SRC1 & IMM
     * 9 = SRC1 & SRC2, DEST = HI_LO
     * 10 = DEST && SRC1 = HI_LO
     * 11 = DEST & IMM
     * 12 = SRC1 & DEST
     * 13 = SRC1 & SRC2, DEST=FCC
     *
     */

//	cout << "Switching " << instructionTypeMap[opCodeStr] << endl);
    switch (instructionTypeMap[opCodeStr]) {
        case 0: //imm only
            imm = atoi(regs[0].c_str());
            break;
        case 1: //src1 only
            src1Reg = regs[0];
            break;
        case 2: //dest & src 1
            destReg = regs[0];
            src1Reg = regs[1];
            break;
        case 3: //src1 & src2 & offset
            src1Reg = regs[0];
            src2Reg = regs[1];
            offset = atoi(regs[2].c_str());
            break;
        case 4: //src1 & offset
            src1Reg = regs[0];
            offset = atoi(regs[1].c_str());
            break;
        case 5: // OFFSET & SRC1=FCC
            src1Reg = "FCC";
            offset = atoi(regs[0].c_str());
            break;
        case 6: //DEST & IMM & SRC1 (in that order)
            destReg = regs[0];
            imm = atoi(regs[1].c_str());
            src1Reg = regs[2];
            break;
        case 7: //DEST & SRC1 & SRC2
            destReg = regs[0];
            src1Reg = regs[1];
            src2Reg = regs[2];
            break;
        case 8: //DEST & SRC1 & IMM
            destReg = regs[0];
            src1Reg = regs[1];
            imm = atoi(regs[2].c_str());
            break;
        case 9: //SRC1 & SRC2, DEST = HI_LO
            src1Reg = regs[0];
            src2Reg = regs[1];
            destReg = "HI_LO";
            break;
        case 10: //s1= HI_LO, dest
            src1Reg = "HI_LO";
            destReg = regs[0];
            break;
        case 11: //DEST & IMM
            destReg = regs[0];
            imm = atoi(regs[1].c_str());
            break;
        case 12: //SRC1 & DEST
            src1Reg = regs[0];
            destReg = regs[1];
            break;
        case 13: //SRC1 & SRC2, DEST=FCC
            src1Reg = regs[0];
            src2Reg = regs[1];
            destReg = "FCC";
            break;
        default:
            break;
    }
//
    src1 = GetRegisterIndexFromName(src1Reg);
    src2 = GetRegisterIndexFromName(src2Reg);
    dest = GetRegisterIndexFromName(destReg);
}

void Instruction::FillMaps() {
//	instructionMap.insert()

    /* Opcode input instruction types
     * 0 = IMM ONLY
     * 1 = SRC1 ONLY
     * 2 = SRC1 & SRC2
     * 3 = SRC1 & SRC2 & OFFSET
     * 4 = SRC1 & OFFSET
     * 5 = OFFSET & SRC1=FCC
     * 6 = DEST & IMM & SRC1 (in that order)
     * 7 = DEST & SRC1 & SRC2
     * 8 = DEST & SRC1 & IMM
     * 9 = SRC1 & SRC2, DEST = HI_LO
     * 10 = DEST && SRC1 = HI_LO
     * 11 = DEST & IMM
     * 12 = SRC1 & DEST
     * 13 = SRC1 & SRC2, DEST=FCC
     *
     */

    //0 = IMM ONLY
    instructionTypeMap["j"] = 0;
    instructionTypeMap["jal"] = 0;

    //1 = SRC1 ONLY
    instructionTypeMap["jr"] = 1;

    //2 = DEST & SRC1
    instructionTypeMap["jalr"] = 2;
    instructionTypeMap["mfc1"] = 2;
    instructionTypeMap["dmfc1"] = 2;
    instructionTypeMap["mov.d"] = 2;
    instructionTypeMap["neg.d"] = 2;
    instructionTypeMap["cvt.s.d"] = 2;
    instructionTypeMap["cvt.s.w"] = 2;
    instructionTypeMap["cvt.d.s"] = 2;
    instructionTypeMap["cvt.d.w"] = 2;
    instructionTypeMap["cvt.w.d"] = 2;
    instructionTypeMap["sqrt.d"] = 2;

    //3 = SRC1 & SRC2 & OFFSET
    instructionTypeMap["beq"] = 3;
    instructionTypeMap["bne"] = 3;

    //4 = SRC1 & OFFSET
    instructionTypeMap["blez"] = 4;
    instructionTypeMap["bgtz"] = 4;
    instructionTypeMap["bltz"] = 4;
    instructionTypeMap["bgez"] = 4;

    //5 = OFFSET & SRC1=FCC
    instructionTypeMap["bc1f"] = 5;
    instructionTypeMap["bc1t"] = 5;

    //6 = DEST & IMM & SRC1 (in that order)
    instructionTypeMap["lb"] = 6;
    instructionTypeMap["lbu"] = 6;
    instructionTypeMap["lh"] = 6;
    instructionTypeMap["lhu"] = 6;
    instructionTypeMap["lw"] = 6;
    instructionTypeMap["l.s"] = 6;
    instructionTypeMap["l.d"] = 6;
    instructionTypeMap["sb"] = 6;
    instructionTypeMap["sh"] = 6;
    instructionTypeMap["sw"] = 6;
    instructionTypeMap["s.s"] = 6;
    instructionTypeMap["s.d"] = 6;

    //7 = DEST & SRC1 & SRC2
    instructionTypeMap["add"] = 7;
    instructionTypeMap["addu"] = 7;
    instructionTypeMap["sub"] = 7;
    instructionTypeMap["subu"] = 7;
    instructionTypeMap["and"] = 7;
    instructionTypeMap["or"] = 7;
    instructionTypeMap["xor"] = 7;
    instructionTypeMap["nor"] = 7;
    instructionTypeMap["sllv"] = 7;
    instructionTypeMap["srlv"] = 7;
    instructionTypeMap["srav"] = 7;
    instructionTypeMap["slt"] = 7;
    instructionTypeMap["sltu"] = 7;
    instructionTypeMap["add.s"] = 7;
    instructionTypeMap["add.d"] = 7;
    instructionTypeMap["sub.s"] = 7;
    instructionTypeMap["sub.d"] = 7;
    instructionTypeMap["mul.s"] = 7;
    instructionTypeMap["mul.d"] = 7;
    instructionTypeMap["div.d"] = 7;

    //8 = DEST & SRC1 & IMM
    instructionTypeMap["addi"] = 8;
    instructionTypeMap["addiu"] = 8;
    instructionTypeMap["andi"] = 8;
    instructionTypeMap["ori"] = 8;
    instructionTypeMap["xori"] = 8;
    instructionTypeMap["sll"] = 8;
    instructionTypeMap["srl"] = 8;
    instructionTypeMap["sra"] = 8;
    instructionTypeMap["slti"] = 8;
    instructionTypeMap["sltiu"] = 8;

    //9 = SRC1 & SRC2, DEST = HI_LO
    instructionTypeMap["mult"] = 9;
    instructionTypeMap["div"] = 9;
    instructionTypeMap["divu"] = 9;

    //10 = DEST && SRC1 = HI_LO
    instructionTypeMap["mfhi"] = 10;
    instructionTypeMap["mflo"] = 10;

    //11 = DEST & IMM
    instructionTypeMap["lui"] = 11;

    //12 = SRC1 & DEST
    instructionTypeMap["mtc1"] = 12;
    instructionTypeMap["dmtc1"] = 12;

    //13 = SRC1 & SRC2, DEST=FCC
    instructionTypeMap["c.eq.d"] = 13;
    instructionTypeMap["c.lt.d"] = 13;
    instructionTypeMap["c.le.d"] = 13;

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
    opcodeTypeMap["mfhi"] = ADD_SUB_I;
    opcodeTypeMap["mflo"] = ADD_SUB_I;
    opcodeTypeMap["lui"] = ADD_SUB_I;
    opcodeTypeMap["mfc1"] = ADD_SUB_I;
    opcodeTypeMap["dmfc1"] = ADD_SUB_I;
    opcodeTypeMap["mtc1"] = ADD_SUB_I;
    opcodeTypeMap["dmtc1"] = ADD_SUB_I;

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

    opcodeTypeMap["nop"] = NOP;
    opcodeTypeMap["syscall"] = NOP;

}
