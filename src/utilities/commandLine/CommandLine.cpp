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

static int maxSSF = 8;
static int maxBTB = 128;
static int maxRS = 8;
static int maxFU = 8;
static int maxRRF = 128;
static int maxROB = 128;
static float maxHR = 100;

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
static struct option long_options[] = { { "ss", required_argument, 0, 's' },
                                        { "btb", required_argument, 0, 'b' },
                                        { "rs", required_argument, 0, 'r' },
                                        { "fu", required_argument, 0, 'f' },
                                        { "rnt", required_argument, 0, 'n' },
                                        { "rob", required_argument, 0, 'o' },
                                        { "inshr", required_argument, 0, 'i' },
                                        { "l1hr", required_argument, 0, '1' },
                                        { "l1at", required_argument, 0, '!' },
                                        {"l2hr", required_argument, 0, '2' },
                                        { "l2at", required_argument, 0, '@' },
                                        { "mmat", required_argument, 0, 'm' },
                                        { "trace", required_argument, 0, 't' },
                                        { "help", required_argument, 0, 'h' },
                                        { "help", required_argument, 0, '?' },
                                        { 0, 0, 0, 0 } };

void promptForInt(std::string promptTextString, int &targetInt, int max) {
    cout << promptTextString;
    cin >> targetInt;

    if (max != 0 && targetInt > max)
        targetInt = max;
}

void promptForFloat(std::string promptTextString, float &targetFloat, float max) {
    cout << promptTextString;
    cin >> targetFloat;

    if (max != 0 && targetFloat > max)
        targetFloat = max;
}

void promptForString(std::string promptTextString, std::string &targetStr) {
    cout << promptTextString;
    cin >> targetStr;
}

int processCommandLine(int argc, char **argv) {
    int opt = 0;
    int long_index = 0;
    int foundCount = 0;
    int targetFound = 13;

    DEBUG_COUT("Processing command line for " << argc << " options\n");

    while ((opt = getopt_long_only(argc, argv, "s:b:r:f:n:1:a:2:c:t:", long_options, &long_index)) != -1) {
        switch (opt) {
            case 's':
                ::superScalarFactor = atoi(optarg);
                if(::superScalarFactor > maxSSF)
                    ::superScalarFactor = maxSSF;
                foundCount++;
                break;
            case 'b':
                ::btbSize = atoi(optarg);
                if(::btbSize > maxBTB)
                    ::btbSize = maxBTB;
                foundCount++;
                break;
            case 'r':
                ::rsEntries = atoi(optarg);
                if(::rsEntries > maxRS)
                    ::rsEntries = maxRS;
                foundCount++;
                break;
            case 'f':
                ::fuCount = atoi(optarg);
                if(::fuCount > maxFU)
                    ::fuCount = maxFU;
                foundCount++;
                break;
            case 'n':
                ::renameTableEntries = atoi(optarg);
                if(::renameTableEntries > maxRRF)
                    ::renameTableEntries = maxRRF;
                foundCount++;
                break;
            case 'o':
                ::reorderBufferEntries = atoi(optarg);
                if(::reorderBufferEntries > maxROB)
                    ::reorderBufferEntries = maxROB;
                foundCount++;
                break;
            case 'i':
                ::instrCacheHitRate = atoi(optarg);
                if(::instrCacheHitRate > maxHR)
                    ::instrCacheHitRate = maxHR;
                else if(::instrCacheHitRate < 1 )
                    ::instrCacheHitRate = 1;
                foundCount++;
                break;
            case '1':
                ::level1CacheHitRate = atoi(optarg);
                if(::level1CacheHitRate > maxHR)
                    ::level1CacheHitRate = maxHR;
                else if(::level1CacheHitRate < 1 )
                    ::level1CacheHitRate = 1;
                foundCount++;
                break;
            case '!':
                ::level1CacheAccessTime = atoi(optarg);
                if(::level1CacheAccessTime <= 0)
                    ::level1CacheAccessTime = 1;
                foundCount++;
                break;
            case '2':
                ::level2CacheHitRate = atoi(optarg);
                if(::level2CacheHitRate > maxHR)
                    ::level2CacheHitRate = maxHR;
                else if(::level2CacheHitRate < 1 )
                    ::level2CacheHitRate = 1;
                foundCount++;
                break;
            case '@':
                ::level2CacheAccessTime = atoi(optarg);
                if(::level2CacheAccessTime <= 0)
                    ::level2CacheAccessTime = 1;
                foundCount++;
                break;
            case 'm':
                ::systemMemoryAccessTime = atoi(optarg);
                if(::systemMemoryAccessTime <= 0)
                    ::systemMemoryAccessTime = 1;
                foundCount++;
                break;
            case 't': {
                std::string tempString(optarg);
                ::inputTraceFile = tempString;
                foundCount++;
                break;
            }
            case 'h':
            case '?':
            default:
                print_usage();
                return -1;
                break; //this is funny..
        }

    }

    //check for any remaining vars now and prompt for them
    if (opt == -1 && foundCount != targetFound) {
        if (::superScalarFactor == -1)
            promptForInt("Enter Superscalar Width (max: 8):\t", ::superScalarFactor, 8);
        if (::btbSize == -1)
            promptForInt("Enter BTB Size (max: 128):\t", ::btbSize, 128);
        if (::rsEntries == -1)
            promptForInt("Enter # of RS Entries (max: 8):\t", ::rsEntries, 8);
        if (::fuCount == -1)
            promptForInt("Enter # of Functional Units (max: 8):\t", ::fuCount, 8);
        if (::renameTableEntries == -1)
            promptForInt("Enter Rename Table size (max 128):\t", ::renameTableEntries, 128);
        if (::reorderBufferEntries == -1)
            promptForInt("Enter ROB size (max 128):\t", ::reorderBufferEntries, 128);

        if (::instrCacheHitRate == -1)
            promptForFloat("Enter Instruction Cache Hit Rate (max 100):\t", ::instrCacheHitRate, 100);

        if (::level1CacheHitRate == -1)
            promptForFloat("Enter L1 Cache Hit Rate (max 100):\t", ::level1CacheHitRate, 100);
        if (::level1CacheAccessTime == -1)
            promptForInt("Enter L1 Cache Access Time:\t", ::level1CacheAccessTime, 0);

        if (::level2CacheHitRate == -1)
            promptForFloat("Enter L2 Cache Hit Rate (max 100):\t", ::level2CacheHitRate, 100);
        if (::level2CacheAccessTime == -1)
            promptForInt("Enter L2 Cache Access Time:\t", ::level2CacheAccessTime, 0);

        if (::systemMemoryAccessTime == -1)
            promptForInt("Enter Main Memory Access Time:\t", ::systemMemoryAccessTime, 0);

        if (::inputTraceFile.size() == 0)
            promptForString("Enter Path and Trace File Name (ex: ./traces/applu.tra):\t", ::inputTraceFile);
    }

    return 0; //return 0 for success, <0 for failure. handle that on the calling side.
}
