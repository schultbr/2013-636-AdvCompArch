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
#include <stdlib.h>

#include "FetchStage.h"


int runSimulation() {

//	simulateCompleteCycle();
//	simulateExecuteCycle();
//	simulateIssueCycle();
//	simulateDispatchCycle();
//	simulateDecodeCycle();
	simulateFetchCycle();

	return 1;
}

void printRunningParameters(int &superScalarFactor,
		int &btbSize,
		int &rsEntries,
		int &fuCount,
		int &renameTableEntries,
		int &reorderBufferEntries,
		float &level1CacheHitRate,
		int &level1CacheAccessTime,
		float &level2CacheHitRate,
		int &level2CacheAccessTime,
		char *inputTrace)
{
    printf("\n!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n");
    printf("**** Running parameters found:\t****\n");
    printf("Target Trace File:\t\t%s\n", inputTrace);
    printf("Superscalar Factor:\t\t%d\n", superScalarFactor);
    printf("BTB Size:\t\t\t%d\n", btbSize);
    printf("Reservation Station Size:\t%d\n", rsEntries);
    printf("Functional Unit Count:\t\t%d\n", fuCount);
    printf("Rename Table Entries:\t\t%d\n", renameTableEntries);
    printf("Level 1 Cache Hit Rate:\t\t%f\n", level1CacheHitRate);
    printf("Level 1 Cache Access Time:\t%d\n", level1CacheAccessTime);
    printf("Level 2 Cache Hit Rate:\t\t%f\n", level2CacheHitRate);
    printf("Level 2 Cache Access Time:\t%d\n", level2CacheAccessTime);
    printf("\n!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n\n");
}

int main(int argc, char** argv) {
	int returnVal = 0;
	int superScalarFactor = 0;
	int btbSize = 0;
	int rsEntries = 0;
	int fuCount = 0;
	int renameTableEntries = 0;
	int reorderBufferEntries = 0;
	float level1CacheHitRate = 0.0;
	int level1CacheAccessTime = 0.0;
	float level2CacheHitRate = 0.0;
	int level2CacheAccessTime = 0.0;
	char inputTrace[50];


	//process command line options to handle inputs
	processCommandLine(argc, argv,
						superScalarFactor,
						btbSize,
						rsEntries,
						fuCount,
						renameTableEntries,
					    reorderBufferEntries,
						level1CacheHitRate,
						level1CacheAccessTime,
						level2CacheHitRate,
						level2CacheAccessTime,
						inputTrace);

	printRunningParameters(superScalarFactor,
	                       btbSize,
	                       rsEntries,
	                       fuCount,
	                       renameTableEntries,
	                       reorderBufferEntries,
	                       level1CacheHitRate,
	                       level1CacheAccessTime,
	                       level2CacheHitRate,
	                       level2CacheAccessTime,
	                       inputTrace);


	//run simulation
	returnVal = runSimulation();

	printf("Return code from simulation: %d\n\n", returnVal);
	printf("Exiting.\n");

	return 0;
}
