/*
 * DispatchStage.cpp
 *
 *  Created on: Nov 12, 2013
 *      Author: brian
 */

#include "DispatchStage.h"

using namespace std;

//check here to make sure the RRF, RS and ROB all have space, if needed
//false means no dice. we stall.
//true means sally forth
bool checkDestAvailable(Instruction inst, std::vector<RS_Element> *targetRS,
                        bool &usesRRF, bool &usesRS) {
    bool isROBFull = true;

    //todo: this can be done by checking head and tail, but i need to
    //know if tail points to the next available entry or the last used entry...
    // that changes the math needed (ideally tail points to next available,
    //so we can check tail == head and be done)
    for(size_t i = 0; i < rob.size() && isROBFull; i++) {
        if(rob[i].busy == false)
            isROBFull = false;
    }
    if(isROBFull)
        return false;

    if(inst.opCode == BRANCH ||
            inst.opCode == JUMP ||
            inst.opCode == LOAD ||
            inst.opCode == STORE ||
            inst.opCode == NOP)
        usesRRF = false;

    if(inst.opCode == NOP)
        usesRS = false;

    bool isRRFFull = true;
    if(usesRRF) {
        for(size_t i = 0; i < rrf.size() && isRRFFull; i++) {
            if(rrf[i].busy == false)
                isRRFFull = false;
        }
    }

    if(isRRFFull && usesRRF)
        return false;


    bool isRSFull = true;
    if(usesRS) {
        for(size_t i = 0; i < targetRS->size() && isRSFull; i++) {
            if(targetRS->at(i).busy == false)
                isRSFull = false;
        }
    }

    if(isRSFull && usesRS)
        return false;

    return true;
}

//use this to translate incoming instructions into RS entries...
//checking the ARF and RRF.
//false means not ready... false AND op == -1 means no valid src
//index (i.e. check imm/offset)
//true means ready bit is tru too, and op contains real data.
bool translateSrcToOp(int srcRegIndex, int &op) {
    if(srcRegIndex == -1) {
        op = -1;
        return false;
    }

    if(!arf[srcRegIndex].busy){
        op = arf[srcRegIndex].data;
        return true;
    }
    else {
        int rrfTag = arf[srcRegIndex].rename;
        if(rrf[rrfTag].valid) {
            op = rrf[rrfTag].data;
            return true;
        }
        else{
            op = rrfTag;
            return false;
        }
    }
}


//returns some reference to the RS... not
//sure what it should be yet though todo:figure out
int dispatchToRS(Instruction inst, std::vector<RS_Element> *targetRS, int robTag) {
    int returnTag = -1;
//    std::vector<RS_Element> *targetRS;

    if(targetRS == NULL)
        return returnTag;

    bool srcToOpRet = false;

    for(size_t i = 0; i < targetRS->size(); i++) {
        if(!(targetRS->at(i).busy)) {
            returnTag = i;
            targetRS->at(i).busy = true; //claim this guy
            targetRS->at(i).PC = inst.PC;

            srcToOpRet = translateSrcToOp(inst.src1, targetRS->at(i).op1);
            if(!srcToOpRet && targetRS->at(i).op1 == -1) {
                //src 1 is not valid... it's -1. check imm or offset.. mainly just imm...
                if(inst.imm != -1) {
                    targetRS->at(i).op1 = inst.imm;
                    targetRS->at(i).valid1 = true;
                }
                else if(inst.offset != 0) {
                    targetRS->at(i).op1 = inst.offset;
                    targetRS->at(i).valid1 = true;
                }
            }
            else {
                //op1 is a valid result now, but it points to the rrf,
                //and the data's not ready -OR- op1 is valid and ready to rock.
                //srcToOpRet is set accordingly..
                targetRS->at(i).valid1 = srcToOpRet;
            }


            srcToOpRet = translateSrcToOp(inst.src2, targetRS->at(i).op2);
            if(!srcToOpRet && targetRS->at(i).op2 == -1) {
                //src 1 is not valid... it's -1. check imm or offset.. mainly just imm...
                if(inst.imm != -1) {
                    targetRS->at(i).op2 = inst.imm;
                    targetRS->at(i).valid2 = true;
                }
                else if(inst.offset != 0) {
                    targetRS->at(i).op2 = inst.offset;
                    targetRS->at(i).valid2 = true;
                }
            }
            else {
                //op1 is a valid result now, but it points to the rrf,
                //and the data's not ready -OR- op1 is valid and ready to rock.
                //srcToOpRet is set accordingly..
                targetRS->at(i).valid2 = srcToOpRet;
            }

            targetRS->at(i).ready = (targetRS->at(i).valid1 && targetRS->at(i).valid2);
            targetRS->at(i).reorder = robTag;
            targetRS->at(i).PTaddr = inst.branchPredictorAddress;
            targetRS->at(i).BRoutcome = inst.GetWasBranchTaken();
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
    rob[robTail].valid = true; //always true, since we don't have prediction blocks
    rob[robTail].rename = renameTag;
    rob[robTail].code = inst.opCode;
    returnTag = robTail;

    robTail++;
    if(robTail == ::reorderBufferEntries) //if we're at the end of the vector now, wrap it to 0
        robTail = 0;

    return returnTag;
}


//returns the RRF Tag
int dispatchToRRF(Instruction inst) {
    int returnTag = -1;

    //loop through from the start to the end of the ROB
    //find the first emtpy spot and sit 'er down.
    //then update the original ARF entry with the
    //chosen RRF tag
    for(size_t i = 0; i < rob.size(); i++){
        if(!rob[i].busy) {
            //bingo. found our entry
            returnTag = i;

            //set it up.
            rob[i].busy = true;
            rob[i].finished = false;
            rob[i].valid = false;
            rob[i].code = inst.opCode;
//            rob[i].OP = inst.GetOpcodeString();

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
//    ReservationStationType targetRSGroup = NO_RS;
    std::vector<RS_Element> *targetRS;
    bool isStalled = false;
    bool checkRet = false; //this is so dumb but im in a hurry
    bool usesRRF = false;
    bool usesRS = false;


    while(instrToDispatch.size() > 0 && !isStalled) {
        switch(instrToDispatch.front().opCode){
            case ADD_SUB_I:
            case MULT_DIV_I:
                targetRS =  &rs_int;
                break;
            case FLOATING_POINT:
                targetRS =  &rs_fp;
                break;
            case BRANCH:
                targetRS =  &rs_br;
                break;
            case LOAD:
            case STORE:
                targetRS =  &rs_mem;
                break;
            case JUMP:
            default:
                targetRS = NULL; //right? JUMP doesn't use a execution unit? maybe?
                break;
        }

        checkRet = checkDestAvailable(instrToDispatch.front(), targetRS, usesRRF, usesRS);
        isStalled = !checkRet; //the check returns false if we can't place things... so, we stall then

        if(isStalled)
            break;

        //for syscall - no RRF or RS entry... straight to ROB and make completed.
        //RRF Dispatch
        if(usesRRF)
            rrfTag = dispatchToRRF(instrToDispatch.front());
//        if(rrfTag == -1) { //shouldnt be needed
//            //we need to stall! RRF is full
//            isStalled = true;
//            break;
//        }

        //ROB Dispatch
        robTag = dispatchToROB(instrToDispatch.front(), rrfTag, (instrToDispatch.front().opCode == NOP ? true : false));
//        if(robTag == -1) {
//            //crap. again, we stall. ROB is full.
//            isStalled = true;
//            break;
//        }

        //RS dispatch
        //does RS dispatch need RRF tag? I can't recall.
        if(usesRS)
            rsTag = dispatchToRS(instrToDispatch.front(), targetRS, robTag);
//        if(rsTag == -1) {
//            //we need to stall! RS is full
//            isStalled = true;
//            break;
//        }

        //we get here? that means the instruction was dispatched. bon voyage!
        instrToDispatch.pop();
    }

    if(isStalled) {
        //do something useful.. or not? I think we just hang and let the decode/fetch stages
        //figure it out? maybe?

    }

}

