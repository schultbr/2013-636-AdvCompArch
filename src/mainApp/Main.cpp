/*
 * main.c
 *
 *  Created on: Oct 22, 2013
 *      Author: brian
 */

#include "StructureDefs.h"
#include "ReservationStations.h"
#include "FunctionalUnits.h"
#include "CommandLine.h"
#include <unistd.h>
#include <stdio.h>
#include <iostream>
#include <stdlib.h>

#include "FetchStage.h"
#include "GlobalVars.h"

using namespace std;

std::queue<Instruction> fetchDecodeBuffer;

void determineStatistics(){
	//replace this with IPC calculations and whatnot

	cout << "Instruction count: " << instructionCount << endl;
	cout << "Cycle count: " << cyclesCompleted << endl;
}

void clearQueue(){
	while(fetchDecodeBuffer.size() > 0)
		fetchDecodeBuffer.pop();
}

int runSimulation() {
	bool notDone = true;
	int i = 0;
	int max = 50;
	while(notDone) {
	//	simulateCompleteCycle();
	//	simulateExecuteCycle();
	//	simulateIssueCycle();
	//	simulateDispatchCycle();
	//	simulateDecodeCycle();
		simulateFetchCycle(fetchDecodeBuffer);

		cyclesCompleted++;

		i++;
		if(i == max)
			notDone = false;

		//todo: remove when done testing fetch
		clearQueue();
	}

	return 1;
}

void printRunningParameters()
{
    cout << "\n!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n";
    cout << "**** Running parameters found:\t****\n";
    cout << "Target Trace File:\t\t" <<  ::inputTraceFile << endl;
    cout << "Superscalar Factor:\t\t" << ::superScalarFactor << endl;
    cout << "BTB Size:\t\t\t" << ::btbSize << endl;
    cout << "Reservation Station Size:\t" << ::rsEntries << endl;
    cout << "Functional Unit Count:\t\t" << ::fuCount << endl;
    cout << "Rename Table Entries:\t\t" << ::renameTableEntries << endl;
    cout << "Level 1 Instruction Cache Hit Rate:\t" << ::instrCacheHitRate << endl;
    cout << "Level 1 Instruction Cache Access Time:\t" << ::instrCacheAccessTime << endl;
    cout << "Level 1 Cache Hit Rate:\t\t" << ::level1CacheHitRate << endl;
    cout << "Level 1 Cache Access Time:\t" << ::level1CacheAccessTime << endl;
    cout << "Level 2 Cache Hit Rate:\t\t" << ::level2CacheHitRate << endl;
    cout << "Level 2 Cache Access Time:\t" << ::level2CacheAccessTime << endl;
    cout << "\n!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n\n";
}

int main(int argc, char** argv) {

	int returnVal = 0;

	//set up global register collections
	registers.resize(32+31);
//	fpRegisters.resize(31);

	//process command line options to handle inputs
	processCommandLine(argc, argv);

	printRunningParameters();

	//run simulation
	returnVal = runSimulation();

	determineStatistics();

	cout << "Return code from simulation: " <<  returnVal << endl << endl;
	cout << "Exiting.\n";

	return 0;
}
