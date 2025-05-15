#include "printer.h"
#include <iostream>
using namespace std;

#define RED "\033[31m"
#define GREEN "\033[32m"
#define RESET "\033[0m"

Printer::Printer(TransportationProblem& problem) : tp(problem) {}

void Printer::printCostMatrixAttribute(string key) {
    for (int i = 0; i < tp.numRow; i++) {
        for (int j = 0; j < tp.numCol; j++) {
            if (tp.costMatrix[i][j].included) cout << GREEN;
            else cout << RED;
            cout << tp.costMatrix[i][j].attributes[key] << RESET << " ";
        }
        cout << endl;
    }
}

void Printer::printRowColAttribute(vector<RowCol> rowcols, string key) {
    for (RowCol rowcol : rowcols) cout << rowcol.attributes[key] << " ";
    cout << endl;
}

void Printer::printAllocationMatrix() {
    for (int i = 0; i < tp.numRow; i++) {
        for (int j = 0; j < tp.numCol; j++) 
            cout << tp.allocationMatrix[i][j] << " ";
        cout << endl;
    }
}

void Printer::printState() {
    printCostMatrixAttribute("cost");
    cout << endl;
    printRowColAttribute(tp.rows, "penalty");
    printRowColAttribute(tp.cols, "penalty");
    for(int pen: tp.selectedPenalties) cout << pen;
    cout << endl << endl;
    for(int sup : tp.supplies) cout << sup << " ";
    cout << endl;
    for(int dem : tp.demands) cout << dem << " ";
    cout << endl << endl;
    cout << tp.bestCell.attributes["cost"] << " " << tp.bestCell.attributes["rowIndex"] << " " << tp.bestCell.attributes["colIndex"] << endl << endl;
    printAllocationMatrix();
    cout << endl;
    int num;
    cout << "cont" << endl << endl;
    cin >> num;
}