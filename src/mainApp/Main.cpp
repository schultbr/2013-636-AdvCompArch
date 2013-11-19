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
    DEBUG_COUT_2 << "\n================================================\n";
    DEBUG_COUT_2 << "\n=====================ARF========================\n";
    DEBUG_COUT_2 << "\n================================================\n";
    DEBUG_COUT_2 << "Index\t| Data\t| Busy\t| RRF\t" << endl;
    for (size_t i = 0; i < arf.size(); i++) {
        DEBUG_COUT_2 << i << "\t| " << arf[i].data << "\t| " << (arf[i].busy ? "T" : "F") << "\t| " << arf[i].rename << endl;
    }

    DEBUG_COUT_2 << "\n================================================\n";
    DEBUG_COUT_2 << "\n=====================RRF========================\n";
    DEBUG_COUT_2 << "\n================================================\n";
    DEBUG_COUT_2 << "Index\t| Data\t| Busy\t| ARF\t| Valid?\t" << endl;
    for (size_t i = 0; i < rrf.size(); i++) {
        DEBUG_COUT_2 << i << "\t| " << rrf[i].data << "\t| " << (rrf[i].busy ? "T" : "F") << "\t| " << rrf[i].dest << "\t| " << (rrf[i].valid ? "T" : "F") << endl;
    }

    DEBUG_COUT_2 << "\n================================================\n";
    DEBUG_COUT_2 << "\n=====================ROB========================\n";
    DEBUG_COUT_2 << "\n================================================\n";
    DEBUG_COUT_2 << "Index\t| Code\t| Busy\t| RRF\t| Valid\t| Finished" << endl;
    for (size_t i = 0; i < rob.size(); i++) {
        DEBUG_COUT_2 << i << "\t| " << rob[i].code << "\t| " << (rob[i].busy ? "T" : "F") << "\t| " << rob[i].rename << "\t| " << (rob[i].valid ? "T" : "F") << "\t| " << (rob[i].finished ? "T" : "F") << endl;
    }

}

int runSimulation() {
    bool notDone = true;
//	int i = 0;
//	unsigned int max = 150;
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

#ifdef DEBUG2
//        if (cyclesCompleted > 186000)
//            dumpRegs();
#endif
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
    arf.resize(32 + 31 + 1 + 1, ARF_Element()); //need +1+1 for the HI_LO and FCC reg... 63 and 64

    rrf.resize(::renameTableEntries, RRF_Element());
    rob.resize(::reorderBufferEntries, ROB_Element());
    rs_mem.resize(::rsEntries, RS_Element());
    rs_int.resize(::rsEntries, RS_Element());
    rs_fp.resize(::rsEntries, RS_Element());
    rs_br.resize(::rsEntries, RS_Element());

    fu_add.resize(::fuCount, FU_Element());
    fu_mult.resize(::fuCount, FU_Element());
    fu_fp.resize(::fuCount, FU_Element());
    fu_mem.resize(::fuCount, FU_Element());

//    //initilize arf
//    for (size_t i = 0; i < arf.size(); i++) {
//        arf[i].busy = false;
//        arf[i].data = 0;
//        arf[i].rename = 0;
//    }
}

void printFU(FU_Element entry)
{
	cout << "PC " << entry.PC << endl;
	cout << "count: " << entry.count << endl;
	cout << "op1: " << entry.op1  << endl;
	cout << "op2: " << entry.op2 << endl;
	cout << "reorder " << entry.reorder << endl;
	cout << "data result " << entry.result << endl;
	cout << "PredictionT Addr: " << entry.PTaddr << endl;
	cout << "Branch Outcome: " << entry.BRoutcome << endl;
	cout << "Branch Prediction " << entry.BRprediction << endl;
	cout << "Branch Target Addr: " << entry.BTaddr << endl << endl;
}

void printRS(RS_Element entry)
{
	cout << "PC " << entry.PC << endl;
	cout << "busy: " << entry.busy << endl;
	cout << "valid1: " << entry.valid1 << endl;
	cout << "valid2: " << entry.valid2 << endl;
	cout << "ready: " << entry.ready << endl;
	cout << "op1: " << entry.op1  << endl;
	cout << "op2: " << entry.op2 << endl;
	cout << "reorder " << entry.reorder << endl;
	cout << "OPcode " << entry.code << endl;
	cout << "PredictionT Addr: " << entry.PTaddr << endl;
	cout << "Branch Outcome: " << entry.BRoutcome << endl;
	cout << "Branch Prediction " << entry.BRprediction << endl;
	cout << "Branch Target Addr: " << entry.BTaddr << endl << endl;
}

void printROB(ROB_Element entry)
{
	cout << "busy: " << entry.busy << ", finished: " << entry.finished << ", valid: " << entry.valid << ", issued: " << entry.issued
		<< ", OPcode: " << entry.code << ", PC: " << entry.PC << ", rename: " << entry.rename << endl;
}

void printRRF(RRF_Element entry)
{
	cout << "busy: " << entry.busy << ", valid: " << entry.valid << ", data: " << entry.data << ", dest: " << entry.dest << endl;
}

void printARF(ARF_Element entry)
{
	cout << "busy: " << entry.busy << ", data: " << entry.data << ", rename: " << entry.rename << endl;
}

int main(int argc, char** argv) {

    int returnVal = 0;

    //process command line options to handle inputs
    processCommandLine(argc, argv);

    printRunningParameters();

    //resize stuff from cmd line prompts
    resizeHardwareFromParameters();

    dumpRegs();

    //run simulation
    returnVal = runSimulation();

    cout << "Return code from simulation: " << returnVal << endl << endl;

    //lets figure it out.
    determineStatistics();


	//print final buffers
	/*
	cout << "Printing RRF" << endl;
	for (int i=0;i<rrf.size();i++)
	{
		cout << "RRF[" << i << "] = ";
		printRRF(rrf[i]);
	}

	cout << "Printing ARF" << endl;
	for (int i=0;i<arf.size();i++)
	{
		cout << "ARF[" << i << "] = ";
		printARF(arf[i]);
		cout << endl;
	}

	cout << "Printing ROB - Head = " << robHead << ", Tail = " << robTail << endl;
	for (int i=0;i<rob.size();i++)
	{
		printROB(rob[i]);
	}
	*/

    cout << "Exiting.\n";

    return 0;
}
