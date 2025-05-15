#pragma once

#include "classes.h"
#include <string>
using namespace std;

class Printer {
    TransportationProblem& tp;
    public:
        Printer(TransportationProblem& problem);
        void printCostMatrixAttribute(string key);
        void printRowColAttribute(vector<RowCol> rowcols, string key);
        void printAllocationMatrix();
        void printState();
};