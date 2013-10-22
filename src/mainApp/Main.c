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



int runSimulation()
{

	return 1;
}

int main(int argc, char** argv)
{

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
						&superScalarFactor,
						&btbSize,
						&rsEntries,
						&fuCount,
						&renameTableEntries,
						&reorderBufferEntries,
						&level1CacheHitRate,
						&level1CacheAccessTime,
						&level2CacheHitRate,
						&level2CacheAccessTime,
						inputTrace);


	//run simulation
	returnVal = runSimulation();

	printf("Return code from simulation: %d\n\n", returnVal);
	printf("Exiting.\n");

	return 0;
}
