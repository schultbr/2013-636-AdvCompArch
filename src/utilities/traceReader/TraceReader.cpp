/*
 * TraceReader.cpp
 *
 *  Created on: Oct 26, 2013
 *      Author: brian
 */

#include "TraceReader.h"
#include <unistd.h>
#include <stdio.h>
#include <iostream>
#include <cstdlib>
#include "GlobalVars.h"

using namespace std;

TraceReader::TraceReader() {
    traceFile = new ifstream();
}

TraceReader::~TraceReader() {
    traceFile->close();
    delete traceFile;
}

string TraceReader::getNextTraceLine() {
    string line;

    if (endOfTraceReached)
        return line;

    //if the trace file is not open... return
    //now before trying to read
    if (!(traceFile->is_open()))
        return line;

    //get next line from trace
    getline((*traceFile), line);

    if ((*traceFile).eof()) {
        endOfTraceReached = true;
        return "";
    }

    return line;
}

Instruction TraceReader::getNextInstruction() {
    string line;
    line = getNextTraceLine();

    if (line == "" && endOfTraceReached) {
        Instruction ret;
        ret.setIsEOF(true);
        return ret;
    }
    else {
        Instruction ret(line);
        return ret;
    }
}

bool TraceReader::isTraceOpen() {
    return traceFile->is_open();
}

//returns 	0 for success
//			-1 for failure to open
int TraceReader::openTrace(std::string traceName) {
    cout << "Opening file: " << traceName << endl;
    traceFile->open(traceName.c_str());

    //check to make sure it's actually open..
    if (!(traceFile->is_open())) {
        cout << "ERROR: unable to open file.\n";
        return -1; //quit if it cant be opened.
    }

    return 0;
}

int TraceReader::peekNextPC() {
    Instruction nextInstr;

    //capture current position
    streampos bookmark = traceFile->tellg();

    //bring in the next instruction
    nextInstr = getNextInstruction();

    //return input reader to our original position...
    traceFile->seekg(bookmark);

    return nextInstr.PC;
}

