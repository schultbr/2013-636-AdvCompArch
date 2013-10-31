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

int runSimulation() {

//	simulateCompleteCycle();
//	simulateExecuteCycle();
//	simulateIssueCycle();
//	simulateDispatchCycle();
//	simulateDecodeCycle();
	simulateFetchCycle();

	return 1;
}

void printRunningParameters()
{
    cout << "\n!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n";
    cout << "**** Running parameters found:\t****\n";
    cout << "Target Trace File:\t\t" <<  ::inputTrace;
    cout << "Superscalar Factor:\t\t" << ::superScalarFactor;
    cout << "BTB Size:\t\t\t" << ::btbSize;
    cout << "Reservation Station Size:\t" << ::rsEntries;
    cout << "Functional Unit Count:\t\t" << ::fuCount;
    cout << "Rename Table Entries:\t\t" << ::renameTableEntries;
    cout << "Level 1 Instruction Cache Hit Rate:\t" << ::instrCacheHitRate;
    cout << "Level 1 Instruction Cache Access Time:\t" << ::instrCacheAccessTime;
    cout << "Level 1 Cache Hit Rate:\t\t" << ::level1CacheHitRate;
    cout << "Level 1 Cache Access Time:\t" << ::level1CacheAccessTime;
    cout << "Level 2 Cache Hit Rate:\t\t" << ::level2CacheHitRate;
    cout << "Level 2 Cache Access Time:\t" << ::level2CacheAccessTime;
    cout << "\n!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n\n";
}

int main(int argc, char** argv) {

	int returnVal = 0;

	//set up global register collections
	registers.resize(32);
	fpRegisters.resize(31);

	//process command line options to handle inputs
	processCommandLine(argc, argv);

	printRunningParameters();


	//run simulation
	returnVal = runSimulation();

	cout << "Return code from simulation: " <<  returnVal << endl << endl;
	cout << "Exiting.\n";

	return 0;
}
