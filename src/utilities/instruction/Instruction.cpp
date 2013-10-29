/*
 * Instruction.cpp
 *
 *  Created on: Oct 29, 2013
 *      Author: brian
 */

#include "Instruction.h"

#include <string>
#include <sstream>
#include <stdio.h>

using namespace std;

Instruction::Instruction() {
    PC = -1;
    dest = -1;
    imm = -1;
    opCode = -1;
    src1 = -1;
    src2 = -1;

}

Instruction::Instruction(string line) {

    //todo: build real parser
    PC = -1;
    dest = -1;
    imm = -1;
    opCode = -1;
    src1 = -1;
    src2 = -1;
}

Instruction::~Instruction() {
    // TODO Auto-generated destructor stub
}

void Instruction::Print() {
    printf("PC:\t%d\n", PC);
    printf("dest:\t%d\n", dest);
    printf("imm:\t%d\n", imm);
    printf("op:\t%d\n", opCode);
    printf("src1:\t%d\n", src1);
    printf("src2:\t%d\n", src2);
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


