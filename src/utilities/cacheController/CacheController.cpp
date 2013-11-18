/*
 * CacheController.cpp
 *
 *  Created on: Nov 3, 2013
 *      Author: brian
 */
#include "CacheController.h"
#include <unistd.h>
#include <stdio.h>
#include <iostream>
#include <stdlib.h>
#include "GlobalVars.h"

using namespace std;
int checkCache(float cacheHitRate, int cacheAccessTime) {
    DEBUG_COUT << "CacheCheck:\t Checking for " << cacheHitRate << "% with a penalty of " << cacheAccessTime << endl;
    int penaltyTime = 0;
    int cacheHitRoll = 0;

    cacheHitRoll = rand() % 100;

    DEBUG_COUT << "CacheCheck:\t Comparing roll " << cacheHitRoll << " with hit rate " << cacheHitRate << " (" << (cacheHitRoll > cacheHitRate ? "miss" : "hit") << ")\n";

    if (cacheHitRoll > (int)cacheHitRate) {
        penaltyTime += cacheAccessTime;
        DEBUG_COUT << "CacheCheck:\t Added " << cacheAccessTime << " to our penalty time (now " << penaltyTime << ")\n";
    }

    return penaltyTime;
}
