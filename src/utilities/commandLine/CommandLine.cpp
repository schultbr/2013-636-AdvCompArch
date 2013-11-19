/*
 * CommandLine.c
 *
 *  Created on: Oct 22, 2013
 *      Author: brian
 */

#include "CommandLine.h"
#include "GlobalVars.h"
#include <unistd.h>
#include <stdio.h>
#include <iostream>
#include <stdlib.h>
#include <string.h>
#include <getopt.h>

using namespace std;

void print_usage() {
    cout << "Usage: project2 --ss NUM --btb NUM --rs NUM \n";
    cout << "                 --fu NUM --rnt NUM --rob NUM \n";
    cout << "                 --inshr NUM --l1hr NUM --l1at NUM  \n";
    cout << "                 --l2hr NUM --l2at NUM --mmat NUM\n";
    cout << "                 --trace TRACE_FILE\n";
    cout << "Key:\n";
    cout << "--ss = SuperScalar Count --btb = BTB Size --rs = ReservationStation Size\n";
    cout << "--fu = FunctionalUnit Count --rnt = RenameTable Size --rob = ReorderBuffer Size\n";
    cout << "--inshr = Level 1 Instruction Cache Hit Rate --l1hr = Level 1 Hit Rate --l1at = Level 1 Access Time\n";
    cout << "--l2hr = Level 2 Hit Rate --l2at = Level 2 Access Time --mmat = Main Memory Access Time\n";
    cout << "--trace = Path to Target Trace File\n";
    cout << "All parameters ARE required. \n";
    cout << "See included test.sh for usage examples\n";
}

//Specifying the expected options
//All options require an argument per the project instructions
static struct option long_options[] = {
	{"ss",      required_argument, 0,  's' },
	{"btb", 	required_argument, 0,  'b' },
	{"rs",    	required_argument, 0,  'r' },
	{"fu",   	required_argument, 0,  'f' },
	{"rnt",   	required_argument, 0,  'n' },
	{"rob",   	required_argument, 0,  'o' },
	{"inshr",  	required_argument, 0,  'i' },
	{"l1hr",  	required_argument, 0,  '1' },
	{"l1at",   	required_argument, 0,  '!' },
	{"l2hr",   	required_argument, 0,  '2' },
	{"l2at",   	required_argument, 0,  '@' },
	{"mmat",    required_argument, 0,  'm' },
	{"trace",   required_argument, 0,  't' },
	{0,         0,                 0,  0   }
};

void promptForInt(std::string promptTextString, int &targetInt, int max) {
	cout <<  promptTextString;
	cin >> targetInt;

	if(max != 0 && targetInt > max)
	    targetInt = max;
}

void promptForFloat(std::string promptTextString, float &targetFloat, float max) {
	cout <<  promptTextString;
	cin >> targetFloat;

	if(max != 0 && targetFloat > max)
	    targetFloat = max;
}

void promptForString(std::string promptTextString, std::string &targetStr) {
	cout <<  promptTextString;
	cin >> targetStr;
}



int processCommandLine(int argc, char **argv) {
	int opt = 0;
	int long_index = 0;
	int foundCount = 0;
	int targetFound = 12;

	DEBUG_COUT << "Processing command line for " << argc << " options\n";

	while ((opt = getopt_long_only(argc, argv,"s:b:r:f:n:1:a:2:c:t:",
				   long_options, &long_index )) != -1) {
//		cout << "Found opt=%d\n", opt;
		switch (opt) {
			 case 's' :
				 DEBUG_COUT << "Found s\n";
				 ::superScalarFactor = atoi(optarg);
				 foundCount++;
				 break;
			 case 'b' :
				 DEBUG_COUT << "Found b\n";
				 ::btbSize = atoi(optarg);
				 foundCount++;
				 break;
			 case 'r' :
				 DEBUG_COUT << "Found r\n";
				 ::rsEntries = atoi(optarg);
				 foundCount++;
				 break;
			 case 'f' :
				 DEBUG_COUT << "Found f\n";
				 ::fuCount = atoi(optarg);
				 foundCount++;
				 break;
			 case 'n' :
				 DEBUG_COUT << "Found n\n";
				 ::renameTableEntries = atoi(optarg);
				 foundCount++;
				 break;
			 case 'o' :
				 DEBUG_COUT << "Found o\n";
				 ::reorderBufferEntries = atoi(optarg);
				 foundCount++;
				 break;
			 case 'i' :
				 DEBUG_COUT << "Found i\n";
				 ::instrCacheHitRate = atoi(optarg);
				 foundCount++;
				 break;
			 case '1' :
				 DEBUG_COUT << "Found 1\n";
				 ::level1CacheHitRate = atoi(optarg);
				 foundCount++;
				 break;
			 case '!' :
				 DEBUG_COUT << "Found !\n";
				 ::level1CacheAccessTime = atoi(optarg);
				 foundCount++;
				 break;
			 case '2' :
				 DEBUG_COUT << "Found 2\n";
				 ::level2CacheHitRate = atoi(optarg);
				 foundCount++;
				 break;
			 case '@' :
				 DEBUG_COUT << "Found @\n";
				 ::level2CacheAccessTime = atoi(optarg);
				 foundCount++;
				 break;
            case 'm' :
                 DEBUG_COUT << "Found m\n";
                 ::systemMemoryAccessTime = atoi(optarg);
                 foundCount++;
                 break;
			 case 't' :
			 {
				 DEBUG_COUT << "Found t (" << optarg << ") size: " << sizeof(&optarg);
//				 inputTrace = optarg;
//				 memcpy(::inputTrace, optarg, sizeof(&optarg));
//				 char *inFile;
//				 inFile = strdup(optarg);
				 std::string tempString(optarg);
				 ::inputTraceFile = tempString;
				 DEBUG_COUT << "EX: " << ::inputTraceFile << endl;
			}
				 foundCount++;
				 break;
			 default:
				 DEBUG_COUT << "Found default\n";
				 break;
//				 print_usage();
//				 goto end_loop;
		}

	}

	if(opt == -1 && foundCount != targetFound) {
		DEBUG_COUT << "Found opt=" << opt << "\t" << foundCount << " of " << targetFound << " required flags found\n";
//		print_usage();
		if(::superScalarFactor == -1)
			promptForInt("Enter Superscalar Width (max: 8):\t", ::superScalarFactor, 8);
		if(::btbSize == -1)
			promptForInt("Enter BTB Size (max: 128):\t", ::btbSize, 128);
		if(::rsEntries == -1)
			promptForInt("Enter # of RS Entries (max: 128):\t", ::rsEntries, 128);
		if(::fuCount == -1)
			promptForInt("Enter # of Functional Units (max: 8):\t", ::fuCount, 8);
		if(::renameTableEntries == -1)
			promptForInt("Enter Rename Table size (max 128):\t", ::renameTableEntries, 128);
		if(::reorderBufferEntries == -1)
			promptForInt("Enter ROB size (max 128):\t", ::reorderBufferEntries, 128);

		if(::instrCacheHitRate == -1)
			promptForFloat("Enter Instruction Cache Hit Rate (max 100):\t", ::instrCacheHitRate, 100);
//		if(::instrCacheAccessTime == -1)
//			promptForInt("Enter Instruction Cache Access Time:\t", ::instrCacheAccessTime);

		if(::level1CacheHitRate == -1)
			promptForFloat("Enter L1 Cache Hit Rate (max 100):\t", ::level1CacheHitRate, 100);
		if(::level1CacheAccessTime == -1)
			promptForInt("Enter L1 Cache Access Time:\t", ::level1CacheAccessTime, 0);

		if(::level2CacheHitRate == -1)
			promptForFloat("Enter L2 Cache Hit Rate (max 100):\t", ::level2CacheHitRate, 100);
		if(::level2CacheAccessTime == -1)
			promptForInt("Enter L2 Cache Access Time:\t", ::level2CacheAccessTime, 0);

		if(::systemMemoryAccessTime == -1)
		    promptForInt("Enter Main Memory Access Time:\t", ::systemMemoryAccessTime, 0);

		if(::inputTraceFile.size() == 0)
		    promptForString("Enter Path and Trace File Name (ex: ./traces/applu.tra):\t", ::inputTraceFile);

        ::instrCacheAccessTime = ::level1CacheAccessTime;
	}

	//check for any remaining vars now and prompt for them

	return 0; //return 0 for success, <0 for failure. handle that on the calling side.
}
