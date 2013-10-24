/*
 * CommandLine.c
 *
 *  Created on: Oct 22, 2013
 *      Author: brian
 */

#include "CommandLine.h"
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>

void print_usage() {
    printf("Usage: project2 --ss NUM --btb NUM --rs NUM \n");
    printf("                 --fu NUM --rnt NUM --rob NUM \n");
    printf("                 --l1hr NUM --l1at NUM --l2hr NUM \n");
    printf("                 --l2at NUM --trace TRACE_FILE\n");
    printf("Key:\n")
    printf("--ss = SuperScalar Count --btb = BTB Size --rs = ReservationStation Size");
    printf("--fu = FunctionalUnit Count --rnt = RenameTable Size --rob = ReorderBuffer Size \n");
    printf("--l1hr = Level 1 Hit Rate --l1at = Level 1 Access Time --l2hr = Level 2 Hit Rate \n");
    printf("--l2at = Level 2 Access Time --trace = Path to Target Trace File\n");
    printf("All parameters ARE required. \n");
    printf("See included test.sh for usage examples\n");
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
	{"l1hr",  	required_argument, 0,  '1' },
	{"l1at",   	required_argument, 0,  'a' },
	{"l2hr",   	required_argument, 0,  '2' },
	{"l2at",   	required_argument, 0,  'c' },
	{"trace",   required_argument, 0,  't' },
	{0,         0,                 0,  0   }
};


int processCommandLine(int argc, char **argv,
						int *superScalarFactor,
						int *btbSize,
						int *rsEntries,
						int *fuCount,
						int *renameTableEntries,
						int *reorderBufferEntries,
						float *level1CacheHitRate,
						int *level1CacheAccessTime,
						float *level2CacheHitRate,
						int *level2CacheAccessTime,
						char *inputTrace) {
	int opt = 0;
	int long_index = 0;

	printf("Processing command line for %d options\n", argc);

	while ((opt = getopt_long_only(argc, argv,"s:b:r:f:n:1:a:2:c:t:",
				   long_options, &long_index )) != -1) {
//		printf("Found opt=%d\n", opt);
		switch (opt) {
			 case 's' :
				 printf("Found s\n");
				 *superScalarFactor = atoi(optarg);
				 break;
			 case 'b' :
				 printf("Found b\n");
				 *btbSize = atoi(optarg);
				 break;
			 case 'r' :
				 printf("Found r\n");
				 *rsEntries = atoi(optarg);
				 break;
			 case 'f' :
				 printf("Found f\n");
				 *fuCount = atoi(optarg);
				 break;
			 case 'n' :
				 printf("Found n\n");
				 *renameTableEntries = atoi(optarg);
				 break;
			 case 'o' :
				 printf("Found o\n");
				 *reorderBufferEntries = atoi(optarg);
				 break;
			 case '1' :
				 printf("Found 1\n");
				 *level1CacheHitRate = atoi(optarg);
				 break;
			 case 'a' :
				 printf("Found a\n");
				 *level1CacheAccessTime = atoi(optarg);
				 break;
			 case '2' :
				 printf("Found 2\n");
				 *level2CacheHitRate = atoi(optarg);
				 break;
			 case 'c' :
				 printf("Found c\n");
				 *level2CacheAccessTime = atoi(optarg);
				 break;
			 case 't' :
				 printf("Found t\n");
				 inputTrace = optarg;
				 break;
			 default:
				 printf("Found default\n");
				 print_usage();
				 exit(EXIT_FAILURE);
		}
	}

	if(opt == -1) {
		printf("Found opt=%d\n", opt);
		print_usage();
		exit(EXIT_FAILURE);
	}

	return 0; //return 0 for success, <0 for failure. handle that on the calling side.
}
