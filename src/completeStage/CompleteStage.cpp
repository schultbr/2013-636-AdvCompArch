/*
 * CompleteStage.cpp
 *
 *  Created on: Oct 29, 2013
 *      Author: jason cain
 */

#include "CompleteStage.h"
#include "GlobalVars.h"
#include <unistd.h>
#include <stdio.h>
#include <iostream>
#include <stdlib.h>

using namespace std;

void simulateCompleteStage(); 
{
	int dest, rename, index;
	int done = 0;

	cout << "Completing " << " instructions\n";
	
	while(!done)
	{
		if (rob[robHead].busy == 1 && rob[robHead].finished == 1 && rob[robHead].valid == 1 &&)
		{
			done 			= 0;
			rob[robHead].busy 	= 0;			//set not busy
			rename 			= rob[robHead].rename;

			if (rename != -1);					//has a destination register
			{
				dest 			= rrf[ rename].dest;
				arf[dest]		= rrf[rename].data;	//copy data from RRF to ARF
				rrf[rename].busy 	= 0;			//set not busy

				if (arf[dest].rename == rename)			//data being written to ARF is newest value
					arf[dest].busy = 0;			//set not busy
			}

			if (robHead = rob.size()-1)				//increment head of circular queue
				robHead = 0;
			else
				robHead += 1;
		}
		else				//do we do anything special for finished but not valid?
		{
			done = 1;		//next element was: not busy(hit tail), not finished, or not valid(wait for br to resolve)
		}
	}
}


