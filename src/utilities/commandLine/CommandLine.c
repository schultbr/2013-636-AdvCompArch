/*
 * CommandLine.c
 *
 *  Created on: Oct 22, 2013
 *      Author: brian
 */

#include "CommandLine.h"


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
						int *level1CacheAccessTime,
						char *inputTrace)
{
	int opt = 0;
	while ((opt = getopt (argc, argv, "abc:")) != -1) {
		switch (opt)
		{

		}
	}

	return 1; //return 1 for success, 0 or <0 for failure. handle that on the calling side.
}
