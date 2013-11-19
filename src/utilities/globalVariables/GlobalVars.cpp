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
float instrCacheHitRate = -1;		//should these be floats or int?
int instrCacheAccessTime = -1;
float level1CacheHitRate = -1;		//should these be floats or int?
int level1CacheAccessTime = -1;
float level2CacheHitRate = -1;		//should these be floats or int?
int level2CacheAccessTime = -1;
int systemMemoryAccessTime = -1;
std::string inputTraceFile;

//////////////////////////////////
//hardware elements:
//////////////////////////////////
int robHead = 0;
int robTail = 0;
int robEntries = 0;
int robEntriesMax = 0;

std::vector<ARF_Element> arf;
std::vector<ROB_Element> rob;
std::vector<RRF_Element> rrf;	

std::vector<RS_Element> rs_int;
std::vector<RS_Element> rs_fp;
std::vector<RS_Element> rs_mem;
std::vector<RS_Element> rs_br;
std::vector<FU_Element> fu_add;
std::vector<FU_Element> fu_mult;
std::vector<FU_Element> fu_fp;
std::vector<FU_Element> fu_mem;
FU_Element fu_br;			//single Functional Unit for branch

BranchPredictor branchPredictor;

bool anyUnresolvedBranches = false;
int unresolvedBranchRobIndex = 0;

//////////////////////////////////
//statistical counters
//////////////////////////////////
unsigned int cyclesCompleted = 0;
unsigned int instructionCount = 0;

unsigned int rs_int_inUse = 0;
unsigned int rs_fp_inUse = 0;
unsigned int rs_br_inUse = 0;
unsigned int rs_mem_inUse = 0;
unsigned int fu_add_inUse = 0;
unsigned int fu_mult_inUse = 0;
unsigned int fu_fp_inUse = 0;
unsigned int fu_br_inUse = 0;
unsigned int fu_mem_inUse = 0;
unsigned int rrf_inUse = 0;
unsigned int rob_inUse = 0;

unsigned int rs_int_total = 0;
unsigned int rs_fp_total = 0;
unsigned int rs_br_total = 0;
unsigned int rs_mem_total = 0;
unsigned int fu_add_total = 0;
unsigned int fu_mult_total = 0;
unsigned int fu_fp_total = 0;
unsigned int fu_br_total = 0;
unsigned int fu_mem_total = 0;
unsigned int rrf_total = 0;
unsigned int rob_total = 0;

//////////////////////////////////
//system flags (for coordinating
//              pipeline stages)
//////////////////////////////////
int fetchStalledInstrPC = 0;
bool fetchStalled = false;
bool endOfTraceReached = false;

bool isFetchFinished = false;
bool isDecodeFinished = false;
bool isDispatchFinished = false;
bool isIssueFinished = false;
bool isExecuteFinished = false;
bool isCompleteFinished = false;
