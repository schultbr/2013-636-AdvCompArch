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
    for (size_t i = 0; i < targetFU->size(); i++) {	//iterate through Functional Units
        DEBUG_COUT << "Issue:\t" << "FU #" << i << " has " << targetFU->at(i).count << " cycles remaining \n";
        if (targetFU->at(i).count == 0) {		//if not busy
            FU_tag = i;
            break; //we don't need to check every FU. issue to the first empty one.
        }
    }
    return FU_tag;
}

//function to compare waiting RS operands with "broadcast" results
void checkValue(std::vector<RS_Element> *targetRS) {
    int rename_tag = 0;

    for (size_t i = 0; i < targetRS->size(); i++) {
        if (targetRS->at(i).valid1 == 0) {			//check if op1 is valid
            rename_tag = targetRS->at(i).op1;	//set rrf tag
            if (rrf[rename_tag].valid == 1) {		//if rrf is valid, copy data
                targetRS->at(i).op1 = rrf[rename_tag].data;
                targetRS->at(i).valid1 = 1;
            }
        }
        if (targetRS->at(i).valid2 == 0) {			//check for valid op2 value
            rename_tag = targetRS->at(i).op2;
            if (rrf[rename_tag].valid == 1) {
                targetRS->at(i).op2 = rrf[rename_tag].data;
                targetRS->at(i).valid2 = 1;
            }
        }
        if (targetRS->at(i).valid1 == 1 && targetRS->at(i).valid2 == 1)	//set ready bit
            targetRS->at(i).ready = 1;
    }
}

//function to check RS for ready instructions and issue if FU is not busy
void checkReady(std::vector<RS_Element> *targetRS) {
    int FU_tag;
    for (size_t i = 0; i < targetRS->size(); i++) {
        if (targetRS->at(i).ready == 1) {		//RS entry is ready
            switch (targetRS->at(i).code)	//check FU for empty slot
            {
                case LOGICAL:
                case ADD_SUB_I:
                    FU_tag = checkFU(&fu_add);
                    DEBUG_COUT << "Issue:\t" << "Checking add fu gave tag of " << FU_tag << endl;
                    if (FU_tag != -1) {
                        //copy RS entry to FU slot & set cycle count
                        copyToFU(targetRS->at(i), fu_add, FU_tag, 1);

                        DEBUG_COUT << "Issue:\t" << "Issued instruction " << targetRS->at(i).PC << " to fu_add #" << FU_tag << endl;
//                        DEBUG_COUT << "Issue:\t" << "RS size starting = " << targetRS->size() << "\n";
                        targetRS->erase(targetRS->begin() + i);			//"pop" RS entry off queue
//                        DEBUG_COUT << "Issue:\t" << "RS size after issue pop = " << targetRS->size() << "\n";
                        targetRS->resize(targetRS->size() + 1); //"push" empty RS entry onto queue
//                        DEBUG_COUT << "Issue:\t" << "RS size after issue push = " << targetRS->size() << "\n";
                    }
                    break;

                case MULT_DIV_I:
                    FU_tag = checkFU(&fu_mult);
                    DEBUG_COUT << "Issue:\t" << "Checking mult fu gave tag of " << FU_tag << endl;
                    if (FU_tag != -1) {
                        DEBUG_COUT << "Issue:\t" << "Issued mult instruction " << targetRS->at(i).PC << " to fu_mult #" << FU_tag << endl;
                        copyToFU(targetRS->at(i), fu_mult, FU_tag, 3);
                        targetRS->erase(targetRS->begin() + i);
                        targetRS->resize(targetRS->size() + 1);
                    }
                    break;

                case FLOATING_POINT:
                    FU_tag = checkFU(&fu_fp);
                    DEBUG_COUT << "Issue:\t" << "Checking fp fu gave tag of " << FU_tag << endl;
                    if (FU_tag != -1) {
                        DEBUG_COUT << "Issue:\t" << "Issued fp instruction " << targetRS->at(i).PC << " to fu_fp #" << FU_tag << endl;
                        copyToFU(targetRS->at(i), fu_fp, FU_tag, 5);
                        targetRS->erase(targetRS->begin() + i);
                        targetRS->resize(targetRS->size() + 1);
                    }
                    break;

                    //********************* DO WE NEED TO ISSUE MEM IN ORDER? ********
                    //********************* IF SO CHANGE THIS CASE *******************
                case LOAD:
                case STORE:
                    FU_tag = checkFU(&fu_mem);
                    DEBUG_COUT << "Issue:\t" << "CheckFU fu_mem gave tag of " << FU_tag << endl;
                    if (FU_tag != -1) {
                        DEBUG_COUT << "Issue:\t" << "Issued mem instruction " << targetRS->at(i).PC << " to fu_mem #" << FU_tag << endl;
                        copyToFU(targetRS->at(i), fu_mem, FU_tag, 2);
                        targetRS->erase(targetRS->begin() + i);
                        targetRS->resize(targetRS->size() + 1);
                    }
                    break;

                    //********************* WHAT DO WE DO WITH NOP AND JUMP? ********

                case JUMP: // JUMP goes here. Correct. --Brs
                case BRANCH:
                    DEBUG_COUT << "Issue:\t" << "Checking branch " << fu_br.count  << endl;
                    if (fu_br.count == 0) {	//FU empty
                        DEBUG_COUT << "Issue:\t" << "Issued br instruction " << targetRS->at(i).PC << " to fu_br #" << FU_tag << endl;
                        copyToBranchFU(targetRS->at(i), fu_br);
                        targetRS->erase(targetRS->begin() + i);
                        targetRS->resize(targetRS->size() + 1);
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
    for(size_t i = 0; i < targetRS->size(); i++) {
        if(targetRS->at(i).busy)
            return false;
    }
    return true;
}

void simulateIssueStage() {
//	int rename_tag = 0;
//	unsigned i;

    if(isDispatchFinished &&
        checkForFinished(&rs_int) &&
        checkForFinished(&rs_fp) &&
        checkForFinished(&rs_mem) &&
        checkForFinished(&rs_br) ) {

        cout << "Issue is now finished\n";
        isIssueFinished = true;
        return;
    }

    DEBUG_COUT << "Issue Stage\n";

    //check Common Data Bus for updates
    DEBUG_COUT << "Issue:\t" << " Checking cdb for rs_int" << endl;
    checkValue(&rs_int);
    DEBUG_COUT << "Issue:\t" << " Checking cdb for rs_fp" << endl;
    checkValue(&rs_fp);
    DEBUG_COUT << "Issue:\t" << " Checking cdb for rs_mem" << endl;
    checkValue(&rs_mem);
    DEBUG_COUT << "Issue:\t" << " Checking cdb for rs_br" << endl;
    checkValue(&rs_br);

    //check RS for ready instructions and issue if FU is not busy
    DEBUG_COUT << "Issue:\t" << " Checking rs_int for issue" << endl;
    checkReady(&rs_int);
    DEBUG_COUT << "Issue:\t" << " Checking rs_fp for issue" << endl;
    checkReady(&rs_fp);
    DEBUG_COUT << "Issue:\t" << " Checking rs_mem for issue" << endl;
    checkReady(&rs_mem);
    DEBUG_COUT << "Issue:\t" << " Checking rs_br for issue" << endl;
    checkReady(&rs_br);

}

