/*
 * BranchPredictor.cpp
 *
 *  Created on: Nov 12, 2013
 *      Author: brian
 */

#include "BranchPredictor.h"
#include "GlobalVars.h"
#include <unistd.h>
#include <stdio.h>
#include <iostream>
#include <stdlib.h>

using namespace std;

BranchPredictor::BranchPredictor() {
    shiftReg = 0x0000;
    std::fill(predictionTable, predictionTable + 1024, 2);  //set initial prediction to '01', Not Taken
    btbInsertIndex = 0;
    branchPredictionCount = 0;
    predictionMissCount = 0;
}

BranchPredictor::~BranchPredictor() {
}

void BranchPredictor::incrementPredictionMissCount() {
    predictionMissCount++;
}

void BranchPredictor::resizeBTB(int size) {
    btb.resize(size, BTB_Element());
}

//returns true if we were able to predict it at all, and sets predictedTargetPC to either currPC+8 or TargetPC(offset)
//          then, fetch can check predictedTargetPC with the real next PC to determine if we hit or miss th prediction
//returns false if btb didn't have an entry, so we have to call that a mis-predict.
bool BranchPredictor::getPredictionForInstruction(Instruction &instrToPredict) {
    instrToPredict.branchPredictorTableAddress = hash(instrToPredict.PC);

    int predictionTableState = predictionTable[instrToPredict.branchPredictorTableAddress];
    bool res = false;

    branchPredictionCount++;

    if (predictionTableState == 2 || predictionTableState == 3) { //predict this branch is taken
        res = true;
    }

    instrToPredict.predictedTargetPC = -1;

    //else, well, it's already false
    if (res) {
        DEBUG_COUT_3("Branch predicted as taken" << endl);
        for (size_t i = 0; i < btb.size(); i++) {
//            DEBUG_COUT_3("BTB[" << i << "].PC is " << btb[i].instrPC << endl);
            if (btb[i].instrPC == instrToPredict.PC) {
                instrToPredict.predictedTargetPC = btb[i].targetPC;
                DEBUG_COUT_3("BTB found target pc at " << i << endl);
                return true; //branch predicted taken _AND_ we have a target pc
            }
        }

        if (instrToPredict.predictedTargetPC == -1) {
            DEBUG_COUT_3("BTB didn't have the target PC..." << endl);
            return false; //branch predicted taken but BTB miss
        }
    }
    else {
//        instrToPredict.predictedTargetPC = instrToPredict.PC + 8; //don't do this here. do it in fetch...
        DEBUG_COUT_3("Branch predicted as not taken" << endl);
        return false; //branch predicted not taken
    }

    cout << "ERRORERRORERROR" << endl;
    return false; //if we got here, stuff is super screwed up.
}

//void BranchPredictor::updatePredictionWithResults(Instruction &executedInstr){
void BranchPredictor::updatePredictorWithResults(FU_Element entry) {
    DEBUG_COUT_3("Predictor:\t Updating predictor with branch execution results (ptable add: " << entry.PTaddr <<")" << endl);
    if (entry.BRoutcome) {
        //update the state machine with the current results
        inc_state(entry.PTaddr);
        shift_left(1);
    }
    else {
        dec_state(entry.PTaddr);
        shift_left(0);
    }

//    DEBUG_COUT_3("Updating BTB record for " << entry.PC << endl);
    updateBTBRecord(entry.PC, entry.BTaddr, entry.BRoutcome);
}

void BranchPredictor::printPredictionStatistics() {
    unsigned int correctPredictions = branchPredictionCount - predictionMissCount;
    cout << endl;
    cout << "Branch predictor predicted " << branchPredictionCount << " branches and got " << predictionMissCount << " wrong... oops." << endl;
    cout << "\tBranch correct prediction rate: " << ((float)correctPredictions / branchPredictionCount) * 100 << "%" << endl;
    cout << "\tBranch mis-prediction rate: " << ((float) predictionMissCount / branchPredictionCount) * 100 << "%" << endl << endl;

    printBTB();
}

//////////////////////////////////////////////////////////////
//							                            	//
//		Private functions. Not needed by anyone else	    //
//								                            //
//////////////////////////////////////////////////////////////
//shift function
void BranchPredictor::shift_left(bool bit) {
//    DEBUG_COUT_3("Shifting the branch" << endl);
    shiftReg = shiftReg << 1;			//shift 0 into lsb
    shiftReg = shiftReg & 0x03FF; 			//only use lower 10 bits
    if (bit == 1)
        shiftReg = shiftReg | 0x0001;			//shift 1 into lsb
}

//hash function
//if hash is not typecast or returns an addr greater than 10 bits, the table value will be out of range and may come back 0 or crazy value
short BranchPredictor::hash(int pc) {
    short hash = 0;
    int y = pc >> 3; //per 11/14/2013 class, ignore lowest 3 bits due to being multiples of 8.. they'd be the same each time.
    short x = (short) y;
    x = x & 0x03FF;		//only use lower 10 bits
    hash = shiftReg ^ x;	//bitwise XOR

//    DEBUG_COUT_3("Hash returned is " << hash << endl);

    return hash;
}

//return Prediction from Table
int BranchPredictor::get_bp(int hashAddr) {
    return predictionTable[hashAddr];
}

//increment Prediction State
void BranchPredictor::inc_state(int hashAddr) {
    int x = predictionTable[hashAddr];
    switch (x)			//increment state machine
    {
        case 0:
            predictionTable[hashAddr] = 1;
            break;
        case 1:
            predictionTable[hashAddr] = 2;
            break;
        case 2:
            predictionTable[hashAddr] = 3;
            break;
        case 3:
            predictionTable[hashAddr] = 3;
            break;
        default:
            cout << "ERRORERRORERROR - inc_state" << endl;
            break; //should we bother with a default?
    }
}

//decrement Prediction State
void BranchPredictor::dec_state(int hashAddr) {
    int y = predictionTable[hashAddr];
    switch (y)			//decrement state machine
    {
        case 0:
            predictionTable[hashAddr] = 0;
            break;
        case 1:
            predictionTable[hashAddr] = 0;
            break;
        case 2:
            predictionTable[hashAddr] = 1;
            break;
        case 3:
            predictionTable[hashAddr] = 2;
            break;
        default:
            cout << "ERRORERRORERROR - dec state" << endl;
            break; //should we bother with a default?
    }
}

void BranchPredictor::updateBTBRecord(int instrPC, int brachTarget, bool wasTaken) {

    //find the entry in the btb that matches the branch PC we're updating
    for (size_t i = 0; i < btb.size(); i++) {
        if (btb[i].instrPC == instrPC) {
            DEBUG_COUT_3("Checking index " << i << " in btb for PC " << instrPC << ". Found " << btb[i].instrPC << endl);
//            btb[i].lastPredictedTaken = wasTaken;
            btb[i].targetPC = brachTarget;
            return;
        }
    }
    //if it ins't there, insert the branch's PC into the index @ btbInsertIndex's location. Round-robin insert
    btb[btbInsertIndex].instrPC = instrPC;
//    btb[btbInsertIndex].lastPredictedTaken = wasTaken;
    btb[btbInsertIndex].targetPC = brachTarget;

    DEBUG_COUT_3("BranchPredictor:\tBTB Updated index " << btbInsertIndex << " to have PC: " << btb[btbInsertIndex].instrPC << " and targetPC: " << btb[btbInsertIndex].targetPC << endl);

    btbInsertIndex++; //next entry

    if (btbInsertIndex == (int) btb.size()) {
        btbInsertIndex = 0; //if we reached the max size, wrap around to 0;
    }

}

void BranchPredictor::printBTB(){
    DEBUG_COUT_3("Branch Address \t\t| Target Address" << endl;)
    for(size_t i = 0; i < btb.size(); i++)
        DEBUG_COUT_3("\t" << btb[i].instrPC << "\t\t|\t" << btb[i].targetPC << endl);

    DEBUG_COUT_3(endl << endl);


    DEBUG_COUT_3("PTable Addr\t\t|PTable State" << endl);
    for(int i = 0; i < 1024; i++)
        DEBUG_COUT_3("\t" << i << "\t\t|\t" << predictionTable[i] << endl);

    DEBUG_COUT_3(endl << endl);
}

