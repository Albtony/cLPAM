#include "classes.h"
#include <algorithm>
#include <set>
#include <iostream>
using namespace std;

void TransportationProblem::initialize(ifstream &inputFile) {
    inputFile >> numSources >> numDestinations;
    numRow = numSources;
    numCol = numDestinations;

    costMatrix.resize(numSources, vector<Cell>(numDestinations));
    allocationMatrix.resize(numSources, vector<int>(numDestinations, 0));
    for (int i = 0; i < numSources; ++i) {
        for (int j = 0; j < numDestinations; ++j) {
            inputFile >> costMatrix[i][j].attributes["cost"];
            costMatrix[i][j].attributes["rowIndex"] = i;
            costMatrix[i][j].attributes["colIndex"] = j;
        }
    }

    supplies.resize(numSources);
    for (int i = 0; i < numSources; ++i) {
        inputFile >> supplies[i];
    }

    demands.resize(numDestinations);
    for (int i = 0; i < numDestinations; ++i) {
        inputFile >> demands[i];
    }

    initSupplies = supplies;
    initDemands = demands;

    rows.resize(numRow);
    cols.resize(numCol);

    inputFile >> optimalSolution;
}

void TransportationProblem::calculateTopBottom() {
    // Find minimum cost in each row and substract
    for (int i = 0; i < numRow; ++i) {
        int minRowCost = costMatrix[i][0].attributes["cost"]; 
        for (int j = 1; j < numCol; ++j) {
            minRowCost = min(minRowCost, costMatrix[i][j].attributes["cost"]);
        }
        for (int j = 0; j < numCol; ++j) {
            costMatrix[i][j].attributes["top"] = costMatrix[i][j].attributes["cost"] - minRowCost;
        }
    }

    // Find minimum cost in each column and substract
    for (int j = 0; j < numCol; ++j) {
        int minColCost = costMatrix[0][j].attributes["cost"];
        for (int i = 1; i < numRow; ++i) {
            minColCost = min(minColCost, costMatrix[i][j].attributes["cost"]);
        }
        for (int i = 0; i < numRow; ++i) {
            costMatrix[i][j].attributes["bot"] = costMatrix[i][j].attributes["cost"] - minColCost;
        }
    }
}

void TransportationProblem::updateAttributes() {
    calculateCostAllocation();
    calculateRowColTotalCost(); 
    calculateSupremeCell();
}

void TransportationProblem::calculateCostAllocation() {
    for (int i = 0; i < numRow; i++) {
        for (int j = 0; j < numCol; j++) {
            costMatrix[i][j].attributes["A"] = min(supplies[i], demands[j]);
            costMatrix[i][j].attributes["CA"] = costMatrix[i][j].attributes["cost"] * costMatrix[i][j].attributes["A"];
        }
    }
}

void TransportationProblem::calculateRowColTotalCost() {
    int rowColTotalCost;
    for (int i = 0; i < numRow; i++) {
        rowColTotalCost = 0;      
        for (int j = 0; j < numCol; j++) 
            if (costMatrix[i][j].included)
                rowColTotalCost += costMatrix[i][j].attributes["cost"];
        rows[i].attributes["totalCost"] = rowColTotalCost;
    }
    for (int i = 0; i < numCol; i++) {
        rowColTotalCost = 0;      
        for (int j = 0; j < numRow; j++) 
            if (costMatrix[i][j].included)
                rowColTotalCost += costMatrix[j][i].attributes["cost"];
        cols[i].attributes["totalCost"] = rowColTotalCost;
    }
}

void TransportationProblem::calculateSupremeCell() {
    int includedRowCell = 0;
    int includedColCell = 0;
    for (int i = 0; i < numRow; i++) {
        for (int j = 0; j < numCol; j++) {
            costMatrix[i][j].attributes["rowSC"] = rows[i].attributes["totalCost"] - (numRow * costMatrix[i][j].attributes["cost"]);
        }
    }
    for (int i = 0; i < numCol; i++) 
        for (int j = 0; j < numRow; j++) 
            costMatrix[j][i].attributes["colSC"] = cols[i].attributes["totalCost"] - (numCol * costMatrix[i][j].attributes["cost"]);
}

int TransportationProblem::calculatePenalty(const std::set<int>& sortedValues, PenaltyStrategy::Calculation strategy) {
    if (sortedValues.empty()) return -1;
    if (sortedValues.size() == 1) return 0;

    switch (strategy) {
        case PenaltyStrategy::Calculation::TwoMin: {
            auto it = sortedValues.begin();
            int firstMin = *it;
            int secondMin = *(++it);
            return secondMin - firstMin; 
        }
        case PenaltyStrategy::Calculation::TwoMax: {
            auto it = sortedValues.end();
            --it;
            int last = *it;
            --it;
            return last - *it; 
        }
        case PenaltyStrategy::Calculation::MinMax:{
            return *sortedValues.rbegin() - *sortedValues.begin();
        } 
        default:
            return -1;
    }
}

void TransportationProblem::calculatePenalties(PenaltyStrategy::Calculation strategy) {
    for (int i = 0; i < numRow; ++i) {
        set<int> topValues;
        for (int j = 0; j < numCol; ++j) 
            if (costMatrix[i][j].included) 
                topValues.insert(costMatrix[i][j].attributes["top"]);
        rows[i].attributes["penalty"] = calculatePenalty(topValues, strategy);
    }
    
    for (int i = 0; i < numCol; ++i) {
        set<int> botValues;
        for (int j = 0; j < numRow; ++j) 
            if (costMatrix[j][i].included) 
                botValues.insert(costMatrix[j][i].attributes["bot"]);

        cols[i].attributes["penalty"] = calculatePenalty(botValues, strategy);
    }
}

void TransportationProblem::selectPenalties(PenaltyStrategy::Selection strategy) {
    selectedPenalties.clear();

    set<int> combinedPenalties;
    for (const auto& row : rows) {
        auto it = row.attributes.find("penalty");
        if (it != row.attributes.end())
            combinedPenalties.insert(it->second);  
    }
    for (const auto& col : cols) {
        auto it = col.attributes.find("penalty");
        if (it != col.attributes.end())
            combinedPenalties.insert(it->second);  
    }

    switch (strategy) {
        case PenaltyStrategy::Selection::MaxPenalty: {
            auto maxIt = --combinedPenalties.end();
            selectedPenalties.push_back(*maxIt);
            break;
        }
        case PenaltyStrategy::Selection::MinPenalty: {
            auto min = combinedPenalties.begin();
            selectedPenalties.push_back(*min);
            break;
        }
        case PenaltyStrategy::Selection::TwoMax: {
            if (combinedPenalties.size() >= 2) {
                auto maxIt = --combinedPenalties.end();
                auto secondMaxIt = std::prev(maxIt);
                selectedPenalties.push_back(*maxIt);
                selectedPenalties.push_back(*secondMaxIt);
            } else {
                selectedPenalties.push_back(*combinedPenalties.begin());
            }
            break;
        }
        default:
            cout << "INVALID STRATEGY\n";
            return;
            break;
    }
}

void TransportationProblem::tieBreakPenalties(PenaltyStrategy::TieBreaker strategy) {
    bestRowPenaltyIndices.clear();
    bestColPenaltyIndices.clear();

    vector<int> rowTC, colTC, rowPen, colPen;

    for (RowCol row : rows) {
        rowTC.push_back(row.attributes.at("totalCost"));
        rowPen.push_back(row.attributes.at("penalty"));
    }
        
    for (RowCol col : cols) {
        colTC.push_back(col.attributes.at("totalCost"));
        colPen.push_back(col.attributes.at("penalty"));
    }
    
    switch (strategy) {
        case PenaltyStrategy::TieBreaker::AllPenalties: {
            for (int penalty : selectedPenalties) {
                for (int i = 0; i < rowPen.size(); ++i) 
                    if (rowPen[i] == penalty) 
                        bestRowPenaltyIndices.push_back(i);
                for (int i = 0; i < colPen.size(); ++i) 
                    if (colPen[i] == penalty) 
                        bestColPenaltyIndices.push_back(i);
            } 
            break;
        }
        case PenaltyStrategy::TieBreaker::MaxTC: {
            int maxRowTCIndex = max_element(rowTC.begin(), rowTC.end()) - rowTC.begin();
            int maxColTCIndex = max_element(colTC.begin(), colTC.end()) - colTC.begin();

            if (rowTC[maxRowTCIndex] >= colTC[maxColTCIndex]) 
                bestRowPenaltyIndices.push_back(maxRowTCIndex);
            else
                bestColPenaltyIndices.push_back(maxColTCIndex);
            break;
        }
        case PenaltyStrategy::TieBreaker::MinTC: {
            int minRowTCIndex = min_element(rowTC.begin(), rowTC.end()) - rowTC.begin();
            int minColTCIndex = min_element(colTC.begin(), colTC.end()) - colTC.begin();

            if (rowTC[minRowTCIndex] <= colTC[minColTCIndex]) 
                bestRowPenaltyIndices.push_back(minRowTCIndex);
            else
                bestColPenaltyIndices.push_back(minColTCIndex);
            break;
        }
        default:
            cout << "INVALID STRATEGY\n";
            return;
            break;
    }
}

void TransportationProblem::selectCells(CellStrategy::Selection strategy) { 
    vector<Cell> cellsFromPenalty;

    for (int rowIndex : bestRowPenaltyIndices) 
        for (int colIndex = 0; colIndex < numCol; colIndex++) 
            cellsFromPenalty.push_back(costMatrix[rowIndex][colIndex]);
    for (int colIndex : bestColPenaltyIndices) 
        for (int rowIndex = 0; rowIndex < numRow; rowIndex++) 
            cellsFromPenalty.push_back(costMatrix[rowIndex][colIndex]);

    switch (strategy) {
        case CellStrategy::Selection::MinCost: {
            int minCost = INT_MAX;
            for (Cell cell : cellsFromPenalty) {
                if (cell.included && (cell.attributes["cost"] <= minCost)) {
                    minCost = cell.attributes["cost"];
                    selectedCells.push_back(cell);
                }
            }
            break;
        }
        case CellStrategy::Selection::MinCA: {
            int minCA = INT_MAX;
            for (Cell cell : cellsFromPenalty) {
                if (cell.included && (cell.attributes["CA"] <= minCA)) {
                    minCA = cell.attributes["CA"];
                    selectedCells.push_back(cell);
                }
            }
            break;
        }
        case CellStrategy::Selection::MaxSC: {
            int maxSC = INT_MIN;
            for (Cell cell : cellsFromPenalty) {
                int currMaxSC = max(cell.attributes["rowSC"], cell.attributes["colSC"]);
                if (cell.included && (currMaxSC >= maxSC)) {
                    maxSC = currMaxSC;
                    selectedCells.push_back(cell);
                }
            }
            break;
        }
        case CellStrategy::Selection::MinSC: {
            int minSC = INT_MAX;
            for (Cell cell : cellsFromPenalty) {
                int currMinSC = min(cell.attributes["rowSC"], cell.attributes["colSC"]);
                if (cell.included && (currMinSC <= minSC)) {
                    minSC = currMinSC;
                    selectedCells.push_back(cell);
                }
            }
            break;
        }
        default:
            cout << "INVALID STRATEGY\n";
            return;
            break;
    }
}

void TransportationProblem::tieBreakCells(CellStrategy::TieBreaker strategy) {
    switch (strategy) {
        case CellStrategy::TieBreaker::Index: {
            bestCell = selectedCells[0];
            break;   
        }
        case CellStrategy::TieBreaker::MaxA: {
            int maxA = INT_MIN;
            for (Cell cell : selectedCells) {
                if (cell.included && (cell.attributes["A"] > maxA)) {
                    maxA = cell.attributes["A"];
                    bestCell = cell;
                }
            }
            break;   
        }
        case CellStrategy::TieBreaker::MinA: {
            int minA = INT_MAX;
            for (Cell cell : selectedCells) {
                if (cell.included && (cell.attributes["A"] < minA)) {
                    minA = cell.attributes["A"];
                    bestCell = cell;
                }
            }
            break;   
        }
        case CellStrategy::TieBreaker::MaxCA: {
            int maxCA = INT_MIN;
            for (Cell cell : selectedCells) {
                if (cell.included && (cell.attributes["CA"] > maxCA)) {
                    maxCA = cell.attributes["CA"];
                    bestCell = cell;
                }
            }
            break;   
        }
        case CellStrategy::TieBreaker::MinCA: {
            int minCA = INT_MAX;
            for (Cell cell : selectedCells) {
                if (cell.included && (cell.attributes["CA"] < minCA)) {
                    minCA = cell.attributes["CA"];
                    bestCell = cell;
                }
            }
            break;   
        }
        case CellStrategy::TieBreaker::MaxD: {
            int maxD = INT_MIN;
            for (Cell cell : selectedCells) {
                if (cell.included && (demands[cell.attributes["rowIndex"]] > maxD)) {
                    maxD = demands[cell.attributes["rowIndex"]];
                    bestCell = cell;
                }
            }
            break;   
        }
        case CellStrategy::TieBreaker::MinD: {
            int minD = INT_MAX;
            for (Cell cell : selectedCells) {
                if (cell.included && (demands[cell.attributes["rowIndex"]] < minD)) {
                    minD = demands[cell.attributes["rowIndex"]];
                    bestCell = cell;
                }
            }
            break;   
        }
        case CellStrategy::TieBreaker::MaxS: {
            int maxS = INT_MIN;
            for (Cell cell : selectedCells) {
                if (cell.included && (supplies[cell.attributes["colIndex"]] > maxS)) {
                    maxS = supplies[cell.attributes["colIndex"]];
                    bestCell = cell;
                }
            }
            break;   
        }
        case CellStrategy::TieBreaker::MinS: {
            int minS = INT_MAX;
            for (Cell cell : selectedCells) {
                if (cell.included && (supplies[cell.attributes["colIndex"]] < minS)) {
                    minS = supplies[cell.attributes["colIndex"]];
                    bestCell = cell;
                }
            }
            break;   
        }
        case CellStrategy::TieBreaker::MaxSDM: {
            int maxSDM = INT_MIN;
            for (Cell cell : selectedCells) {
                int currSDM = supplies[cell.attributes["colIndex"]] * demands[cell.attributes["rowIndex"]];
                if (cell.included && (currSDM > maxSDM)) {
                    maxSDM = currSDM;
                    bestCell = cell;
                }
            }
            break;   
        }
        case CellStrategy::TieBreaker::MinSDM: {
            int minSDM = INT_MAX;
            for (Cell cell : selectedCells) {
                int currSDM = supplies[cell.attributes["colIndex"]] * demands[cell.attributes["rowIndex"]];
                if (cell.included && (currSDM < minSDM)) {
                    minSDM = currSDM;
                    bestCell = cell;
                }
            }
            break;   
        }
        case CellStrategy::TieBreaker::MaxSoD: {
            int maxSoD = INT_MIN;
            for (Cell cell : selectedCells) {
                int currMaxSoD = max(supplies[cell.attributes["colIndex"]], demands[cell.attributes["rowIndex"]]);
                if (cell.included && (currMaxSoD > maxSoD)) {
                    maxSoD = currMaxSoD;
                    bestCell = cell;
                }
            }
            break;   
        }
        case CellStrategy::TieBreaker::MinSoD: {
            int minSoD = INT_MAX;
            for (Cell cell : selectedCells) {
                int currMinSoD = min(supplies[cell.attributes["colIndex"]], demands[cell.attributes["rowIndex"]]);
                if (cell.included && (currMinSoD < minSoD)) {
                    minSoD = currMinSoD;
                    bestCell = cell;
                }
            }
            break;   
        }
        case CellStrategy::TieBreaker::MaxSTB: {
            int maxSTB = INT_MIN;
            for (Cell cell : selectedCells) {
                int currSTB = cell.attributes["top"] + cell.attributes["bottom"];
                if (cell.included && (currSTB > maxSTB)) {
                    maxSTB = currSTB;
                    bestCell = cell;
                }
            }
            break;   
        }
        case CellStrategy::TieBreaker::MinSTB: {
            int minSTB = INT_MAX;
            for (Cell cell : selectedCells) {
                int currSTB = cell.attributes["top"] + cell.attributes["bottom"];
                if (cell.included && (currSTB < minSTB)) {
                    minSTB = currSTB;
                    bestCell = cell;
                }
            }
            break;   
        }
        default:
            cout << "INVALID STRATEGY\n";
            return;
            break;
    }
}

void TransportationProblem::allocateUnits() {
    int cellRowIndex = bestCell.attributes["rowIndex"];
    int cellColIndex = bestCell.attributes["colIndex"];
    int allocation = bestCell.attributes["A"];

    allocationMatrix[cellRowIndex][cellColIndex] = allocation;
}

void TransportationProblem::updateSupplyDemand() {
    int cellRowIndex = bestCell.attributes["rowIndex"];
    int cellColIndex = bestCell.attributes["colIndex"];
    int allocation = bestCell.attributes["A"];

    supplies[cellRowIndex] -= allocation;
    demands[cellColIndex] -= allocation;

    if (supplies[cellRowIndex] == 0) 
        for (int i = 0; i < numCol; i++)
            costMatrix[cellRowIndex][i].included = false;
    if (demands[cellColIndex] == 0)
        for (int i = 0; i < numRow; i++)
            costMatrix[i][cellColIndex].included = false;
} 

bool TransportationProblem::isSolved() {
    for (int supply : supplies)
        if (supply != 0) 
            return false;
    for (int demand : demands)
        if (demand != 0) 
            return false;
    return true;
}

void TransportationProblem::calculateTotalCost() {
    totalCost = 0;
    for (int i = 0; i < numRow; i++) 
        for (int j = 0; j < numCol; j++) 
            totalCost += costMatrix[i][j].attributes["cost"] * allocationMatrix[i][j];
}