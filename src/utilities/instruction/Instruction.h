/*
 * Instruction.h
 *
 *  Created on: Oct 29, 2013
 *      Author: brian
 */

#ifndef INSTRUCTION_H_
#define INSTRUCTION_H_

#include <string>

class Instruction {
public:
    Instruction();
    Instruction(std::string line);

    virtual ~Instruction();

    void Print();
    std::string ToString();

private:
    int PC;
    int opCode;
    int src1;
    int src2;
    int imm;
    int dest;
};

#endif /* INSTRUCTION_H_ */
