/*LPAM Technique*/

#include <iostream>
#include <fstream>
#include "classes.h"
#include "printer.h"
using namespace std;

int main() {
    ifstream inputFile("./input/input1.txt");
    
    if (!inputFile.is_open()) {
        cerr << "Error opening file!" << endl;
        return 1;
    }

    TransportationProblem problem;
    Printer printer(problem);

    PenaltyStrategy::Calculation pCal = PenaltyStrategy::Calculation::TwoMin;
    PenaltyStrategy::Selection pSel = PenaltyStrategy::Selection::MaxPenalty;
    PenaltyStrategy::TieBreaker pTie = PenaltyStrategy::TieBreaker::AllPenalties;
    CellStrategy::Selection cSel = CellStrategy::Selection::MinCost;
    CellStrategy::TieBreaker cTie = CellStrategy::TieBreaker::MinSTB;

    problem.initialize(inputFile);
    problem.updateAttributes();

    while (!problem.isSolved()) {
        problem.calculatePenalties(pCal);
        problem.selectPenalties(pSel);
        problem.tieBreakPenalties(pTie);
        problem.selectCells(cSel);
        problem.tieBreakCells(cTie);
        problem.allocateUnits();
        problem.updateSupplyDemand();
        printer.printState();
    }

    cout << "DONE\n";

    return 0;
}