/*LPAM Technique*/

#include "header.h"
using namespace std;

void exportToCSV(vector<StrategyResult> &strategyResults, const string &filename) {
    ofstream file(filename);

    sort(strategyResults.begin(), strategyResults.end(), [](const StrategyResult &a, const StrategyResult &b) {
        return a.accuracy > b.accuracy; 
    });

    // Write the CSV header
    file << "No,Penalty Calculation Strategy,Penalty Selection Strategy,Penalty TieBreaker Strategy,"
         << "Cell Selection Strategy,Cell TieBreaker Strategy,Akurasi (Normal)\n";
    int i = 1;
    
    // Write the data
    for (const auto &strategyResult : strategyResults) {
        stringstream ss(strategyResult.strategyCombination);
        string pcStrat, psStrat, ptbStrat, csStrat, ctbStrat;

        // Parse strategyCombination using a comma and space as delimiters
        getline(ss, pcStrat, ',');
        ss.ignore(1); 
        getline(ss, psStrat, ',');
        ss.ignore(1);
        getline(ss, ptbStrat, ',');
        ss.ignore(1);
        getline(ss, csStrat, ',');
        ss.ignore(1);
        getline(ss, ctbStrat);

        // Write the parsed fields and accuracy to the CSV
        file << i++ << ","
             << pcStrat << ","
             << psStrat << ","
             << ptbStrat << ","
             << csStrat << ","
             << ctbStrat << ","
             << strategyResult.accuracy << "\n";
    }

    file.close();
    cout << "CSV file exported to: " << filename << endl;
}

int main() {
    string inputDirectory = "./input";
    string sortBy = "overall";
    vector<fs::path> files;
    vector<StrategyResult> strategyResults;

    for (const auto &entry : fs::directory_iterator(inputDirectory)) {
        if (entry.is_regular_file()) {
            files.push_back(entry.path());
        }
    }

    sort(files.begin(), files.end(), compareFileNames);

    cout << "working on it...\n";

    for (int ptbStrat = static_cast<int>(PenaltyTieBreakerStrategy::AllPenalties); 
        ptbStrat <= static_cast<int>(PenaltyTieBreakerStrategy::MinTC); ++ptbStrat) {
        for (int pcStrat = static_cast<int>(PenaltyCalculationStrategy::TwoMin); 
            pcStrat <= static_cast<int>(PenaltyCalculationStrategy::MinMax); ++pcStrat) {
            for (int psStrat = static_cast<int>(PenaltySelectionStrategy::MaxPenalty); 
                psStrat <= static_cast<int>(PenaltySelectionStrategy::TwoMax); ++psStrat) {
                for (int csStrat = static_cast<int>(CellSelectionStrategy::MinCost); 
                    csStrat <= static_cast<int>(CellSelectionStrategy::MaxSC); ++csStrat) {  
                    for (int ctbStrat = static_cast<int>(CellTieBreakerStrategy::Index); 
                        ctbStrat <= static_cast<int>(CellTieBreakerStrategy::MinSDM); ++ctbStrat) {

                        vector<Result> results;
                        PenaltyTieBreakerStrategy currPtbStrat = static_cast<PenaltyTieBreakerStrategy>(ptbStrat);
                        PenaltyCalculationStrategy currPcStrat = static_cast<PenaltyCalculationStrategy>(pcStrat);
                        PenaltySelectionStrategy currPsStrat = static_cast<PenaltySelectionStrategy>(psStrat);
                        CellSelectionStrategy currCsStrat = static_cast<CellSelectionStrategy>(csStrat);
                        CellTieBreakerStrategy currCtbStrat = static_cast<CellTieBreakerStrategy>(ctbStrat);
                        
                        string strategyCombination = 
                            pcStratToString(currPcStrat) + ", " +
                            psStratToString(currPsStrat) + ", " +
                            ptbStratToString(currPtbStrat) + ", " + 
                            csStratToString(currCsStrat) + ", " +
                            ctbStratToString(currCtbStrat); 

                        for (const auto &filePath : files) {
                            processFile(filePath, results, currPcStrat, currPsStrat, currPtbStrat, currCsStrat, currCtbStrat);
                        }

                        int optimalCount = 0;
                        int errorOptimalCount = 0;
                        for (int i = 0; i < results.size(); ++i) {
                            if (results[i].isOptimal) ++optimalCount;
                            if ((i == 5 || i == 7 || i == 10 || i == 11 || i == 16 || i == 17 || i == 19) && results[i].isOptimal) ++errorOptimalCount;
                        }

                        int errorAccuracy = errorOptimalCount * 5;
                        int accuracy = optimalCount * 5;
                        bool isValid = true;
                        strategyResults.push_back({strategyCombination, accuracy, errorAccuracy, isValid});
                        for (int i = 0; i < results.size(); ++i) {
                            if (results[i].solution < results[i].optimalSolution) {
                                strategyResults.back().isValid = false;
                                break;
                            }
                        }
                    }
                }
            }
        }
    }

    exportToCSV(strategyResults, "strategy_results.csv");

    return 0;
}

