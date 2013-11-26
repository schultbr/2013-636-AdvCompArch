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
    for (size_t i = 0; i < rob.size(); i++) {
        if (rob[i].busy) {
            cout << "ROB: #" << i << " is busy still\n";
            return false;
        }
    }
    return true;
}

void simulateCompleteStage() {
    int dest_tag, rename_tag;
    bool done = false;

    if (!isCompleteFinished && isExecuteFinished) {
        if (sweepROBForFinished()) {
            cout << "Complete is now finished" << endl;
            isCompleteFinished = true;
            return;
        }
        else
            finishNext = true;
    }
    else if (isCompleteFinished)
        return;

    if (finishNext) {
        cout << "Complete being forced to finish" << endl;
        isCompleteFinished = true;
        return;
    }

    while (!done) {

        if (rob[robHead].busy == 1 && rob[robHead].finished == 1 && rob[robHead].valid == 1) {
            done = false;
            rob[robHead].busy = false;			            //set not busy
            robEntries--;
            rob_inUse--;
            rename_tag = rob[robHead].rename;

            if (rename_tag != -1) {				        //has a destination register

                dest_tag = rrf[rename_tag].dest;

                arf[dest_tag].data = rrf[rename_tag].data;	//copy data from RRF to ARF
                rrf[rename_tag].busy = false;			    //set not busy

                if (arf[dest_tag].rename == rename_tag)		//data being written to ARF is newest value
                    arf[dest_tag].busy = false;			        //set not busy
            }

            robHead++;
            if (robHead == (int) rob.size())				    //increment head of circular queue
                robHead = 0;

        }
        else {
            done = true;		//next element was: not busy(hit tail), not finished, or not valid(wait for br to resolve)
        }
    }
}

