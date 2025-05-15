bool compareFileNames(const fs::path &a, const fs::path &b);
bool compareByAccuracy(const StrategyResult& a, const StrategyResult& b)
bool compareByErrorRelativeAccuracy(const StrategyResult& a, const StrategyResult& b)
string pcStratToString(PenaltyCalculationStrategy strategy)
string psStratToString(PenaltySelectionStrategy strategy)
string ptbStratToString(PenaltyTieBreakerStrategy strategy)
string csStratToString(CellSelectionStrategy strategy)
string ctbStratToString(CellTieBreakerStrategy strategy)
void inputVar(ifstream &inputFile, int &numSources, int &numDestinations, vector<vector<CostCell>> &costMatrix, vector<int> &supply, vector<int> &demand, int &optimalSolution)
vector<vector<int>> initializeSupplyDemandMatrix(int numSources, int numDestinations)
void printProblemStatement(int numSources, int numDestinations, const vector<vector<CostCell>> &costMatrix, const vector<int> &supply, const vector<int> &demand, PenaltyCalculationStrategy pcStrat, PenaltySelectionStrategy psStrat, PenaltyTieBreakerStrategy ptbStrat, CellTieBreakerStrategy ctbStrat)
void printVector(const vector<int> &vec)
void printState(const vector<int> &supply, const vector<int> &demand, const vector<int> &initSupply, const vector<int> &initDemand, const vector<vector<int>> &supplyDemandMatrix, int bestRowIndex, int bestColIndex) 
void printCostMatrix(const vector<vector<CostCell>> &costMatrix, const vector<int> &rowPenalties, const vector<int> &colPenalties) 
void printResults(const vector<Result>& results) 

// UTIL // // UTIL // // UTIL // // UTIL // // UTIL // // UTIL // // UTIL // // UTIL // // UTIL // // UTIL // 
// UTIL // // UTIL // // UTIL // // UTIL // // UTIL // // UTIL // // UTIL // // UTIL // // UTIL // // UTIL // 

bool compareFileNames(const fs::path &a, const fs::path &b) {
    auto getNumber = [](const fs::path &file) -> int {
        string filename = file.stem().string();
        string numberStr;
        for (char c : filename) {
            if (isdigit(c)) numberStr += c;
        }
        return numberStr.empty() ? 0 : stoi(numberStr);
    };
    return getNumber(a) < getNumber(b);
}

bool compareByAccuracy(const StrategyResult& a, const StrategyResult& b) {
    return a.accuracy < b.accuracy; 
}

bool compareByErrorRelativeAccuracy(const StrategyResult& a, const StrategyResult& b) {
    return a.errorAccuracy < b.errorAccuracy;
}

string pcStratToString(PenaltyCalculationStrategy strategy) {
    switch (strategy) {
        case PenaltyCalculationStrategy::MinMax:        return "Min Max";
        case PenaltyCalculationStrategy::TwoMax:        return "Two Max";
        case PenaltyCalculationStrategy::TwoMin:        return "Two Min";     
        default:                                        return "Unknown Strategy";
    }
}

string psStratToString(PenaltySelectionStrategy strategy) {
    switch (strategy) {
        case PenaltySelectionStrategy::MaxPenalty:      return "Max Penalty";    
        case PenaltySelectionStrategy::MinPenalty:      return "Min Penalty";
        case PenaltySelectionStrategy::TwoMax:          return "Two Max";
        default:                                        return "Unknown Strategy";
    }
}

string ptbStratToString(PenaltyTieBreakerStrategy strategy) {
    switch (strategy) {
        case PenaltyTieBreakerStrategy::AllPenalties:   return "All Penalties";
        case PenaltyTieBreakerStrategy::MaxTC:          return "Max Total Cost";
        case PenaltyTieBreakerStrategy::MinTC:          return "Min Total Cost";
        default:                                        return "Unknown Strategy";
    }
}

string csStratToString(CellSelectionStrategy strategy) {
    switch (strategy) {
        case CellSelectionStrategy::MaxSC:              return "MaxSC";     
        case CellSelectionStrategy::MinSC:              return "MinSC";     
        case CellSelectionStrategy::MinCA:              return "MinCA";     
        case CellSelectionStrategy::MinCost:            return "MinCost";     
        default:                                        return "Unknown Strategy";
    }
}

string ctbStratToString(CellTieBreakerStrategy strategy) {
    switch (strategy) {
        case CellTieBreakerStrategy::MaxS:      return "Max Supply";
        case CellTieBreakerStrategy::MinS:      return "Min Supply";
        case CellTieBreakerStrategy::MaxD:      return "Max Demand";
        case CellTieBreakerStrategy::MinD:      return "Min Demand";
        case CellTieBreakerStrategy::MaxSoD:    return "Max Supply and Demand";
        case CellTieBreakerStrategy::MinSoD:    return "Min Supply and Demand";
        case CellTieBreakerStrategy::MaxSTB:    return "Max Top-Bottom Sum";
        case CellTieBreakerStrategy::MinSTB:    return "Min Top-Bottom Sum";
        case CellTieBreakerStrategy::MaxA:      return "Max Allocation";
        case CellTieBreakerStrategy::MinA:      return "Min Allocation";
        case CellTieBreakerStrategy::MaxCA:     return "Max Cost Allocation";
        case CellTieBreakerStrategy::MinCA:     return "Min Cost Allocation";
        case CellTieBreakerStrategy::MaxSDM:    return "Max Supply Demand Mult";
        case CellTieBreakerStrategy::MinSDM:    return "Min Supply Demand Mult";
        case CellTieBreakerStrategy::Index:     return "Index";
        default:                                return "Unknown Strategy";
    }
}

void inputVar(ifstream &inputFile, int &numSources, int &numDestinations, vector<vector<CostCell>> &costMatrix, vector<int> &supply, vector<int> &demand, int &optimalSolution) {
    inputFile >> numSources >> numDestinations;
    costMatrix.resize(numSources, vector<CostCell>(numDestinations));

    for (int i = 0; i < numSources; ++i) {
        for (int j = 0; j < numDestinations; ++j) {
            inputFile >> costMatrix[i][j].cost;
        }
    }

    supply.resize(numSources);
    for (int i = 0; i < numSources; ++i) {
        inputFile >> supply[i];
    }
    
    demand.resize(numDestinations);
    for (int i = 0; i < numDestinations; ++i) {
        inputFile >> demand[i];
    }

    inputFile >> optimalSolution;
}

vector<vector<int>> initializeSupplyDemandMatrix(int numSources, int numDestinations) {
    return vector<vector<int>>(numSources, vector<int>(numDestinations, 0));
}

// UTIL // // UTIL // // UTIL // // UTIL // // UTIL // // UTIL // // UTIL // // UTIL // // UTIL // // UTIL // 
// UTIL // // UTIL // // UTIL // // UTIL // // UTIL // // UTIL // // UTIL // // UTIL // // UTIL // // UTIL // 

// PRINT // // PRINT // // PRINT // // PRINT // // PRINT // // PRINT // // PRINT // // PRINT // // PRINT // 
// PRINT // // PRINT // // PRINT // // PRINT // // PRINT // // PRINT // // PRINT // // PRINT // // PRINT // 

void printProblemStatement(int numSources, int numDestinations, const vector<vector<CostCell>> &costMatrix, const vector<int> &supply, const vector<int> &demand, PenaltyCalculationStrategy pcStrat, PenaltySelectionStrategy psStrat, PenaltyTieBreakerStrategy ptbStrat, CellTieBreakerStrategy ctbStrat) {
    cout << "================================== PROBLEM STATEMENT ==================================" << endl;
    cout << "Penalty Calculation: " << pcStratToString(pcStrat) << endl;
    cout << "Penalty Selection: " << psStratToString(psStrat) << endl;
    cout << "Penalty Tie Breaker: " << ptbStratToString(ptbStrat) << endl;
    cout << "Cell Tie Breaker: " << ctbStratToString(ctbStrat) << endl;
    cout << "Cost Matrix (with top and bottom values):" << endl;
    for (int i = 0; i < numSources; ++i) {
        for (int j = 0; j < numDestinations; ++j) {
            cout << "[" << costMatrix[i][j].cost << ", " << costMatrix[i][j].top << ", " << costMatrix[i][j].bottom << "] ";
        }
        cout << endl;
    }
    cout << endl;

    cout << "Supply: ";
    for (int s : supply) cout << s << " ";
    cout << endl;

    cout << "demand: ";
    for (int d : demand) cout << d << " ";
    cout << endl;
    cout << "================================== PROBLEM STATEMENT ==================================" << endl;
}

void printVector(const vector<int> &vec) {
    for (const int &value : vec) {
        cout << value << " ";
    }
    cout << endl;
}

void printState(const vector<int> &supply, const vector<int> &demand, const vector<int> &initSupply, const vector<int> &initDemand, const vector<vector<int>> &supplyDemandMatrix, int bestRowIndex, int bestColIndex) {
    cout << "Cell yang terpilih adalah: (" << bestColIndex + 1 << ", " << bestRowIndex + 1 << ")" << endl << endl;
    cout << "Alokasi unit pada matriks\n";
       for (size_t i = 0; i < supplyDemandMatrix.size(); ++i) {
        for (size_t j = 0; j < supplyDemandMatrix[i].size(); ++j) {
            if (i == bestRowIndex && j == bestColIndex) {
                cout << "\033[32m" << supplyDemandMatrix[i][j] << "\033[0m\t"; 
            } else {
                cout << supplyDemandMatrix[i][j] << "\t";
            }
        }
        cout << endl;
    }

    cout << "Supply:\t";
    for (size_t i = 0; i < supply.size(); ++i) {
        if (i == bestRowIndex) {
            cout << "\033[32m" << supply[i] << "\033[0m ";
        } else {
            cout << supply[i] << " ";
        }
        cout << "(" << initSupply[i] << ")" << "\t";
    }
    cout << endl;

    cout << "Demand:\t";
    for (size_t i = 0; i < demand.size(); ++i) {
        if (i == bestColIndex) {
            cout << "\033[32m" << demand[i] << "\033[0m ";
        } else {
            cout << demand[i] << " ";
        }
        cout << "(" << initDemand[i] << ")" << "\t";
    }
    cout << endl << endl;
}


void printCostMatrix(const vector<vector<CostCell>> &costMatrix, 
                     const vector<int> &rowPenalties, 
                     const vector<int> &colPenalties) {
    cout << "Cost Matrix:\n";
    
    for (size_t i = 0; i < costMatrix.size(); ++i) {
        for (size_t j = 0; j < costMatrix[i].size(); ++j) {
            const auto &cell = costMatrix[i][j];
            if (cell.included) {
                cout << "\033[32m[" << cell.cost << ", " << cell.top << ", " << cell.bottom << "]\033[0m ";
            } else {
                cout << "\033[31m[" << cell.cost << ", " << cell.top << ", " << cell.bottom << "]\033[0m ";
            }
        }
        cout << " " << rowPenalties[i];
        cout << endl;
    }

    for (size_t j = 0; j < colPenalties.size(); ++j) {
        cout << " " << colPenalties[j] << "        ";  
    }
    cout << endl << endl;
}

void printResults(const vector<Result>& results) {
    for (int i = 0; i < results.size(); ++i) {
        if (results[i].isOptimal) {
            if (i == 5 || i == 7 || i == 10 || i == 11 || i == 16 || i == 17 || i == 19) cout << "\033[33m";
            else cout << "\033[32m"; 
        } else {
            cout << "\033[31m"; 
        }
        cout << setw(10) << left << results[i].solution << "\033[0m" << setw(10) << left ;
        // cout << results[i].optimalSolution;
        if (results[i].solution < results[i].optimalSolution) cout << setw(10) << left << " Error detected!";
        cout << endl;
    }
}
