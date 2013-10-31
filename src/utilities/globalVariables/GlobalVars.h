/*
 * GlobalVars.h
 *
 *  Created on: Oct 26, 2013
 *      Author: brian
 */

#ifndef GLOBALVARS_H_
#define GLOBALVARS_H_

//#include "TraceReader.h"
#include <vector>

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
extern char inputTrace[50];


extern std::vector<int> registers;
extern std::vector<int> fpRegisters;
extern int regHILO;
extern int regFCC;


//
//void setSuperScalarSize(int size);
//void setInstructionTrace(char *file);

#endif /* GLOBALVARS_H_ */
