/*
 * TraceReader.cpp
 *
 *  Created on: Oct 26, 2013
 *      Author: brian
 */

#include "TraceReader.h"

using namespace std;

TraceReader::TraceReader() {
	// TODO Auto-generated constructor stub
	traceFile = new ifstream();
}

TraceReader::~TraceReader() {
	// TODO Auto-generated destructor stub
	traceFile->close();
	delete traceFile;
}

string TraceReader::getNextTraceLine(){
	string line;

	//if the trace file is not open... return
	//now before trying to read
	if(!(traceFile->is_open()))
		return line;

	//get next line from trace
	getline((*traceFile), line);


	return line;
}

Instruction TraceReader::getNextInstruction(){
	string line;
	line = getNextTraceLine();

	printf("Line:\t%s\n", line.c_str());

	Instruction ret(line);

	//replace the following code with the proper parsers...
//	ret.PC = -1;
//	ret.dest = -1;
//	ret.imm = -1;
//	ret.opCode = -1;
//	ret.src1 = -1;
//	ret.src2 = -1;

	return ret;
}

bool TraceReader::isTraceOpen() {
	return traceFile->is_open();
}

//returns 	0 for success
//			-1 for failure to open
int TraceReader::openTrace(char *traceName){
	traceFile->open(traceName);

	//check to make sure it's actually open..
	if(!(traceFile->is_open()))
		return -1;

	return 0;
}

