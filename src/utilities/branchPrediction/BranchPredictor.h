/*
 * BranchPredictor.h
 *
 *  Created on: Nov 12, 2013
 *      Author: brian
 */

#ifndef BRANCHPREDICTOR_H_
#define BRANCHPREDICTOR_H_

#include <vector>
#include "Instruction.h"

class BranchPredictor {
public:
	BranchPredictor();
	virtual ~BranchPredictor();

	void incrementPredictionMissCount();

	void resizeBTB(int size);

	bool getPredictionForInstruction(Instruction &instrToPredict);

	void updatePredictorWithResults(FU_Element entry);

	void printPredictionStatistics();
	void printBTB();

//moved the below to private since the stages that reference this don't need to know the
//logic it uses... the predictor should be a magic 8-ball to them
private:
	short shiftReg;
	int predictionTable[1024];

	unsigned int branchPredictionCount;
	unsigned int predictionMissCount;

	std::vector<BTB_Element> btb;
	int btbInsertIndex; //for round-robin entry into btb

	void shift_left(bool bit);
	short hash (int pc);
	int	get_bp (int hashAddr);
	void inc_state(int hashAddr);
	void dec_state(int hashAddr);

	void updateBTBRecord(int instrPC, int brachTargetPC, bool wasTaken);

};

#endif /* BRANCHPREDICTOR_H_ */
