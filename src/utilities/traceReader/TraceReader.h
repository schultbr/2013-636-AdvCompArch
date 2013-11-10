/*
 * TraceReader.h
 *
 *  Created on: Oct 26, 2013
 *      Author: brian
 */

#ifndef TRACEREADER_H_
#define TRACEREADER_H_

#include <fstream>
#include <string.h>
#include "Instruction.h"

class TraceReader {
public:
	TraceReader();
	virtual ~TraceReader();

	Instruction getNextInstruction();
	std::string getNextTraceLine();
	bool isTraceOpen();
	int peekNextPC();
	int openTrace(char *traceName);

private:
	std::ifstream *traceFile;
};

#endif /* TRACEREADER_H_ */
