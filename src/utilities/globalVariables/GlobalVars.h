/*
 * GlobalVars.h
 *
 *  Created on: Oct 26, 2013
 *      Author: brian
 */

#ifndef GLOBALVARS_H_
#define GLOBALVARS_H_

//#include "TraceReader.h"
#include "StructureDefs.h"
//#include "BranchPredictor.h"
#include <string>
#include <vector>

class BranchPredictor; //forward declaration of branch predictor

//////////////////////////////////////////////////////////
//
//              User Options
//
//////////////////////////////////////////////////////////
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
extern int systemMemoryAccessTime;
extern std::string inputTraceFile;

//////////////////////////////////////////////////////////
//
//          Architecture Elements
//
//////////////////////////////////////////////////////////
extern int robHead;
extern int robTail;

extern BranchPredictor branchPredictor;

extern std::vector<ARF_Element> arf;		//registers;
extern std::vector<ROB_Element> rob;		//reOrderBuffer;
extern std::vector<RRF_Element> rrf;		//renameRegisterFile;
extern std::vector<RS_Element> rs_int;
extern std::vector<RS_Element> rs_fp;
extern std::vector<RS_Element> rs_mem;
extern std::vector<RS_Element> rs_br;
extern std::vector<RS_Element> fu_add;
extern std::vector<RS_Element> fu_mult;
extern std::vector<RS_Element> fu_fp;
extern std::vector<RS_Element> fu_mem;
extern RS_Element fu_branch;			//single Functional Unit
extern int regHILO;
extern int regFCC;

extern BranchPredictor branchPredictor;

//////////////////////////////////
//statistical counters
//////////////////////////////////

//////////////////////////////////////////////////////////
//
//          Statistical Variables
//
//////////////////////////////////////////////////////////
extern unsigned int cyclesCompleted;
extern unsigned int instructionCount;

extern unsigned int branchPredictionCount;


//////////////////////////////////////////////////////////
//         Inter-stage communications
//
//////////////////////////////////////////////////////////
extern int fetchStalledInstrPC;
extern bool fetchStalled;


#endif /* GLOBALVARS_H_ */
