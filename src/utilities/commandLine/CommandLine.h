/*
 * CommandLine.h
 *
 *  Created on: Oct 22, 2013
 *      Author: brian
 */

#ifndef COMMANDLINE_H_
#define COMMANDLINE_H_

int processCommandLine(int argc, char **argv,
						int &superScalarFactor,
						int &btbSize,
						int &rsEntries,
						int &fuCount,
						int &renameTableEntries,
						int &reorderBufferEntries,
						float &level1CacheHitRate,
						int &level1CacheAccessTime,
						float &level2CacheHitRate,
						int &level2CacheAccessTime,
						char *inputTrace);

#endif /* COMMANDLINE_H_ */
