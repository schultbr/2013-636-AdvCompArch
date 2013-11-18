/*
 * CompleteStage.cpp
 *
 *  Created on: Oct 29, 2013
 *      Author: jason cain
 */

#include "CompleteStage.h"
#include "GlobalVars.h"
#include <unistd.h>
#include <stdio.h>
#include <iostream>
#include <stdlib.h>

using namespace std;


bool finishNext = false;

bool sweepROBForFinished() {
    for(size_t i = 0; i < rob.size(); i++) {
        if(rob[i].busy) {
            cout << "ROB: #" << i << " is busy still\n";
            return false;
        }
    }
    return true;
}

void simulateCompleteStage() {
    int dest_tag, rename_tag;
    bool done = false;

    if(isExecuteFinished) {
        if(sweepROBForFinished()) {
            cout << "Complete is now finished" << endl;
            isCompleteFinished = true;
            return;
        }
        else
            finishNext = true;
    }

    if(finishNext) {
        cout << "Complete being forced to finish" << endl;
        isCompleteFinished = true;
        return;
    }

//    int incrementer = robHead;

    DEBUG_COUT << "Complete:\t" << "Completing instructions\n";

    while (!done) {
        DEBUG_COUT << "Complete:\t Checking " << robHead << " busy=" << rob[robHead].busy << endl;
        DEBUG_COUT << "Complete:\t Checking " << robHead << " finished=" << rob[robHead].finished << endl;
        DEBUG_COUT << "Complete:\t Checking " << robHead << " valid=" << rob[robHead].valid << endl;
        if (rob[robHead].busy == 1 && rob[robHead].finished == 1 && rob[robHead].valid == 1) {
            DEBUG_COUT << "Complete:\t Found #" << robHead << " to be ready for complete" << endl;

            done = false;
            rob[robHead].busy = 0;			            //set not busy
            rename_tag = rob[robHead].rename;

            if (rename_tag != -1) {				        //has a destination register
                dest_tag = rrf[rename_tag].dest;
                arf[dest_tag].data = rrf[rename_tag].data;	//copy data from RRF to ARF
                rrf[rename_tag].busy = 0;			    //set not busy

                if (arf[dest_tag].rename == rename_tag)		//data being written to ARF is newest value
                    arf[dest_tag].busy = 0;			        //set not busy
            }

            robHead++;
            if (robHead == (int) rob.size())				    //increment head of circular queue
                robHead = 0;
        }
        else { 			//do we do anything special for finished but not valid?
            DEBUG_COUT << "Complete:\t Hit else case. Done checking." << endl;
            done = true;		//next element was: not busy(hit tail), not finished, or not valid(wait for br to resolve)
        }

//        if (!done && incrementer == ::robTail) {
//            DEBUG_COUT << "Complete:\t Found robTail. Done checking." << endl;
//            done = true;
//        }
    }
}
