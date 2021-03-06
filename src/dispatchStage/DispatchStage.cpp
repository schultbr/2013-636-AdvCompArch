/*
 * DispatchStage.cpp
 *
 *  Created on: Nov 12, 2013
 *      Author: brian
 */

#include "DispatchStage.h"

#include <unistd.h>
#include <stdio.h>
#include <iostream>
#include <stdlib.h>

using namespace std;

//check here to make sure the RRF, RS and ROB all have space, if needed
//false means no dice. we stall.
//true means sally forth
bool checkDestAvailable(Instruction inst, std::vector<RS_Element> *targetRS, bool &usesRRF, bool &usesRS) {
    bool isROBFull = true;

    for (size_t i = 0; i < rob.size() && isROBFull; i++) {
        if (rob[i].busy == false) {
            isROBFull = false;
            break;
        }
    }
    if (isROBFull)
        return false;

    //the following is over the top. try the new one/
    if (inst.opCode == BRANCH || inst.opCode == JUMP || inst.opCode == LOAD || inst.opCode == STORE || inst.opCode == NOP)
        usesRRF = false;
    else
        usesRRF = true;

    usesRS = (inst.opCode == NOP ? false : true);

    bool isRRFFull = true;

    if (usesRRF) {
        for (size_t i = 0; i < rrf.size() && isRRFFull; i++) {
            if (rrf[i].busy == false) {
                isRRFFull = false;
                break;
            }
        }
    }

    if (isRRFFull && usesRRF)
        return false;

    bool isRSFull = true;

    if (usesRS) {
        for (size_t i = 0; i < targetRS->size() && isRSFull; i++) {
            if (targetRS->at(i).busy == false) {
                isRSFull = false;
                break;
            }
        }
    }

    if (isRSFull && usesRS)
        return false;

    return true;
}

//use this to translate incoming instructions into RS entries...
//checking the ARF and RRF.
//false means not ready... false AND op == -1 means no valid src
//index (i.e. check imm/offset)
//true means ready bit is tru too, and op contains real data.
bool translateSrcToOp(int srcRegIndex, int &op) {

//    DEBUG_COUT("Dispatch:\t" << "Dispatching to ROB." << endl;

    if (srcRegIndex == -1) {
        op = -1;
        return false;
    }

    if (!arf[srcRegIndex].busy) {
        op = arf[srcRegIndex].data;
        return true;
    }
    else {
        int rrfTag = arf[srcRegIndex].rename;
        if (rrf[rrfTag].valid) {
            op = rrf[rrfTag].data;
            return true;
        }
        else {
            op = rrfTag;
            return false;
        }
    }
}

//translate the reg index supplied by the instruction into a real op val (rrf tag or data)
void checkForValid(int &op, bool &valid) {
    string check = (op == -1 ? "true" : "false");

    if (op == -1) {
        valid = true;
        return;
    }

    if (arf[op].busy == false) {
        op = arf[op].data;
        valid = true;
        return;
    }
    else {
        int rrfTag = arf[op].rename;
        if (rrf[rrfTag].valid == true) {
            op = rrf[rrfTag].data;
            valid = true;
            return;
        }
        else {
            op = rrfTag;
            valid = false;
            return;
        }
    }

}

//returns tag to the RS
int dispatchToRS(Instruction inst, std::vector<RS_Element> *targetRS, int robTag) {
    int returnTag = -1;
    bool isReg1 = false;
    bool isReg2 = false;

    DEBUG_COUT("Dispatch:\t" << "Dispatching to RS." << endl);

    if (targetRS == NULL)
        return returnTag;

    for (size_t i = 0; i < targetRS->size(); i++) {
        if (!(targetRS->at(i).busy)) {
            isReg1 = false;
            isReg2 = false;
            returnTag = i;
            targetRS->at(i).busy = true; //claim this guy
            targetRS->at(i).PC = inst.PC;

            //set the op's and valid's:
            inst.TraslateToFUEntry(targetRS->at(i).op1, targetRS->at(i).valid1, isReg1, targetRS->at(i).op2, targetRS->at(i).valid2, isReg2, targetRS->at(i).op3);

            if (isReg1)
                checkForValid(targetRS->at(i).op1, targetRS->at(i).valid1);

            if (isReg2)
                checkForValid(targetRS->at(i).op2, targetRS->at(i).valid2);

            targetRS->at(i).ready = (targetRS->at(i).valid1 && targetRS->at(i).valid2);
            targetRS->at(i).reorder = robTag;
            targetRS->at(i).PTaddr = inst.branchPredictorTableAddress;
            targetRS->at(i).BRprediction = inst.wasBranchPredictedAsTaken; //br prediction to be false if nt or btb miss, true if t
            targetRS->at(i).BRoutcome = inst.wasBranchActuallyTaken;
            targetRS->at(i).BTaddr = inst.predictedTargetPC;
            targetRS->at(i).code = inst.opCode;

            break; //we got our parking spot. quit the loop.
        }
    }

    return returnTag;
}

//returns the ROB Tag
//assumes we've already made sure we have the space to do ths...
int dispatchToROB(Instruction inst, int renameTag, bool initAsFinished = false) {
    int returnTag = -1;

    rob[robTail].busy = true;
    rob[robTail].finished = initAsFinished;
    rob[robTail].valid = !(::anyUnresolvedBranches); //set to 0 if we have a branch waiting to resolve
    rob[robTail].rename = renameTag;
    rob[robTail].code = inst.opCode;
    rob[robTail].PC = inst.PC;
    rob[robTail].issued = false;
    returnTag = robTail;

    rob_inUse++;
    robEntries++;
    if (robEntries > robEntriesMax)
        robEntriesMax = robEntries;

    robTail++;
    if (robTail == ::reorderBufferEntries) //if we're at the end of the vector now, wrap it to 0
        robTail = 0;

    if (inst.opCode == BRANCH) {
        ::unresolvedBranchRobIndex = returnTag;
        ::anyUnresolvedBranches = true;
    }

    return returnTag;
}

//returns the RRF Tag
int dispatchToRRF(Instruction inst) {
    int returnTag = -1;

    //loop through from the start to the end of the rrf
    //find the first emtpy spot and sit 'er down.
    //then update the original ARF entry with the
    //chosen RRF tag
    for (size_t i = 0; i < rrf.size(); i++) {
        if (!rrf[i].busy) {
            //bingo. found our entry
            returnTag = i;

            //set it up.
            rrf[i].busy = true;
            rrf[i].valid = false;
            rrf[i].data = 0;
            rrf[i].dest = inst.dest;

            break;    //dont bother continuing. let's move on.
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
    int robTag = -1;
    std::vector<RS_Element> *targetRS;
    bool isStalled = false;
    bool checkRet = false;
    bool usesRRF = false;
    bool usesRS = false;

    if (!isDispatchFinished && (isDecodeFinished && instrToDispatch.size() == 0)) {
        isDispatchFinished = true;
        cout << "Dispatch is now finished" << endl;
        return;
    }
    else if (isDispatchFinished)
        return;

    while (instrToDispatch.size() > 0 && !isStalled) {
        switch (instrToDispatch.front().opCode) {
            case ADD_SUB_I:
            case MULT_DIV_I:
            case LOGICAL:
                targetRS = &rs_int;
                break;
            case FLOATING_POINT:
                targetRS = &rs_fp;
                break;
            case JUMP:
            case BRANCH:
                targetRS = &rs_br;
                break;
            case LOAD:
            case STORE:
                targetRS = &rs_mem;
                break;
            default:
                targetRS = NULL;
                break;
        }

        checkRet = checkDestAvailable(instrToDispatch.front(), targetRS, usesRRF, usesRS);
        isStalled = !checkRet; //the check returns false if we can't place things... so, we stall then

        string boolResTrue = "true";
        string boolResFalse = "false";

        if (isStalled)
            break;

        //for syscall - no RRF or RS entry... straight to ROB and make completed.
        //RRF Dispatch
        if (usesRRF)
            rrfTag = dispatchToRRF(instrToDispatch.front());
        else
            rrfTag = -1;

        //ROB Dispatch
        robTag = dispatchToROB(instrToDispatch.front(), rrfTag, (instrToDispatch.front().opCode == NOP ? true : false));

        //RS dispatch
        if (usesRS) {
            dispatchToRS(instrToDispatch.front(), targetRS, robTag);
            //update stats for RS entries in use
            switch (instrToDispatch.front().opCode) {
                case ADD_SUB_I:
                case MULT_DIV_I:
                case LOGICAL:
                    rs_int_inUse++;
                    break;
                case FLOATING_POINT:
                    rs_fp_inUse++;
                    break;
                case JUMP:
                case BRANCH:
                    rs_br_inUse++;
                    break;
                case LOAD:
                case STORE:
                    rs_mem_inUse++;
                    break;
                case NOP:
                default:
                    break;
            }
        }

        //update ARF to show that the RRF now has the latest value
        if (usesRRF) {
//            instrToDispatch.front().dest
            arf[instrToDispatch.front().dest].busy = true;
            arf[instrToDispatch.front().dest].rename = rrfTag;
        }

        //we get here? that means the instruction was dispatched. bon voyage!
        instrToDispatch.pop();
    }

}

