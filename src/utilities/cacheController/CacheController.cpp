/*
 * CacheController.cpp
 *
 *  Created on: Nov 3, 2013
 *      Author: brian
 */
#include "CacheController.h"
#include <unistd.h>
#include <stdlib.h>

//using namespace std;
int checkCache(float cacheHitRate, int cacheAccessTime)
{
	int penaltyTime = 0;
	int cacheHitRoll = 0;

	cacheHitRoll = rand() % 100;

	if(cacheHitRoll > cacheHitRate)
	{
		penaltyTime += cacheAccessTime;
	}

	return penaltyTime;

}
