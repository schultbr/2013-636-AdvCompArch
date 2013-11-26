/*
 * ExecuteStage.cpp
 *
 *  Created on: Oct 29, 2013
 *      Author: jason cain
 */

#include "IssueStage.h"
#include "GlobalVars.h"
#include "StructureDefs.h"
#include <unistd.h>
#include <stdio.h>
#include <iostream>
#include <stdlib.h>

using namespace std;

void copyToFU(RS_Element entry, std::vector<FU_Element> &targetFU, int index, int cnt) {
    targetFU[index].op1 = entry.op1;
    targetFU[index].op2 = entry.op2;
    targetFU[index].reorder = entry.reorder;
    targetFU[index].code = entry.code;
    targetFU[index].PC = entry.PC;
    targetFU[index].count = cnt;
    targetFU[index].isFirstClock = true;

}

void copyToBranchFU(RS_Element entry, FU_Element &targetFU) {	//single element FU, not a vector
    targetFU.op1 = entry.op1;
    targetFU.op2 = entry.op2;
    targetFU.reorder = entry.reorder;
    targetFU.code = entry.code;
    targetFU.PC = entry.PC;
    targetFU.PTaddr = entry.PTaddr;
    targetFU.BTaddr = entry.BTaddr;
    targetFU.BRoutcome = entry.BRoutcome;
    targetFU.BRprediction = entry.BRprediction;
    targetFU.count = 1;
}

//function to check for empty FU
int checkFU(std::vector<FU_Element> *targetFU) {
    int FU_tag = -1;				//default = no empty slots

    for (size_t i = 0; i < targetFU->size(); i++) { //iterate through Functional Units
        if (targetFU->at(i).count == 0)	{ //if not busy
            FU_tag = i;
            break;
        }
    }
    return FU_tag;
}

//function to compare waiting RS operands with "broadcast" results
void checkValue(std::vector<RS_Element> *targetRS) {
    int rename_tag = 0;

    for (size_t i = 0; i < targetRS->size(); i++) {

        if (targetRS->at(i).valid1 == false) {        //check if op1 is valid
            rename_tag = targetRS->at(i).op1;	//set rrf tag
            if (rename_tag != -1 && rrf[rename_tag].valid == true){ //if rrf is valid, copy data
                targetRS->at(i).op1 = rrf[rename_tag].data;
                targetRS->at(i).valid1 = true;
            }
        }
        if (targetRS->at(i).valid2 == 0){           //check for valid op2 value
            rename_tag = targetRS->at(i).op2;
//            DEBUG_COUT("Issue: checking for new op2 value at rrf[" << rename_tag << "]" << endl;
            if (rename_tag != -1 && rrf[rename_tag].valid == true) {
                targetRS->at(i).op2 = rrf[rename_tag].data;
                targetRS->at(i).valid2 = true;
            }
        }
        if (targetRS->at(i).valid1 == true && targetRS->at(i).valid2 == true)	//set ready bit
            targetRS->at(i).ready = true;
    }
}

//function to check RS for ready instructions and issue if FU is not busy
void checkReady(std::vector<RS_Element> *targetRS) {
    int FU_tag = 0;
    int cnt = targetRS->size();

    //DEBUG_COUT("Issue: Check if RS is Ready" << endl;
    for (int i = 0; i < cnt; i++) {
        if (targetRS->at(i).ready == true){ //RS entry is ready

            switch (targetRS->at(i).code) { //check FU for empty slot
                case LOGICAL:
                case ADD_SUB_I:
                    FU_tag = checkFU(&fu_add);

                    if (FU_tag != -1) {

                        //copy RS entry to FU slot & set cycle count
                        copyToFU(targetRS->at(i), fu_add, FU_tag, 1);
                        rob[targetRS->at(i).reorder].issued = true;
                        targetRS->erase(targetRS->begin() + i);	//"pop" RS entry off queue
                        i--;		//erase will reindex vector so i needs adjusted
                        cnt--;		//erase will reindex vector so cnt needs adjusted

                        targetRS->resize(targetRS->size() + 1, RS_Element()); //"push" empty RS entry onto queue
                        rs_int_inUse--;
                        fu_add_inUse++;
                    }
                    break;

                case MULT_DIV_I:
                    FU_tag = checkFU(&fu_mult);

                    if (FU_tag != -1) {
                        copyToFU(targetRS->at(i), fu_mult, FU_tag, 3);
                        rob[targetRS->at(i).reorder].issued = true;
                        targetRS->erase(targetRS->begin() + i);
                        i--;
                        cnt--;

                        targetRS->resize(targetRS->size() + 1, RS_Element());
                        rs_int_inUse--;
                        fu_mult_inUse++;
                    }
                    break;

                case FLOATING_POINT:
                    FU_tag = checkFU(&fu_fp);

                    if (FU_tag != -1) {
                        copyToFU(targetRS->at(i), fu_fp, FU_tag, 5);
                        rob[targetRS->at(i).reorder].issued = true;
                        targetRS->erase(targetRS->begin() + i);
                        i--;
                        cnt--;

                        targetRS->resize(targetRS->size() + 1, RS_Element());
                        rs_fp_inUse--;
                        fu_fp_inUse++;
                    }
                    break;

                case LOAD:
                case STORE:
                    //if(i == 0)	Would implement if we needed to issue MEM instr in order
                    FU_tag = checkFU(&fu_mem);

                    if (FU_tag != -1) {
                        copyToFU(targetRS->at(i), fu_mem, FU_tag, 1); //only adding 1 cycle to count because L1 access time
                        //will add at least 1 additional cycle during ExecuteStage, for a min of 2 cycles
                        rob[targetRS->at(i).reorder].issued = true;
                        targetRS->erase(targetRS->begin() + i);
                        i--;
                        cnt--;

                        targetRS->resize(targetRS->size() + 1, RS_Element());
                        rs_mem_inUse--;
                        fu_mem_inUse++;
                    }
                    break;

                case JUMP:
                case BRANCH:
                    if (fu_br.count == 0) {	//FU empty
                        copyToBranchFU(targetRS->at(i), fu_br);
                        rob[targetRS->at(i).reorder].issued = true;
                        targetRS->erase(targetRS->begin() + i);

                        targetRS->resize(targetRS->size() + 1, RS_Element());
                        //can only issue 1 per cycle so no reindexing needed
                        rs_br_inUse--;
                        fu_br_inUse++;
                    }
                    break;

                case NOP: //NOP goes straight to ROB with complete marked as true -- brs
                default:
                    break;
            }
        }
    }
}

//quick sweep to see if all of our RS's are empty.
//returns true if empty
//returns false if any element is still busy
bool checkForFinished(std::vector<RS_Element> *targetRS) {
    for (size_t i = 0; i < targetRS->size(); i++) {
        if (targetRS->at(i).busy)
            return false;
    }
    return true;
}

void simulateIssueStage() {

    if (!isIssueFinished && (isDispatchFinished && checkForFinished(&rs_int) && checkForFinished(&rs_fp) && checkForFinished(&rs_mem) && checkForFinished(&rs_br))) {
        cout << "Issue is now finished\n";
        isIssueFinished = true;
        return;
    }
    else if (isIssueFinished)
        return;

    //DEBUG_COUT("Issue Stage\n";

    //check Common Data Bus for updates
    checkValue(&rs_int);
    checkValue(&rs_fp);
    checkValue(&rs_mem);
    checkValue(&rs_br);

    //check RS for ready instructions and issue if FU is not busy
    checkReady(&rs_int);
    checkReady(&rs_fp);
    checkReady(&rs_mem);
    checkReady(&rs_br);
}

