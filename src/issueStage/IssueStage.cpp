/*
 * ExecuteStage.cpp
 *
 *  Created on: Oct 29, 2013
 *      Author: jason cain
 */

#include "IssueStage.h"
#include "GlobalVars.h"
#include "StructureDefs.h"
#include <unistd.h>
#include <stdio.h>
#include <iostream>
#include <stdlib.h>

using namespace std;

void checkValue( std::vector<RS_Element> *targetRS )
{
	int rename_tag = 0;

	for(size_t i = 0; i < targetRS->size(); i++) 
	{
		if(targetRS->at(i).valid1 == 0)				//check if op1 is valid
		{	
			rename_tag = targetRS->at(i).op1;	//set rrf tag
			if(rrf[rename_tag].valid == 1)		//if rrf is valid, copy data
			{
    				targetRS->at(i).op1 = rrf[rename_tag].data;
    				targetRS->at(i).valid1 = 1;
			}
		}
		if(targetRS->at(i).valid2 == 0)		//check for valid op2 value
		{
			rename_tag = targetRS->at(i).op2;
			if(rrf[rename_tag].valid == 1)
			{
    				targetRS->at(i).op2 = rrf[rename_tag].data;
    				targetRS->at(i).valid2 = 1;
			}
		}
		if(targetRS->at(i).valid1 == 1 && targetRS->at(i).valid2 == 1)	//set ready bit
			targetRS->at(i).ready = 1;
	}
}

void checkReady( std::vector<RS_Element> *targetRS )
{
	int FU_tag;
	for(size_t i = 0; i < targetRS->size(); i++)
	{
		if(targetRS->at(i).ready == 1)
		{
			//FU_tag = checkFU( //check for free FU
			//copy values to FU
			//set counter or busy bit in FU
			//set busy bit in RS to 0
		}
	}
}

int checkFU( std::vector<FU_Element> *targetFU )
{
	int FU_tag = -1;				//default = no empty slots
	for(size_t i = 0; i < targetFU->size(); i++)	//iterate through Functional Units
	{
		if(targetFU->at(i).count == 0)		//if not busy
			FU_tag = i;
	}
	return FU_tag;
}
	

void simulateIssueStage()
{
	int rename_tag = 0;
	unsigned i;

	cout << "Issue Stage\n";
	
	//check Commom Data Bus for updates
	checkValue( &rs_int );
	checkValue( &rs_fp );
	checkValue( &rs_mem );
	checkValue( &rs_br );
	
	//checkReady( rs_int, fu_add, fu_mult );

}	


