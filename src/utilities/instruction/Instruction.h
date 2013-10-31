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

class Instruction {
public:
    Instruction();
    Instruction(std::string line);

    virtual ~Instruction();

    void Print();
    std::string ToString();

private:
    int PC;
    int opCodeType;
    std::string opCode;
    int src1;
    int src2;
    int imm;
    int dest;

    std::map<std::string, int> instructionMap;

    int OpcodeLookup(std::string code);
    void ParseRegisters(std::vector<std::string> tokens);
    void FillMap();
};

#endif /* INSTRUCTION_H_ */
