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

void simulateCompleteStage()
{
	int dest_tag, rename_tag;
	int done = 0;

	DEBUG_COUT << "inside Complete Stage" << endl;
	
	while(!done)
	{
		if (rob[robHead].busy == 1 && rob[robHead].finished == 1 && rob[robHead].valid == 1)
		{
			done = 0;
			rob[robHead].busy = 0;			            //set not busy
			rename_tag = rob[robHead].rename;

			if (rename_tag != -1);				        //has a destination register
			{
				dest_tag = rrf[rename_tag].dest;
				arf[dest_tag].data = rrf[rename_tag].data;	//copy data from RRF to ARF
				rrf[rename_tag].busy = 0;			    //set not busy

				if (arf[dest_tag].rename == rename_tag)		//data being written to ARF is newest value
					arf[dest_tag].busy = 0;			        //set not busy
			}

			if (robHead == rob.size()-1)				    //increment head of circular queue
				robHead = 0;
			else
				robHead++;
		}
		else				//do we do anything special for finished but not valid?
		{
			done = 1;		//next element was: not busy(hit tail), not finished, or not valid(wait for br to resolve)
		}
	}
}

