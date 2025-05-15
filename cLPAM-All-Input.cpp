/*LPAM Technique*/

#include "header.h"
using namespace std;

int main() {
    string inputDirectory = "./input";
    vector<fs::path> files;
    vector<Result> results;
    PenaltyCalculationStrategy currPcStrat = PenaltyCalculationStrategy::TwoMin;
    PenaltySelectionStrategy currPsStrat = PenaltySelectionStrategy::MaxPenalty;
    PenaltyTieBreakerStrategy currPtbStrat = PenaltyTieBreakerStrategy::AllPenalties;
    CellSelectionStrategy currCsStrat = CellSelectionStrategy::MinCost;
    CellTieBreakerStrategy currCtbStrat = CellTieBreakerStrategy::MinS;
    

    for (const auto &entry : fs::directory_iterator(inputDirectory)) {
        if (entry.is_regular_file()) {
            files.push_back(entry.path());
        }
    }

    sort(files.begin(), files.end(), compareFileNames);
    for (const auto &filePath : files) processFile(filePath, results, currPcStrat, currPsStrat, currPtbStrat, currCsStrat, currCtbStrat);

    printResults(results);

    return 0;
}