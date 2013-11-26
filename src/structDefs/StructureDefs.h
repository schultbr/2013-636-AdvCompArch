/*
 * StructureDefs.h
 *
 *  Created on: Oct 29, 2013
 *      Author: jason cain
 */

#ifndef STRUCTUREDEFS_H_
#define STRUCTUREDEFS_H_

enum OpcodeType {
    ADD_SUB_I, MULT_DIV_I, BRANCH, JUMP, LOAD, STORE, FLOATING_POINT, LOGICAL, NOP
};

//Reorder Buffer slot
struct ROB_Element {
    bool busy;		//busy bit, this entry in use
    bool finished;		//out of FU, has finished execution
    bool valid;		//instr after a br are speculative, valid=0 by default
    bool issued;		//out of RS, has been issued
    int rename;		//Rename Register File tag
    OpcodeType code;		//opcode
    int PC;			//PC

    ROB_Element() {			//constructor
        busy = false;
        finished = false;
        valid = false;
        issued = false;
        rename = -1;
        code = NOP;
        PC = -1;
    }
};

//Architecture Register File slot
struct ARF_Element {
    bool busy;
    int data;
    int rename;

    ARF_Element() {
        busy = false;
        data = 0;
        rename = -1;
    }
};

//Rename Register File slot
struct RRF_Element {
    bool busy;
    bool valid;
    int data;
    int dest;

    RRF_Element() {
        busy = false;
        valid = false;
        data = 0;
        dest = -1;
    }
};

//Reservation Station slot
struct RS_Element {
    bool busy;
    bool valid1;
    bool valid2;
    bool ready;
    int op1;
    int op2;
    int op3;    //only used by BNE and BEQ, where we compare 2 regs and have a branch offset, too
    int reorder;
    short PTaddr;			//Prediction Table address
    bool BRoutcome;			//Branch Outcome
    bool BRprediction;		//Branch Prediction
    int PC;
    int BTaddr;			//Branch Target Address

    OpcodeType code;		//opcode

    RS_Element() {
        busy = false;
        valid1 = false;
        valid2 = false;
        ready = false;
        op1 = -1;
        op2 = -1;
        op3 = -1;
        reorder = -1;
        code = NOP;
        PTaddr = -1;
        BRoutcome = false;
        BRprediction = false;
        PC = -1;
        BTaddr = -1;
    }
};

//Functional Unit slot
struct FU_Element {

    int count;
    int op1;
    int op2;
    int reorder;
    int result;
    OpcodeType code;		//opcode
    short PTaddr;			//Prediction Table address
    bool BRoutcome;			//Branch Outcome
    bool BRprediction;		//Branch Prediction
    int PC;
    int BTaddr;			//Branch Target Address
    bool isFirstClock;

    FU_Element() {
        count = 0;
        op1 = -1;
        op2 = -1;
        reorder = -1;
        code = NOP;
        result = 7;
        PTaddr = -1;
        BRoutcome = false;
        BRprediction = false;
        PC = -1;
        BTaddr = -1;
        isFirstClock = false; // used by memory fu
    }
};

struct BTB_Element {
    int instrPC;
    int targetPC;

    BTB_Element() {
        instrPC = -1;
        targetPC = -1;
    }

};

#endif /* STRUCTUREDEFS_H_ */
