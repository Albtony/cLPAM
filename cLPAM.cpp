/*LPAM Technique*/

#include "header.h"

int main() {
    ifstream inputFile("./input/input1.txt");
    
    if (!inputFile.is_open()) {
        cerr << "Error opening file!" << endl;
        return 1;
    }

    int numSources, numDestinations;
    int maxRowPenIndex, maxColPenIndex;
    int bestRowIndex, bestColIndex;
    int optimalSolution;
    vector<vector<CostCell>> costMatrix;
    vector<vector<int>> supplyDemandMatrix; 
    vector<int> supply;
    vector<int> demand;
    vector<int> initSupply;
    vector<int> initDemand;
    vector<int> rowPenalties;
    vector<int> colPenalties;
    vector<Result> results;
    PenaltyCalculationStrategy currPcStrat = PenaltyCalculationStrategy::TwoMin;
    PenaltySelectionStrategy currPsStrat = PenaltySelectionStrategy::MaxPenalty;
    PenaltyTieBreakerStrategy currPtbStrat = PenaltyTieBreakerStrategy::AllPenalties;
    CellSelectionStrategy currCsStrat = CellSelectionStrategy::MinCost;
    CellTieBreakerStrategy currCtbStrat = CellTieBreakerStrategy::MinS;

    inputVar(inputFile, numSources, numDestinations, costMatrix, supply, demand, optimalSolution);
    inputFile.close();

    initSupply = supply;
    initDemand = demand;

    calculateTopBottom(costMatrix);
    printProblemStatement(numSources, numDestinations, costMatrix, supply, demand, currPcStrat, currPsStrat, currPtbStrat, currCtbStrat);
    supplyDemandMatrix = initializeSupplyDemandMatrix(numSources, numDestinations);

    // Main loop
    while (true) {
        // Calculate Penalties and find best cell
        calculatePenalties(costMatrix, rowPenalties, colPenalties, currPcStrat);

        findBestCostIndex(costMatrix, rowPenalties, colPenalties, supply, demand, bestRowIndex, bestColIndex, currPsStrat, currPtbStrat, currCsStrat, currCtbStrat);
        // Check if valid best indices were found
        if (bestRowIndex == -1 || bestColIndex == -1) {
            break; 
        }

        // Calculate and store the shipping amount
        calculateAndStoreShippingAmount(supply, demand, supplyDemandMatrix, bestRowIndex, bestColIndex);
        evaluateSupplyAndDemand(supply, demand, costMatrix);

        // Print the current state
        printCostMatrix(costMatrix, rowPenalties, colPenalties);
        printState(supply, demand, initSupply, initDemand, supplyDemandMatrix, bestRowIndex, bestColIndex);
    }

    int result = calculateTotalCost(costMatrix, supplyDemandMatrix);
    cout << "Total cost dari pengiriman adalah: " << result << endl;

    if(result == optimalSolution) 
        cout << "\033[32mHasil sudah optimal:\033[0m ";
    else 
        cout << "\033[31mHasil belum optimal:\033[0m ";
    cout << optimalSolution << endl;

    return 0;
}