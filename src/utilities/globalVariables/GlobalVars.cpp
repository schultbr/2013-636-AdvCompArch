/*
 * GlobalVars.cpp
 *
 *  Created on: Oct 26, 2013
 *      Author: brian
 */
#include "GlobalVars.h"
#include <vector>


int returnVal = -1;
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
char inputTrace[50];

std::vector<int> registers;
std::vector<int> fpRegisters;
int regHILO = 0;
int regFCC = 0;


////these setters are totally unnecessary but, meh. theyre already here.
////kinda the right thing to do... sometimes
//void setSuperScalarSize(int size) {
//	superScalarSize = size;
//}
//
//void setInstructionTrace(char *file) {
//	instructionTrace.openTrace(file);
//}
