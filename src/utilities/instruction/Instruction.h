/*
 * Instruction.h
 *
 *  Created on: Oct 29, 2013
 *      Author: brian
 */

#ifndef INSTRUCTION_H_
#define INSTRUCTION_H_

#include <string>
#include <map>
#include <vector>
#include "StructureDefs.h"

class Instruction {
public:
    Instruction();
    Instruction(std::string line);

    bool IsBranchOrJump();
    void DecodeInstructionString();
    virtual ~Instruction();

    void Print();
    std::string ToString();

    int PC;
    int src1;
    int src2;
    int imm;
    int dest;
    int offset;
    short branchPredictorAddress;
    OpcodeType opCode;

private:

    std::string instructionLine;
    int opCodeType;
    std::string opCodeStr;
    bool isBranchOrJump;

    static std::map<std::string, int> instructionTypeMap;
    static std::map<std::string, OpcodeType> opcodeTypeMap;
    int InstructionTypeLookup(std::string code);
    int OpcodeTypeLookup(std::string code);
    void SplitPCandString(std::string line);
    void FillMaps();

    void DecodeRegisters(std::vector<std::string> tokens);

//    bool CheckIfBranchOrJump();
};

#endif /* INSTRUCTION_H_ */
