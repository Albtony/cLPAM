
#include <bits/stdc++.h>
#include <classes.h>
using namespace std;
namespace fs = filesystem; 

void calculateTopBottom(vector<vector<CostCell>> &costMatrix) {
    int numSources = costMatrix.size();
    int numDestinations = costMatrix[0].size();

    // Find minimum cost in each row and substract
    for (int i = 0; i < numSources; ++i) {
        int minRowCost = costMatrix[i][0].cost; 
        for (int j = 1; j < numDestinations; ++j) {
            minRowCost = min(minRowCost, costMatrix[i][j].cost);
        }
        for (int j = 0; j < numDestinations; ++j) {
            costMatrix[i][j].top = costMatrix[i][j].cost - minRowCost;
        }
    }

    // Find minimum cost in each column and substract
    for (int j = 0; j < numDestinations; ++j) {
        int minColCost = costMatrix[0][j].cost;
        for (int i = 1; i < numSources; ++i) {
            minColCost = min(minColCost, costMatrix[i][j].cost);
        }
        for (int i = 0; i < numSources; ++i) {
            costMatrix[i][j].bottom = costMatrix[i][j].cost - minColCost;
        }
    }
}

int calculateRowPenalty(const vector<int> &rowValues, PenaltyCalculationStrategy pcStrat) {
    vector<int> uniqueValues = rowValues;
    sort(uniqueValues.begin(), uniqueValues.end());
    uniqueValues.erase(unique(uniqueValues.begin(), uniqueValues.end()), uniqueValues.end());

    if (uniqueValues.size() < 1) {
        return -1;
    }

    if (uniqueValues.size() == 1) {
        return 0;
    }

    switch (pcStrat) {
        case PenaltyCalculationStrategy::TwoMin:
            return uniqueValues[1] - uniqueValues[0];

        case PenaltyCalculationStrategy::TwoMax:
            return uniqueValues.back() - uniqueValues[uniqueValues.size() - 2];

        case PenaltyCalculationStrategy::MinMax:
            return uniqueValues.back() - uniqueValues[0];

        default:
            return -1;
    }
}


int calculateColPenalty(const vector<int> &colValues, PenaltyCalculationStrategy pcStrat) {
    vector<int> uniqueValues = colValues;
    sort(uniqueValues.begin(), uniqueValues.end());
    uniqueValues.erase(unique(uniqueValues.begin(), uniqueValues.end()), uniqueValues.end());

    if (uniqueValues.size() < 1) {
        return -1;
    }

    if (uniqueValues.size() == 1) {
        return 0;
    }

    switch (pcStrat) {
        case PenaltyCalculationStrategy::TwoMin:
            return uniqueValues[1] - uniqueValues[0];

        case PenaltyCalculationStrategy::TwoMax:
            return uniqueValues.back() - uniqueValues[uniqueValues.size() - 2];

        case PenaltyCalculationStrategy::MinMax:
            return uniqueValues.back() - uniqueValues[0];

        default:
            return -1;
    }
}

void calculatePenalties(const vector<vector<CostCell>> &costMatrix, vector<int> &rowPenalties, vector<int> &colPenalties, PenaltyCalculationStrategy pcStrat) {
    colPenalties.assign(costMatrix[0].size(), 0);
    rowPenalties.assign(costMatrix.size(), 0);

    for (int i = 0; i < costMatrix.size(); ++i) {
        vector<int> rowValues;
        for (int j = 0; j < costMatrix[i].size(); ++j) {
            if (costMatrix[i][j].included) {
                rowValues.push_back(costMatrix[i][j].top);
            }
        }

        rowPenalties[i] = calculateRowPenalty(rowValues, pcStrat);
    }

    for (int j = 0; j < costMatrix[0].size(); ++j) {
        vector<int> colValues;
        for (int i = 0; i < costMatrix.size(); ++i) {
            if (costMatrix[i][j].included) {
                colValues.push_back(costMatrix[i][j].bottom);
            }
        }

        colPenalties[j] = calculateColPenalty(colValues, pcStrat);
    }
}

int calculateRowSC(const vector<vector<CostCell>> &costMatrix, int rowIndex, int colIndex) {
    int rowSum = 0;  
    int numCols = costMatrix[0].size();  

    for (int j = 0; j < numCols; ++j) rowSum += costMatrix[rowIndex][j].cost;
    int rowSC = rowSum - (numCols * costMatrix[rowIndex][colIndex].cost);

    return rowSC;
}

int calculateColSC (const vector<vector<CostCell>> &costMatrix, int rowIndex, int colIndex) {
    int colSum = 0;
    int numRows = costMatrix.size();  

    for (int i = 0; i < numRows; ++i) colSum += costMatrix[i][colIndex].cost;
    int colSC = colSum - (numRows * costMatrix[rowIndex][colIndex].cost);

    return colSC;
}

void checkRow(const vector<vector<CostCell>> &costMatrix, const vector<int> &supply, const vector<int> &demand, int rowIndex, ComparatorValues &comparator, int &selectedRowCostIndex, int &selectedColCostIndex, CellSelectionStrategy csStrat, CellTieBreakerStrategy ctbStrat) {
    for (int j = 0; j < costMatrix[rowIndex].size(); ++j) {
        if (!costMatrix[rowIndex][j].included) continue;
        
        bool update = false;
        int currentCost = costMatrix[rowIndex][j].cost;
        int currentSupply = supply[rowIndex];  
        int currentDemand = demand[j]; 
        int top = costMatrix[rowIndex][j].top;
        int bottom = costMatrix[rowIndex][j].bottom;
        int sumTopBottom = top + bottom;
        int shippingAmount = min(currentDemand, currentSupply);
        int allocationCost = currentCost * shippingAmount;
        int supplyDemandMult = currentSupply * currentDemand;
        int rowSC = calculateRowSC(costMatrix, rowIndex, j);
        int colSC = calculateColSC(costMatrix, rowIndex, j);
        int minSC = min(rowSC, colSC);
        int maxSC = max(rowSC, colSC);

        switch (csStrat) {
            case CellSelectionStrategy::MinCost:
                if(currentCost < comparator.minCost) {
                    comparator.bestCost = currentCost;
                    update = true;
                }
                break;
            case CellSelectionStrategy::MinCA:
                if (allocationCost < comparator.minAllocationCost) {
                    comparator.bestCost = currentCost;
                    update = true;
                }
                break;
            case CellSelectionStrategy::MinSC:
                if (minSC < comparator.minSC) {
                    comparator.bestCost = currentCost;
                    update = true;
                }
                break;
            case CellSelectionStrategy::MaxSC:
                if (maxSC > comparator.maxSC) {
                    comparator.bestCost = currentCost;
                    update = true;
                }
                break;
        }

        if (update) {
            comparator.minCost = currentCost;
            comparator.maxCost = currentCost;
            comparator.maxSupply = currentSupply;
            comparator.minSupply = currentSupply;
            comparator.maxDemand = currentDemand;
            comparator.minDemand = currentDemand;
            comparator.maxSumTopBottom = sumTopBottom;
            comparator.minSumTopBottom = sumTopBottom;
            comparator.maxShippingAmount = shippingAmount;
            comparator.minShippingAmount = shippingAmount;
            comparator.maxAllocationCost = allocationCost;
            comparator.minAllocationCost = allocationCost;
            comparator.maxSupplyDemandMult = supplyDemandMult;
            comparator.minSupplyDemandMult = supplyDemandMult;
            comparator.maxSC - maxSC;
            comparator.minSC = minSC;
            selectedRowCostIndex = rowIndex;
            selectedColCostIndex = j;
            update = false;
        }

        if (currentCost == comparator.minCost) {
            switch (ctbStrat) {
                case CellTieBreakerStrategy::MaxS:
                    if (currentSupply > comparator.maxSupply) {
                        comparator.maxSupply = currentSupply;
                        selectedRowCostIndex = rowIndex;
                        selectedColCostIndex = j;
                    }
                    break;

                case CellTieBreakerStrategy::MinS:
                    if (currentSupply < comparator.minSupply) {
                        comparator.minSupply = currentSupply;
                        selectedRowCostIndex = rowIndex;
                        selectedColCostIndex = j;
                    }
                    break;

                case CellTieBreakerStrategy::MaxD:
                    if (currentDemand > comparator.maxDemand) {
                        comparator.maxDemand = currentDemand;
                        selectedRowCostIndex = rowIndex;
                        selectedColCostIndex = j;
                    }
                    break;

                case CellTieBreakerStrategy::MinD:
                    if (currentDemand < comparator.minDemand) {
                        comparator.minDemand = currentDemand;
                        selectedRowCostIndex = rowIndex;
                        selectedColCostIndex = j;
                    }
                    break;

                case CellTieBreakerStrategy::MaxSTB:
                    if (sumTopBottom > comparator.maxSumTopBottom) {
                        comparator.maxSumTopBottom = sumTopBottom;
                        selectedRowCostIndex = rowIndex;
                        selectedColCostIndex = j;
                    }
                    break;

                case CellTieBreakerStrategy::MinSTB:
                    if (sumTopBottom < comparator.minSumTopBottom) {
                        comparator.minSumTopBottom = sumTopBottom;
                        selectedRowCostIndex = rowIndex;
                        selectedColCostIndex = j;
                    }
                    break;
                
                case CellTieBreakerStrategy::MaxSoD:
                    if (max(currentSupply, currentDemand) > max(comparator.maxSupply, comparator.maxDemand)) {
                        comparator.maxSupply = currentSupply;
                        comparator.maxDemand = currentDemand;
                        selectedRowCostIndex = rowIndex;
                        selectedColCostIndex = j;
                    }
                    break;

                case CellTieBreakerStrategy::MinSoD:
                    if (min(currentSupply, currentDemand) < min(comparator.minSupply, comparator.minDemand)) {
                        comparator.minSupply = currentSupply;
                        comparator.minDemand = currentDemand;
                        selectedRowCostIndex = rowIndex;
                        selectedColCostIndex = j;
                    }
                    break;

                case CellTieBreakerStrategy::MaxA:
                    if (shippingAmount > comparator.maxShippingAmount) {
                        comparator.maxShippingAmount = shippingAmount;
                        selectedRowCostIndex = rowIndex;
                        selectedColCostIndex = j;
                    }
                    break;
                
                case CellTieBreakerStrategy::MinA:
                    if (shippingAmount < comparator.minShippingAmount) {
                        comparator.maxShippingAmount = shippingAmount;
                        selectedRowCostIndex = rowIndex;
                        selectedColCostIndex = j;
                    }
                    break;
                
                case CellTieBreakerStrategy::MaxCA:
                    if (allocationCost > comparator.maxAllocationCost) {
                        comparator.maxAllocationCost = allocationCost;
                        selectedRowCostIndex = rowIndex;
                        selectedColCostIndex = j;
                    }
                    break;
                
                case CellTieBreakerStrategy::MinCA:
                    if (allocationCost < comparator.minAllocationCost) {
                        comparator.maxAllocationCost = allocationCost;
                        selectedRowCostIndex = rowIndex;
                        selectedColCostIndex = j;
                    }
                    break;
                
                case CellTieBreakerStrategy::MaxSDM:
                    if (supplyDemandMult > comparator.maxSupplyDemandMult) {
                        comparator.maxSupplyDemandMult = supplyDemandMult;
                        selectedRowCostIndex = rowIndex;
                        selectedColCostIndex = j;
                    }
                
                case CellTieBreakerStrategy::MinSDM:
                    if (supplyDemandMult < comparator.minSupplyDemandMult) {
                        comparator.maxSupplyDemandMult = supplyDemandMult;
                        selectedRowCostIndex = rowIndex;
                        selectedColCostIndex = j;
                    }

                case CellTieBreakerStrategy::Index:
                    break;

                default:
                    std::cerr << "Unknown strategy" << std::endl;
                    break;
            }
        }
    }
}

void checkColumn(const vector<vector<CostCell>> &costMatrix, const vector<int> &supply, const vector<int> &demand, int colIndex, ComparatorValues &comparator, int &selectedRowCostIndex, int &selectedColCostIndex, CellSelectionStrategy csStrat, CellTieBreakerStrategy ctbStrat) {
    for (int i = 0; i < costMatrix.size(); ++i) {
        if (!costMatrix[i][colIndex].included) continue;

        bool update = false;
        int currentCost = costMatrix[i][colIndex].cost;
        int currentSupply = supply[i];  
        int currentDemand = demand[colIndex]; 
        int top = costMatrix[i][colIndex].top;
        int bottom = costMatrix[i][colIndex].bottom;
        int sumTopBottom = top + bottom;
        int shippingAmount = min(currentDemand, currentSupply);
        int allocationCost = currentCost * shippingAmount;
        int supplyDemandMult = currentSupply * currentDemand;
        int rowSC = calculateRowSC(costMatrix, i, colIndex);
        int colSC = calculateColSC(costMatrix, i, colIndex);
        int minSC = min(rowSC, colSC);
        int maxSC = max(rowSC, colSC);

        switch (csStrat) {
            case CellSelectionStrategy::MinCost:
                if(currentCost < comparator.minCost) {
                    comparator.bestCost = currentCost;
                    update = true;
                }
                break;
            case CellSelectionStrategy::MinCA:
                if (allocationCost < comparator.minAllocationCost) {
                    comparator.bestCost = currentCost;
                    update = true;
                }
                break;
            case CellSelectionStrategy::MinSC:
                if (minSC < comparator.minSC) {
                    comparator.bestCost = currentCost;
                    update = true;
                }
                break;
            case CellSelectionStrategy::MaxSC:
                if (maxSC > comparator.maxSC) {
                    comparator.bestCost = currentCost;
                    update = true;
                }
                break;
        }

        if (update) {
            comparator.minCost = currentCost;
            comparator.maxCost = currentCost;
            comparator.maxSupply = currentSupply;
            comparator.minSupply = currentSupply;
            comparator.maxDemand = currentDemand;
            comparator.minDemand = currentDemand;
            comparator.maxSumTopBottom = sumTopBottom;
            comparator.minSumTopBottom = sumTopBottom;
            comparator.maxShippingAmount = shippingAmount;
            comparator.minShippingAmount = shippingAmount;
            comparator.maxAllocationCost = allocationCost;
            comparator.minAllocationCost = allocationCost;
            comparator.maxSupplyDemandMult = supplyDemandMult;
            comparator.minSupplyDemandMult = supplyDemandMult;
            comparator.maxSC - maxSC;
            comparator.minSC = minSC;
            selectedRowCostIndex = i;
            selectedColCostIndex = colIndex;
            update = false;
        }

        if (currentCost == comparator.minCost) {
            switch (ctbStrat) {
                case CellTieBreakerStrategy::MaxS:
                    if (currentSupply > comparator.maxSupply) {
                        comparator.maxSupply = currentSupply;
                        selectedRowCostIndex = i;
                        selectedColCostIndex = colIndex;
                    }
                    break;

                case CellTieBreakerStrategy::MinS:
                    if (currentSupply < comparator.minSupply) {
                        comparator.minSupply = currentSupply;
                        selectedRowCostIndex = i;
                        selectedColCostIndex = colIndex;
                    }
                    break;

                case CellTieBreakerStrategy::MaxD:
                    if (currentDemand > comparator.maxDemand) {
                        comparator.maxDemand = currentDemand;
                        selectedRowCostIndex = i;
                        selectedColCostIndex = colIndex;
                    }
                    break;

                case CellTieBreakerStrategy::MinD:
                    if (currentDemand < comparator.minDemand) {
                        comparator.minDemand = currentDemand;
                        selectedRowCostIndex = i;
                        selectedColCostIndex = colIndex;
                    }
                    break;

                case CellTieBreakerStrategy::MaxSTB: 
                    if (sumTopBottom > comparator.maxSumTopBottom) {
                        comparator.maxSumTopBottom = sumTopBottom;
                        selectedRowCostIndex = i;
                        selectedColCostIndex = colIndex;
                    }
                    break;

                case CellTieBreakerStrategy::MinSTB: 
                    if (sumTopBottom < comparator.minSumTopBottom) {
                        comparator.minSumTopBottom = sumTopBottom;
                        selectedRowCostIndex = i;
                        selectedColCostIndex = colIndex;
                    }
                    break;

                case CellTieBreakerStrategy::MaxSoD: 
                    if (max(currentSupply, currentDemand) > max(comparator.maxSupply, comparator.maxDemand)) {
                        comparator.maxSupply = currentSupply;
                        comparator.maxDemand = currentDemand;
                        selectedRowCostIndex = i;
                        selectedColCostIndex = colIndex;
                    }
                    break;
            
                case CellTieBreakerStrategy::MinSoD: 
                    if (min(currentSupply, currentDemand) < min(comparator.minSupply, comparator.minDemand)) {
                        comparator.minSupply = currentSupply;
                        comparator.minDemand = currentDemand;
                        selectedRowCostIndex = i;
                        selectedColCostIndex = colIndex;
                    }
                    break;

                case CellTieBreakerStrategy::MaxA:
                    if (shippingAmount > comparator.maxShippingAmount) {
                        comparator.maxShippingAmount = shippingAmount;
                        selectedRowCostIndex = i;
                        selectedColCostIndex = colIndex;
                    }
                    break;
                
                case CellTieBreakerStrategy::MinA:
                    if (shippingAmount < comparator.minShippingAmount) {
                        comparator.maxShippingAmount = shippingAmount;
                        selectedRowCostIndex = i;
                        selectedColCostIndex = colIndex;
                    }
                    break;

                case CellTieBreakerStrategy::MaxCA:
                    if (allocationCost > comparator.maxAllocationCost) {
                        comparator.maxAllocationCost = allocationCost;
                        selectedRowCostIndex = i;
                        selectedColCostIndex = colIndex;
                    }
                    break;
                
                case CellTieBreakerStrategy::MinCA:
                    if (allocationCost < comparator.minAllocationCost) {
                        comparator.maxAllocationCost = allocationCost;
                        selectedRowCostIndex = i;
                        selectedColCostIndex = colIndex;
                    }
                    break;
                
                case CellTieBreakerStrategy::MaxSDM:
                    if (supplyDemandMult > comparator.maxSupplyDemandMult) {
                        comparator.maxSupplyDemandMult = supplyDemandMult;
                        selectedRowCostIndex = i;
                        selectedColCostIndex = colIndex;
                    }
                    break;
                
                case CellTieBreakerStrategy::MinSDM:
                    if (supplyDemandMult < comparator.minSupplyDemandMult) {
                        comparator.maxSupplyDemandMult = supplyDemandMult;
                        selectedRowCostIndex = i;
                        selectedColCostIndex = colIndex;
                    }
                    break;

                case CellTieBreakerStrategy::Index:
                    // no compare
                    break;

                default:
                    throw invalid_argument("Invalid strategy specified!");
            }
        }
    }
}

void selectPenalties(vector<vector<CostCell>> &costMatrix, vector<int>& rowPenalties, vector<int>& colPenalties, vector<int>& selectedRowPenalties, vector<int>& selectedColPenalties, PenaltySelectionStrategy psStrat) {
    int maxRowPenalty, maxColPenalty, maxPenalty;
    int minRowPenalty, minColPenalty, minPenalty;
    int selectedRowIndex, selectedColIndex;
    int secondMaxRowPenalty, secondMaxColPenalty;
    double maxRowAvgPenalty, maxColAvgPenalty;
    double minRowAvgPenalty, minColAvgPenalty;
    double maxRowAvgASTB, maxColAvgASTB;
    double minRowAvgASTB, minColAvgASTB;
    auto validRowPenalties = rowPenalties;
    auto validColPenalties = colPenalties;
    validRowPenalties.erase(remove_if(validRowPenalties.begin(), validRowPenalties.end(), [](int p) { return p < 0; }), validRowPenalties.end());
    validColPenalties.erase(remove_if(validColPenalties.begin(), validColPenalties.end(), [](int p) { return p < 0; }), validColPenalties.end());

    switch (psStrat) {
        case PenaltySelectionStrategy::MaxPenalty:
            maxRowPenalty = *max_element(validRowPenalties.begin(), validRowPenalties.end());
            maxColPenalty = *max_element(validColPenalties.begin(), validColPenalties.end());
            if (maxRowPenalty >= maxColPenalty) selectedRowPenalties.push_back(maxRowPenalty);
            if (maxColPenalty >= maxRowPenalty) selectedColPenalties.push_back(maxColPenalty);
            break;

        case PenaltySelectionStrategy::MinPenalty:
            minRowPenalty = *min_element(validRowPenalties.begin(), validRowPenalties.end());
            minColPenalty = *min_element(validColPenalties.begin(), validColPenalties.end());
            if (minRowPenalty <= minColPenalty) selectedRowPenalties.push_back(minRowPenalty);
            if (minColPenalty <= minRowPenalty) selectedColPenalties.push_back(minColPenalty);
            break;

        case PenaltySelectionStrategy::TwoMax:
            maxRowPenalty = *max_element(validRowPenalties.begin(), validRowPenalties.end());
            maxColPenalty = *max_element(validColPenalties.begin(), validColPenalties.end());
            validRowPenalties.erase(remove(validRowPenalties.begin(), validRowPenalties.end(), maxRowPenalty), validRowPenalties.end());
            validColPenalties.erase(remove(validColPenalties.begin(), validColPenalties.end(), maxColPenalty), validColPenalties.end());
            secondMaxRowPenalty = *max_element(validRowPenalties.begin(), validRowPenalties.end());
            secondMaxColPenalty = *max_element(validColPenalties.begin(), validColPenalties.end());
            selectedRowPenalties.push_back(maxRowPenalty);
            selectedRowPenalties.push_back(secondMaxRowPenalty);
            selectedColPenalties.push_back(maxColPenalty);
            selectedColPenalties.push_back(secondMaxColPenalty);
            break;

        default:
            cout << "Invalid strategy!" << endl;
            return;
    }
}

void tieBreakPenalties(vector<vector<CostCell>> &costMatrix, vector<int>& rowPenalties, vector<int>& colPenalties, vector<int>& modifiedRowPenalties, vector<int>& modifiedColPenalties, PenaltyTieBreakerStrategy ptbStrat) {
    int bestTotalCostRowIndex = -1;
    int bestTotalCostColIndex = -1;
    int currTotalCost = 0;
    int bestTotalCostCol = 0;
    int bestTotalCostRow = 0;

    switch (ptbStrat) {
        case PenaltyTieBreakerStrategy::AllPenalties:
            // nothing
            break;

        case PenaltyTieBreakerStrategy::MaxTC:
            bestTotalCostCol = INT_MIN;
            bestTotalCostRow = INT_MIN;
            for (int rowPenaltyIndex = 0; rowPenaltyIndex < modifiedRowPenalties.size(); ++rowPenaltyIndex) {
                if (modifiedRowPenalties[rowPenaltyIndex] == -1) continue; 
                currTotalCost = 0; 

                for (int j = 0; j < costMatrix[0].size(); ++j) 
                    if (costMatrix[rowPenaltyIndex][j].included) 
                        currTotalCost += costMatrix[rowPenaltyIndex][j].cost;

                if (currTotalCost > bestTotalCostRow) {
                    bestTotalCostRow = currTotalCost;
                    bestTotalCostRowIndex = rowPenaltyIndex;
                }
            }

            for (int colPenaltyIndex = 0; colPenaltyIndex < modifiedColPenalties.size(); ++colPenaltyIndex) {
                if (modifiedColPenalties[colPenaltyIndex] == -1) continue; 
                currTotalCost = 0; 

                for (int i = 0; i < costMatrix.size(); ++i) 
                    if (costMatrix[i][colPenaltyIndex].included) 
                        currTotalCost += costMatrix[i][colPenaltyIndex].cost;

                if (currTotalCost > bestTotalCostCol) {
                    bestTotalCostCol = currTotalCost;
                    bestTotalCostColIndex = colPenaltyIndex;
                }
            }

            if (bestTotalCostRow > bestTotalCostCol) {
                for (int i = 0; i < modifiedRowPenalties.size(); ++i)
                    if (i != bestTotalCostRowIndex) 
                        modifiedRowPenalties[i] = -1; 
                for (int i = 0; i < modifiedColPenalties.size(); ++i) 
                    modifiedColPenalties[i] = -1;  
            } else if (bestTotalCostCol > bestTotalCostRow) {
                for (int i = 0; i < modifiedColPenalties.size(); ++i) 
                    if (i != bestTotalCostColIndex) 
                        modifiedColPenalties[i] = -1;  
                for (int i = 0; i < modifiedRowPenalties.size(); ++i)
                    modifiedRowPenalties[i] = -1; 
            } else {
                for (int i = 0; i < modifiedRowPenalties.size(); ++i) 
                    if (i != bestTotalCostRowIndex) 
                        modifiedRowPenalties[i] = -1; 
                for (int i = 0; i < modifiedColPenalties.size(); ++i) 
                    if (i != bestTotalCostColIndex) 
                        modifiedColPenalties[i] = -1;  
            }
            break;

        case PenaltyTieBreakerStrategy::MinTC:
            bestTotalCostCol = INT_MAX;
            bestTotalCostRow = INT_MAX;
            for (int rowPenaltyIndex = 0; rowPenaltyIndex < modifiedRowPenalties.size(); ++rowPenaltyIndex) {
                if (modifiedRowPenalties[rowPenaltyIndex] == -1) continue;
                currTotalCost = 0;

                for (int j = 0; j < costMatrix[0].size(); ++j) 
                    if (costMatrix[rowPenaltyIndex][j].included) 
                        currTotalCost += costMatrix[rowPenaltyIndex][j].cost;

                if (currTotalCost < bestTotalCostRow || bestTotalCostRow == -1) { 
                    bestTotalCostRow = currTotalCost;
                    bestTotalCostRowIndex = rowPenaltyIndex;
                }
            }

            for (int colPenaltyIndex = 0; colPenaltyIndex < modifiedColPenalties.size(); ++colPenaltyIndex) {
                if (modifiedColPenalties[colPenaltyIndex] == -1) continue;
                currTotalCost = 0;

                for (int i = 0; i < costMatrix.size(); ++i) 
                    if (costMatrix[i][colPenaltyIndex].included) 
                        currTotalCost += costMatrix[i][colPenaltyIndex].cost;

                if (currTotalCost < bestTotalCostCol || bestTotalCostCol == -1) { 
                    bestTotalCostCol = currTotalCost;
                    bestTotalCostColIndex = colPenaltyIndex;
                }
            }

            // cout << "btcr : " << bestTotalCostRow << endl; 
            // cout << "btcc : " << bestTotalCostCol << endl;
            // cout << "btcri : " << bestTotalCostRowIndex << endl; 
            // cout << "btcci : " << bestTotalCostColIndex << endl;
            // cout << "btcrp : " << modifiedRowPenalties[bestTotalCostRowIndex] << endl; 
            // cout << "btccp : " << modifiedColPenalties[bestTotalCostColIndex] << endl;

            if (bestTotalCostRow < bestTotalCostCol) {
                for (int i = 0; i < modifiedRowPenalties.size(); ++i)
                    if (i != bestTotalCostRowIndex) 
                        modifiedRowPenalties[i] = -1;
                for (int i = 0; i < modifiedColPenalties.size(); ++i) 
                    modifiedColPenalties[i] = -1;
            } else if (bestTotalCostCol < bestTotalCostRow) {
                for (int i = 0; i < modifiedColPenalties.size(); ++i) 
                    if (i != bestTotalCostColIndex) 
                        modifiedColPenalties[i] = -1;
                for (int i = 0; i < modifiedRowPenalties.size(); ++i)
                    modifiedRowPenalties[i] = -1;
            } else {
                for (int i = 0; i < modifiedRowPenalties.size(); ++i) 
                    if (i != bestTotalCostRowIndex) 
                        modifiedRowPenalties[i] = -1;
                for (int i = 0; i < modifiedColPenalties.size(); ++i) 
                    if (i != bestTotalCostColIndex) 
                        modifiedColPenalties[i] = -1;
            }
            break;

        default:
            cout << "Invalid strategy!" << endl;
            return;
    } 
} 


void findBestCostIndex(vector<vector<CostCell>> &costMatrix, vector<int> &rowPenalties, vector<int> &colPenalties, vector<int> &supply, vector<int> &demand, int &selectedRowCostIndex, int &selectedColCostIndex, PenaltySelectionStrategy psStrat, PenaltyTieBreakerStrategy ptbStrat, CellSelectionStrategy csStrat, CellTieBreakerStrategy ctbStrat) {
    selectedRowCostIndex = -1; 
    selectedColCostIndex = -1; 
    vector<int> selectedRowPenalties, selectedColPenalties;
    vector<int> modifiedRowPenalties = rowPenalties;
    vector<int> modifiedColPenalties = colPenalties;

    selectPenalties(costMatrix, rowPenalties, colPenalties, selectedRowPenalties, selectedColPenalties, psStrat);

    // cout << "Row Pen: ";
    // for (int i = 0; i < rowPenalties.size(); ++i) 
    //     cout << rowPenalties[i] << " ";
    // cout << endl;
    // cout << "Col Pen: ";
    // for (int i = 0; i < colPenalties.size(); ++i) 
    //     cout << colPenalties[i] << " ";
    // cout << endl;

    // cout << "Selected Row Pen: ";
    // for (int i = 0; i < selectedRowPenalties.size(); ++i) 
    //     cout << selectedRowPenalties[i] << " ";
    // cout << endl;
    // cout << "Selected Col Pen: ";
    // for (int i = 0; i < selectedColPenalties.size(); ++i) 
    //     cout << selectedColPenalties[i] << " ";
    // cout << endl;

    // set element to -1 if not selected
    for (int i = 0; i < modifiedRowPenalties.size(); ++i) 
        if (find(selectedRowPenalties.begin(), selectedRowPenalties.end(), modifiedRowPenalties[i]) == selectedRowPenalties.end()) 
            modifiedRowPenalties[i] = -1; 
    for (int i = 0; i < modifiedColPenalties.size(); ++i) 
        if (find(selectedColPenalties.begin(), selectedColPenalties.end(), modifiedColPenalties[i]) == selectedColPenalties.end()) 
            modifiedColPenalties[i] = -1; 
    
    // cout << "Modified Row Penalties BEFORE: ";
    // for (int i = 0; i < modifiedRowPenalties.size(); ++i) 
    //     cout << modifiedRowPenalties[i] << " ";
    // cout << endl;
    // cout << "Modified Col Penalties BEFORE: ";
    // for (int i = 0; i < modifiedColPenalties.size(); ++i) 
    //     cout << modifiedColPenalties[i] << " ";
    // cout << endl;

    tieBreakPenalties(costMatrix, rowPenalties, colPenalties, modifiedRowPenalties, modifiedColPenalties, ptbStrat);

    // cout << "Modified Row Penalties AFTER: ";
    // for (int i = 0; i < modifiedRowPenalties.size(); ++i) 
    //     cout << modifiedRowPenalties[i] << " ";
    // cout << endl;
    // cout << "Modified Col Penalties AFTER: ";
    // for (int i = 0; i < modifiedColPenalties.size(); ++i) 
    //     cout << modifiedColPenalties[i] << " ";
    // cout << endl;

    ComparatorValues comparator;
    for (int rowIndex = 0; rowIndex < modifiedRowPenalties.size(); rowIndex++) 
        if (modifiedRowPenalties[rowIndex] >= 0) 
            checkRow(costMatrix, supply, demand, rowIndex, comparator, selectedRowCostIndex, selectedColCostIndex, csStrat, ctbStrat);
    for (int colIndex = 0; colIndex < modifiedColPenalties.size(); ++colIndex) 
        if (modifiedColPenalties[colIndex] >= 0) 
            checkColumn(costMatrix, supply, demand, colIndex, comparator, selectedRowCostIndex, selectedColCostIndex, csStrat, ctbStrat);

    {
        bool allPenaltiesMinusOne = true;
        for (int penalty : rowPenalties) {
            if (penalty != -1) {
                allPenaltiesMinusOne = false;
                break;
            }
        }
        for (int penalty : colPenalties) {
            if (penalty != -1) {
                allPenaltiesMinusOne = false;
                break;
            }
        }

        // If all penalties are -1, find the last included cell
        if (allPenaltiesMinusOne) {
            for (int i = costMatrix.size() - 1; i >= 0; --i) {
                for (int j = costMatrix[i].size() - 1; j >= 0; --j) {
                    if (costMatrix[i][j].included) {
                        selectedRowCostIndex = i;
                        selectedColCostIndex = j;
                        return; 
                    }
                }
            }
        }
    }
}

void calculateAndStoreShippingAmount(vector<int> &supply, vector<int> &demand, vector<vector<int>> &supplyDemandMatrix, int rowIndex, int colIndex) {
    // cout << "row: "<< rowIndex+1 << " col: " << colIndex+1 << endl;
    int availableSupply = supply[rowIndex]; 
    int requiredDemand = demand[colIndex];  

    int maxShippingAmount = min(availableSupply, requiredDemand);
    supplyDemandMatrix[rowIndex][colIndex] += maxShippingAmount;

    supply[rowIndex] -= maxShippingAmount; 
    demand[colIndex] -= maxShippingAmount;
}

void evaluateSupplyAndDemand(const vector<int> &supply, const vector<int> &demand, vector<vector<CostCell>> &costMatrix) {
    for (int i = 0; i < supply.size(); ++i) {
        for (int j = 0; j < demand.size(); ++j) {
            if (supply[i] == 0 || demand[j] == 0) {
                costMatrix[i][j].included = false;
            }
        }
    }
}

int calculateTotalCost(const vector<vector<CostCell>> &costMatrix, const vector<vector<int>> &supplyDemandMatrix) {
    int totalCost = 0;

    for (int i = 0; i < costMatrix.size(); ++i) {
        for (int j = 0; j < costMatrix[i].size(); ++j) {
            totalCost += supplyDemandMatrix[i][j] * costMatrix[i][j].cost;
        }
    }

    return totalCost;
}

void processFile(const fs::path &filePath, vector<Result> &results, PenaltyCalculationStrategy pcStrat, PenaltySelectionStrategy psStrat, PenaltyTieBreakerStrategy ptbStrat, CellSelectionStrategy csStrat, CellTieBreakerStrategy ctbStrat) {
    ifstream inputFile(filePath);
    
    if (!inputFile.is_open()) {
        cerr << "Error opening file: " << filePath << endl;
        return;
    }

    int numSources, numDestinations;
    int maxRowPenIndex, maxColPenIndex;
    int bestRowIndex, bestColIndex;
    int optimalSolution;
    vector<vector<CostCell>> costMatrix;
    vector<vector<int>> supplyDemandMatrix; 
    vector<int> supply;
    vector<int> demand;
    vector<int> rowPenalties;
    vector<int> colPenalties;

    inputVar(inputFile, numSources, numDestinations, costMatrix, supply, demand, optimalSolution);
    inputFile.close();

    calculateTopBottom(costMatrix);
    supplyDemandMatrix = initializeSupplyDemandMatrix(numSources, numDestinations);

    // Main loop
    while (true) {
        calculatePenalties(costMatrix, rowPenalties, colPenalties, pcStrat);
        findBestCostIndex(costMatrix, rowPenalties, colPenalties, supply, demand, bestRowIndex, bestColIndex, psStrat, ptbStrat, csStrat, ctbStrat);

        if (bestRowIndex == -1 || bestColIndex == -1) {
            break;
        }

        calculateAndStoreShippingAmount(supply, demand, supplyDemandMatrix, bestRowIndex, bestColIndex);
        evaluateSupplyAndDemand(supply, demand, costMatrix);
    }

    int result = calculateTotalCost(costMatrix, supplyDemandMatrix);
    bool isOptimal = (result == optimalSolution); 
    
    Result resultObj;
    resultObj.solution = result;
    resultObj.optimalSolution = optimalSolution;
    resultObj.isOptimal = isOptimal;
    resultObj.strategyUsed = ctbStratToString(ctbStrat);
    
    results.push_back(resultObj);
}

// PROCESS // // PROCESS // // PROCESS // // PROCESS // // PROCESS // // PROCESS // // PROCESS // // PROCESS // 
// PROCESS // // PROCESS // // PROCESS // // PROCESS // // PROCESS // // PROCESS // // PROCESS // // PROCESS // 