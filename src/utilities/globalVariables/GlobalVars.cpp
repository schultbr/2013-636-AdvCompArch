/*
 * GlobalVars.cpp
 *
 *  Created on: Oct 26, 2013
 *      Author: brian
 */
#include "GlobalVars.h"
#include "BranchPredictor.h"
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
int systemMemoryAccessTime = -1;
std::string inputTraceFile;

//////////////////////////////////
//hardware elements:
//////////////////////////////////
int robHead = -1;
int robTail = -1;

std::vector<ARF_Element> arf;
std::vector<ROB_Element> rob;
std::vector<RRF_Element> rrf;	

std::vector<RS_Element> rs_int;
std::vector<RS_Element> rs_fp;
std::vector<RS_Element> rs_mem;
std::vector<RS_Element> rs_br;
std::vector<RS_Element> fu_add;
std::vector<RS_Element> fu_mult;
std::vector<RS_Element> fu_fp;
std::vector<RS_Element> fu_mem;
RS_Element fu_branch;			//single Functional Unit for branch

BranchPredictor branchPredictor;

//////////////////////////////////
//statistical counters
//////////////////////////////////
unsigned int cyclesCompleted = 0;
unsigned int instructionCount = 0;


//////////////////////////////////
//system flags (for coordinating
//              pipeline stages)
//////////////////////////////////
int fetchStalledInstrPC = 0;
bool fetchStalled = false;
