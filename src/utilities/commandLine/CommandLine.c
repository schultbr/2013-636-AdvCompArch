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
    printf("All parameters ARE required.");
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

	while ((opt = getopt_long_only(argc, argv,"",
				   long_options, &long_index )) != -1) {
		switch (opt) {
			 case 's' : *superScalarFactor = atoi(optarg);
				 break;
			 case 'b' : *btbSize = atoi(optarg);
				 break;
			 case 'r' : *rsEntries = atoi(optarg);
				 break;
			 case 'f' : *fuCount = atoi(optarg);
				 break;
			 case 'n' : *renameTableEntries = atoi(optarg);
				 break;
			 case 'o' : *reorderBufferEntries = atoi(optarg);
				 break;
			 case '1' : *level1CacheHitRate = atoi(optarg);
				 break;
			 case 'a' : *level1CacheAccessTime = atoi(optarg);
				 break;
			 case '2' : *level2CacheHitRate = atoi(optarg);
				 break;
			 case 'c' : *level2CacheAccessTime = atoi(optarg);
				 break;
			 case 't' : inputTrace = optarg;
				 break;
			 default: print_usage();
				 exit(EXIT_FAILURE);
		}
	}

	return 0; //return 0 for success, <0 for failure. handle that on the calling side.
}
