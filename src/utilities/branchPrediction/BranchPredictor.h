/*
 * BranchPredictor.h
 *
 *  Created on: Nov 12, 2013
 *      Author: brian
 */

#ifndef BRANCHPREDICTOR_H_
#define BRANCHPREDICTOR_H_

class Instruction; //forward declaration to avoid circular includes...

class BranchPredictor {
public:
	BranchPredictor();
	virtual ~BranchPredictor();

	bool getPredictionForInstruction(Instruction &instrToPredict);
	void updatePredictionWithResults(Instruction &executedInstr);

//moved the below to private since the stages that reference this don't need to know the
//logic it uses... the predictor should be a magic 8-ball to them
private:
	short shiftReg;
	int predictionTable[1024];

	void shift_left(bool bit);
	short hash (int pc);
	int	get_bp (int hashAddr);
	void inc_state(int hashAddr);
	void dec_state(int hashAddr);

};

#endif /* BRANCHPREDICTOR_H_ */
