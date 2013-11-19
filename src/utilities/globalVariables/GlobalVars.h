/*
 * GlobalVars.h
 *
 *  Created on: Oct 26, 2013
 *      Author: brian
 */

#ifndef GLOBALVARS_H_
#define GLOBALVARS_H_

//#define DEBUG //for testing prints. Comment this line out to slim down the output.

#ifdef DEBUG
#define DEBUG_COUT cout
#else
#define DEBUG_COUT 0 && cout
#endif


#define DEBUG2 //for testing prints. Comment this line out to slim down the output.

#ifdef DEBUG2
#define DEBUG_COUT_2 cout
#else
#define DEBUG_COUT_2 0 && cout
#endif

//#include "TraceReader.h"
#include "StructureDefs.h"
#include "BranchPredictor.h"
#include <string>
#include <vector>

//class BranchPredictor; //forward declaration of branch predictor

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
extern std::vector<FU_Element> fu_add;
extern std::vector<FU_Element> fu_mult;
extern std::vector<FU_Element> fu_fp;
extern std::vector<FU_Element> fu_mem;
extern FU_Element fu_br;			//single Functional Unit

extern bool anyUnresolvedBranches;
extern int unresolvedBranchRobIndex;


//////////////////////////////////////////////////////////
//
//          Statistical Variables
//
//////////////////////////////////////////////////////////
extern unsigned int cyclesCompleted;
extern unsigned int instructionCount;

//branch info is tracked by the predictor itself..

//////////////////////////////////////////////////////////
//
//         Inter-stage communications
//
//////////////////////////////////////////////////////////
extern int fetchStalledInstrPC;
extern bool fetchStalled;
extern bool endOfTraceReached;

extern bool isFetchFinished;
extern bool isDecodeFinished;
extern bool isDispatchFinished;
extern bool isIssueFinished;
extern bool isExecuteFinished;
extern bool isCompleteFinished;


#endif /* GLOBALVARS_H_ */
