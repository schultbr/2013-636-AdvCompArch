/*
 * DispatchStage.cpp
 *
 *  Created on: Nov 12, 2013
 *      Author: brian
 */

#include "DispatchStage.h"

using namespace std;

//returns some reference to the RS... not
//sure what it should be yet though todo:figure out
int dispatchToRS(Instruction inst, ReservationStationType rsType) {
    int returnTag = -1;

    return returnTag;
}


//returns the ROB Tag
int dispatchToROB(Instruction inst, int rsTag) {
    int returnTag = -1;

    return returnTag;
}


//returns the RRF Tag
int dispatchToRRF(Instruction inst) {
    int returnTag = -1;

    //loop through from the start to the end of the ROB
    //find the first emtpy spot and sit 'er down.
    //then update the original ARF entry with the
    //chosen RRF tag
    for(size_t i = 0; i < reOrderBuffer.size(); i++){
        if(!reOrderBuffer[i].busy) {
            //bingo. found our entry
            returnTag = i;

            //set it up.
            reOrderBuffer[i].busy = true;
            reOrderBuffer[i].finished = false;
            reOrderBuffer[i].valid = false;
            reOrderBuffer[i].opCode = inst.opCode;
            reOrderBuffer[i].OP = inst.GetOpcodeString();

            break; //dont bother continuing. let's move on.
        }
    }

    //if we're here and returnTag is still -1...
    //we need to stall. RRF is full.
    return returnTag;
}




void simulateDispatchStage(std::queue<Instruction> &instrToDispatch) {

    //Dispatch takes each instruction and sends it to:
    //	The ROB
    //	The correct RS
    //	The RRF (where logical... not mem instr or branch instr)

    int rrfTag = -1;
    int rsTag = -1;
    int robTag = -1;
    ReservationStationType targetRSGroup = NO_RS;
    bool isStalled = false;


    while(instrToDispatch.size() > 0) {
        //RRF Dispatch
        rrfTag = dispatchToRRF(instrToDispatch.front());
        if(rrfTag == -1) {
            //we need to stall! RRF is full
            isStalled = true;
            break;
        }

        //RS dispatch
        switch(instrToDispatch.front().opCode){
            case ADD_SUB_I:
            case MULT_DIV_I:
                targetRSGroup = INTEGER_RS;
                break;
            case FLOATING_POINT:
                targetRSGroup = FLOATING_POINT_RS;
                break;
            case BRANCH:
                targetRSGroup = BRANCH_RS;
                break;
            case LOAD:
            case STORE:
                targetRSGroup = MEMORY_RS;
                break;
            case JUMP:
            default:
                targetRSGroup = NO_RS; //right? JUMP doesn't use a execution unit? maybe?
                break;
        }
        //does RS dispatch need RRF tag? I can't recall.
        rsTag = dispatchToRS(instrToDispatch.front(), targetRSGroup);
        if(rsTag == -1) {
            //we need to stall! RS is full
            isStalled = true;
            break;
        }

        //ROB Dispatch
        robTag = dispatchToROB(instrToDispatch.front(), rsTag);
        if(robTag == -1) {
            //crap. again, we stall. ROB is full.
            isStalled = true;
            break;
        }

        //we get here? that means the instruction was dispatched. bon voyage!
        instrToDispatch.pop();
    }

    if(isStalled) {
        //do something useful
    }

}

