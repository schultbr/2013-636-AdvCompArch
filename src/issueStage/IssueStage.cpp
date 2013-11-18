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

void copyToFU(RS_Element entry, std::vector<FU_Element> &targetFU, int index, int cnt) {
    targetFU[index].op1 = entry.op1;
    targetFU[index].op2 = entry.op2;
    targetFU[index].reorder = entry.reorder;
    targetFU[index].code = entry.code;
    targetFU[index].PC = entry.PC;
    targetFU[index].count = cnt;
    targetFU[index].isFirstClock = true;

}

void copyToBranchFU(RS_Element entry, FU_Element &targetFU) {	//single element FU, not a vector
    targetFU.op1 = entry.op1;
    targetFU.op2 = entry.op2;
    targetFU.reorder = entry.reorder;
    targetFU.code = entry.code;
    targetFU.PC = entry.PC;
    targetFU.PTaddr = entry.PTaddr;
    targetFU.BTaddr = entry.BTaddr;
    targetFU.BRoutcome = entry.BRoutcome;
    targetFU.BRprediction = entry.BRprediction;
    targetFU.count = 1;
}

//function to check for empty FU
int checkFU( std::vector<FU_Element> *targetFU )
{
	int FU_tag = -1;				//default = no empty slots
	DEBUG_COUT << "Issue: Check FU for empty slots" << endl;

	for(size_t i = 0; i < targetFU->size(); i++)	//iterate through Functional Units
	{
        	DEBUG_COUT << "Issue:\t" << "FU #" << i << " has " << targetFU->at(i).count << " cycles remaining \n";
		if(targetFU->at(i).count == 0)	//if not busy
		{
			FU_tag = i;
			break;
		}
	}
	DEBUG_COUT << "Empty Element = " << FU_tag << endl << endl;
	return FU_tag;
}

//function to compare waiting RS operands with "broadcast" results
void checkValue( std::vector<RS_Element> *targetRS )
{
	int rename_tag = 0;

	DEBUG_COUT << "Issue: Check Values in RS" << endl;

	for(size_t i = 0; i < targetRS->size(); i++) 
	{
		
		if(targetRS->at(i).valid1 == false)			//check if op1 is valid
		{	
			//DEBUG_COUT << "Issue: checking for new op1 value" << endl;
			rename_tag = targetRS->at(i).op1;	//set rrf tag
			if(rename_tag != -1 && rrf[rename_tag].valid == true)	//if rrf is valid, copy data
			{
				DEBUG_COUT << "Issue: updating new op1 value" << endl;
    				targetRS->at(i).op1 = rrf[rename_tag].data;
    				targetRS->at(i).valid1 = true;
			}
		}
		if(targetRS->at(i).valid2 == 0)			//check for valid op2 value
		{
			//DEBUG_COUT << "Issue: checking for new op2 value" << endl;
			rename_tag = targetRS->at(i).op2;
			if(rename_tag != -1 && rrf[rename_tag].valid == true)
			{
				DEBUG_COUT << "Issue: updating new op1 value" << endl;
    				targetRS->at(i).op2 = rrf[rename_tag].data;
    				targetRS->at(i).valid2 = true;
			}
		}
		if(targetRS->at(i).valid1 == true && targetRS->at(i).valid2 == true)	//set ready bit
			targetRS->at(i).ready = true;
	}
	DEBUG_COUT << endl;
}

//function to check RS for ready instructions and issue if FU is not busy
void checkReady( std::vector<RS_Element> *targetRS )
{
	int FU_tag= 0;
	int cnt = targetRS->size();

	//DEBUG_COUT << "Issue: Check if RS is Ready" << endl;
	for(int i = 0; i < cnt; i++)
	{
		if(targetRS->at(i).ready == true)		//RS entry is ready
		{	
			DEBUG_COUT << "RS at: [" << i << "] is ready to issue" << endl << endl;
			switch(targetRS->at(i).code)	//check FU for empty slot
			{
				case LOGICAL:
				case ADD_SUB_I: 
					FU_tag = checkFU( &fu_add );
					DEBUG_COUT << "Issue:\t" << "Checking ADD FU returned tag: " << FU_tag << endl;

					if (FU_tag != -1)
					{
						//copy RS entry to FU slot & set cycle count
						copyToFU( targetRS->at(i), fu_add, FU_tag, 1 );
						DEBUG_COUT << "Issuing INT RS[" << i << "]: " << targetRS->at(i).PC << " to ADD FU[" << FU_tag << "]\n";
						DEBUG_COUT << "Resizing INT RS" << endl << endl;
						//DEBUG_COUT << "RS size starting = " << targetRS->size() << "\n";

						targetRS->erase( targetRS->begin()+i );	//"pop" RS entry off queue
						i--;		//erase will reindex vector so i needs adjusted
						cnt--;		//erase will reindex vector so cnt needs adjusted
						//DEBUG_COUT << "RS size after issue pop = " << targetRS->size() << "\n";

						targetRS->resize( targetRS->size()+1 ); //"push" empty RS entry onto queue
						//DEBUG_COUT << "RS size after issue push = " << targetRS->size() << "\n" << "\n";
					}
					break;

				case MULT_DIV_I:
					FU_tag = checkFU( &fu_mult );
					DEBUG_COUT << "Issue:\t" << "Checking MULT FU returned tag: " << FU_tag << endl;

					if (FU_tag != -1)
					{
						copyToFU( targetRS->at(i), fu_mult, FU_tag, 3 );
						DEBUG_COUT << "Issuing INT RS[" << i << "]: " << targetRS->at(i).PC << " to MULT FU[" << FU_tag << "]\n";
						DEBUG_COUT << "Resizing INT RS" << endl << endl;
						targetRS->erase( targetRS->begin()+i );	
						i--;
						cnt--;
						targetRS->resize( targetRS->size()+1 );	
					}
					break;

				case FLOATING_POINT:
					FU_tag = checkFU( &fu_fp );
					DEBUG_COUT << "Issue:\t" << "Checking FP FU returned tag: " << FU_tag << endl;

					if (FU_tag != -1)
					{
						copyToFU( targetRS->at(i), fu_fp, FU_tag, 5 );
						DEBUG_COUT << "Issuing FP RS[" << i << "]: " << targetRS->at(i).PC << " to FP FU[" << FU_tag << "]\n";
						DEBUG_COUT << "Resizing FP RS" << endl << endl;
						targetRS->erase( targetRS->begin()+i );	
						i--;
						cnt--;
						targetRS->resize( targetRS->size()+1 );
					}
					break;
				
				//********************* DO WE NEED TO ISSUE MEM IN ORDER? ********
				//********************* IF SO CHANGE THIS CASE *******************
				case LOAD:
				case STORE:
					//if(i == 0)	Would implement if we needed to issue MEM instr in order
					FU_tag = checkFU( &fu_mem );
					DEBUG_COUT << "Issue:\t" << "Checking MEM FU returned tag: " << FU_tag << endl;

					if (FU_tag != -1)
					{
						copyToFU( targetRS->at(i), fu_mem, FU_tag, 2 );
						DEBUG_COUT << "Issuing MEM RS[" << i << "]: " << targetRS->at(i).PC << " to MEM FU[" << FU_tag << "]\n";
						DEBUG_COUT << "Resizing MEM RS" << endl << endl;
						targetRS->erase( targetRS->begin()+i );	
						targetRS->resize( targetRS->size()+1 );
						i--;
						cnt--;
					}
					break;
			
				case JUMP:
				case BRANCH:
					DEBUG_COUT << "Issue:\t" << "Checking if single Branch FU is empty" << endl;
					if(fu_br.count == 0)	//FU empty
					{
						copyToBranchFU( targetRS->at(i), fu_br );
						DEBUG_COUT << "Issuing BR RS[" << i << "]: " << targetRS->at(i).PC << " to BR FU[" << FU_tag << "]\n";
						DEBUG_COUT << "Resizing BR RS" << endl << endl;
						targetRS->erase( targetRS->begin()+i );	
						targetRS->resize( targetRS->size()+1 );
						//can only issue 1 per cycle so no reindexing needed
					}
					break;

                		case NOP: //NOP goes straight to ROB with complete marked as true -- brs
                		default:
                    			break;
			}
		}
	}
}


//quick sweep to see if all of our RS's are empty.
//returns true if empty
//returns false if any element is still busy
bool checkForFinished(std::vector<RS_Element> *targetRS) {
    for(size_t i = 0; i < targetRS->size(); i++) {
        if(targetRS->at(i).busy)
            return false;
    }
    return true;
}

void simulateIssueStage() {

    if(isDispatchFinished &&
        checkForFinished(&rs_int) &&
        checkForFinished(&rs_fp) &&
        checkForFinished(&rs_mem) &&
        checkForFinished(&rs_br) ) {

        cout << "Issue is now finished\n";
        isIssueFinished = true;
        return;
    }

    //DEBUG_COUT << "Issue Stage\n";

    //check Common Data Bus for updates
    DEBUG_COUT << "Issue:\t" << " Checking CDB for RS int" << endl;
    checkValue(&rs_int);
    DEBUG_COUT << "Issue:\t" << " Checking cdb for RS fp" << endl;
    checkValue(&rs_fp);
    DEBUG_COUT << "Issue:\t" << " Checking cdb for RS mem" << endl;
    checkValue(&rs_mem);
    DEBUG_COUT << "Issue:\t" << " Checking cdb for RS br" << endl;
    checkValue(&rs_br);

    //check RS for ready instructions and issue if FU is not busy
    DEBUG_COUT << "Issue:\t" << " Checking if RS int is ready" << endl;
    checkReady(&rs_int);
    DEBUG_COUT << "Issue:\t" << " Checking if RS fp is ready" << endl;
    checkReady(&rs_fp);
    DEBUG_COUT << "Issue:\t" << " Checking if RS mem is ready" << endl;
    checkReady(&rs_mem);
    DEBUG_COUT << "Issue:\t" << " Checking if RS br is ready" << endl;
    checkReady(&rs_br);
}
