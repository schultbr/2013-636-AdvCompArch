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
    cout << "Target Trace File:\t\t" <<  ::inputTrace << endl;
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
