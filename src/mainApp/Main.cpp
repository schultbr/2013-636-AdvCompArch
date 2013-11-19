/*
 * main.c
 *
 *  Created on: Oct 22, 2013
 *      Author: brian
 */

#include "StructureDefs.h"
#include "CommandLine.h"
#include <unistd.h>
#include <stdio.h>
#include <iostream>
#include <stdlib.h>

#include "FetchStage.h"
#include "DecodeStage.h"
#include "DispatchStage.h"
#include "IssueStage.h"
#include "ExecuteStage.h"
#include "CompleteStage.h"
#include "BranchPredictor.h"
#include "GlobalVars.h"

using namespace std;

std::queue<Instruction> decodeDispatchBuffer;
std::queue<Instruction> fetchDecodeBuffer;

void determineStatistics() {
    //replace this with IPC calculations and whatnot

    float CPI = (float) cyclesCompleted / instructionCount;

    cout << "Instruction count: " << instructionCount << endl;
    cout << "Cycle count: " << cyclesCompleted << endl;
    cout << "CPI, then, is " << CPI << endl;
}
//
//void clearQueue(){
//	while(decodeDispatchBuffer.size() > 0)
//		decodeDispatchBuffer.pop();
//}

void dumpRegs() {
    cout << "\n================================================\n";
    cout << "\n=====================ARF========================\n";
    cout << "\n================================================\n";
    DEBUG_COUT_2 << "Index\t|Data\t|Busy\t|RRFTag\t" << endl;
    for (size_t i = 0; i < arf.size(); i++) {
        DEBUG_COUT_2 << i << "\t|" << arf[i].data << "\t|" << (arf[i].busy ? "T" : "F") << "\t|" << arf[i].rename << endl;
    }

    cout << "\n================================================\n";
    cout << "\n=====================RRF========================\n";
    cout << "\n================================================\n";
    DEBUG_COUT_2 << "Index\t|Data\t|Busy\t|ARFTag\t|Valid?\t" << endl;
    for (size_t i = 0; i < rrf.size(); i++) {
        DEBUG_COUT_2 << i << "\t|" << rrf[i].data << "\t|" << (rrf[i].busy ? "T" : "F") << "\t|" << rrf[i].dest << "\t|" << (rrf[i].valid ? "T" : "F") << endl;
    }

    cout << "\n================================================\n";
    cout << "\n=====================ROB========================\n";
    cout << "\n================================================\n";
    DEBUG_COUT_2 << "Index\t|Code\t|Busy\t|RRFTag\t|Valid?\t" << endl;
    for (size_t i = 0; i < rob.size(); i++) {
        DEBUG_COUT_2 << i << "\t|" << rob[i].code << "\t|" << (rrf[i].busy ? "T" : "F") << "\t|" << rrf[i].dest << "\t|" << (rrf[i].valid ? "T" : "F") << endl;
    }

}

int runSimulation() {
    bool notDone = true;
//	int i = 0;
//	int max = 2000;
    unsigned int max = 500000; // if this thing runs away... don't wait
    while (notDone) {
        DEBUG_COUT << "Simulating cycle " << cyclesCompleted << endl;
        DEBUG_COUT << "Size3: " << fetchDecodeBuffer.size() << endl;

        simulateCompleteStage();
        simulateExecuteStage();
        simulateIssueStage();
        simulateDispatchStage(decodeDispatchBuffer);
        simulateDecodeStage(fetchDecodeBuffer, decodeDispatchBuffer);
        simulateFetchStage(fetchDecodeBuffer);

        cyclesCompleted++;

        if (isFetchFinished && isDecodeFinished && isDispatchFinished && isIssueFinished && isExecuteFinished && isCompleteFinished)
            notDone = false;

        if (cyclesCompleted == max)
            notDone = false;

        if (cyclesCompleted > 186000)
            dumpRegs();
//
//		if(i%50 == 0)
//		    fetchStalled = false;
        //todo: remove when done testing fetch
//		clearQueue();
    }

    return 1;
}

void printRunningParameters() {
    cout << "\n!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n";
    cout << "**** Running parameters found:\t****\n";
    cout << "Target Trace File:\t\t" << ::inputTraceFile << endl;
    cout << "Superscalar Factor:\t\t" << ::superScalarFactor << endl;
    cout << "BTB Size:\t\t\t" << ::btbSize << endl;
    cout << "Reservation Station Size:\t" << ::rsEntries << endl;
    cout << "Functional Unit Count:\t\t" << ::fuCount << endl;
    cout << "Rename Table Entries:\t\t" << ::renameTableEntries << endl;
    cout << "Reorder Buffer Size:\t\t" << ::reorderBufferEntries << endl;
    cout << "Level 1 Instruction Cache Hit Rate:\t" << ::instrCacheHitRate << endl;
    cout << "Level 1 Instruction Cache Access Time:\t" << ::instrCacheAccessTime << endl;
    cout << "Level 1 Cache Hit Rate:\t\t" << ::level1CacheHitRate << endl;
    cout << "Level 1 Cache Access Time:\t" << ::level1CacheAccessTime << endl;
    cout << "Level 2 Cache Hit Rate:\t\t" << ::level2CacheHitRate << endl;
    cout << "Level 2 Cache Access Time:\t" << ::level2CacheAccessTime << endl;
    cout << "\n!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n\n";
}

void resizeHardwareFromParameters() {
    branchPredictor.resizeBTB(::btbSize);

    //set up global register collections
    arf.resize(32 + 31 + 1 + 1); //need +1+1 for the HI_LO and FCC reg... 63 and 64

    rrf.resize(::renameTableEntries);
    rob.resize(::reorderBufferEntries);
    rs_mem.resize(::rsEntries);
    rs_int.resize(::rsEntries);
    rs_fp.resize(::rsEntries);
    rs_br.resize(::rsEntries);

    fu_add.resize(::fuCount);
    fu_mult.resize(::fuCount);
    fu_fp.resize(::fuCount);
    fu_mem.resize(::fuCount);

    //initilize arf
    for (size_t i = 0; i < arf.size(); i++) {
        arf[i].busy = false;
        arf[i].data = 0;
        arf[i].rename = 0;
    }
}

int main(int argc, char** argv) {

    int returnVal = 0;

    //process command line options to handle inputs
    processCommandLine(argc, argv);

    printRunningParameters();

    //resize stuff from cmd line prompts
    resizeHardwareFromParameters();

    //run simulation
    returnVal = runSimulation();

    cout << "Return code from simulation: " << returnVal << endl << endl;

    //lets figure it out.
    determineStatistics();

    cout << "Exiting.\n";

    return 0;
}
