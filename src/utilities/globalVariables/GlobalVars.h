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
extern std::string inputTraceFile;

extern BranchPredictor branchPredictor;


extern std::vector<ARF_Element> registers;
extern std::vector<ROB_Element> reOrderBuffer;
extern std::vector<RRF_Element> renameRegisterFile;
extern std::vector<RRF_Element> renameRegisterFile;



extern int regHILO;
extern int regFCC;

extern int cyclesCompleted;
extern int instructionCount;

extern int fetchStalledInstrPC;
extern bool fetchStalled;


//
//void setSuperScalarSize(int size);
//void setInstructionTrace(char *file);

#endif /* GLOBALVARS_H_ */
