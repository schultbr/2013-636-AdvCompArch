/*
 * GlobalVars.cpp
 *
 *  Created on: Oct 26, 2013
 *      Author: brian
 */
#include "GlobalVars.h"
#include <vector>

//////////////////////////////////
//program options:
//////////////////////////////////
int superScalarFactor = -1;
int btbSize = -1;
int rsEntries = -1;
int fuCount = -1;
int renameTableEntries = -1;
int reorderBufferEntries = -1;
float instrCacheHitRate = -1;
int instrCacheAccessTime = -1;
float level1CacheHitRate = -1;
int level1CacheAccessTime = -1;
float level2CacheHitRate = -1;
int level2CacheAccessTime = -1;
int systemMemoryAccessTime = -1; //todo: Add in command line option for this
std::string inputTraceFile;

//////////////////////////////////
//hardware elements:
//////////////////////////////////
std::vector<ARF_Element> registers;
std::vector<ROB_Element> reOrderBuffer;
std::vector<RRF_Element> renameRegisterFile;

int regHILO = 0;
int regFCC = 0;

BranchPredictor branchPredictor;

//////////////////////////////////
//statistical counters
//////////////////////////////////
int cyclesCompleted = 0;
int instructionCount = 0;

//////////////////////////////////
//system flags (for coordinating
//              pipeline stages)
//////////////////////////////////
int fetchStalledInstrPC = 0;
bool fetchStalled = false;
