/*
 * GlobalVars.h
 *
 *  Created on: Oct 26, 2013
 *      Author: brian
 */

#ifndef GLOBALVARS_H_
#define GLOBALVARS_H_

//#include "TraceReader.h"
#include <string>
#include <vector>
#include "BranchPredictor.h"
#include "StructureDefs.h"


//////////////////////////////////
//program options:
//////////////////////////////////
extern int superScalarFactor;
extern int btbSize;
extern int rsEntries;
extern int fuCount;
extern int renameTableEntries;
extern int reorderBufferEntries;
extern float instrCacheHitRate;
extern int instrCacheAccessTime;
extern float level1CacheHitRate;
extern int level1CacheAccessTime;
extern float level2CacheHitRate;
extern int level2CacheAccessTime;
extern int systemMemoryAccessTime; //todo: Add in command line option for this
extern std::string inputTraceFile;


//////////////////////////////////
//hardware elements:
//////////////////////////////////
extern std::vector<ARF_Element> registers;
extern std::vector<ROB_Element> reOrderBuffer;
extern std::vector<RRF_Element> renameRegisterFile;
//extern std::vector<RRF_Element> renameRegisterFile;

extern int regHILO;
extern int regFCC;

extern BranchPredictor branchPredictor;

//////////////////////////////////
//statistical counters
//////////////////////////////////
extern int cyclesCompleted;
extern int instructionCount;

//////////////////////////////////
//system flags (for coordinating
//              pipeline stages)
//////////////////////////////////
extern int fetchStalledInstrPC;
extern bool fetchStalled;

#endif /* GLOBALVARS_H_ */
