#include <limit>
#include <string>

struct CostCell {
    int cost;
    int top;      
    int bottom;   
    bool included = true;
};

struct ComparatorValues {
    int bestCost;
    int minCost;
    int maxCost;
    int minDemand;
    int maxDemand;
    int minSupply;
    int maxSupply;
    int minSumTopBottom;
    int maxSumTopBottom;
    int minShippingAmount; 
    int maxShippingAmount;
    int minAllocationCost;
    int maxAllocationCost;
    int minSupplyDemandMult;
    int maxSupplyDemandMult;
    int maxSC;
    int minSC;

    ComparatorValues() 
        : bestCost(-1),
          minCost(std::numeric_limits<int>::max()), 
          maxCost(std::numeric_limits<int>::min()), 
          minDemand(std::numeric_limits<int>::max()), 
          maxDemand(std::numeric_limits<int>::min()), 
          minSupply(std::numeric_limits<int>::max()), 
          maxSupply(std::numeric_limits<int>::min()),
          minSumTopBottom(std::numeric_limits<int>::max()),
          maxSumTopBottom(std::numeric_limits<int>::min()),
          minShippingAmount(std::numeric_limits<int>::max()),
          maxShippingAmount(std::numeric_limits<int>::min()),
          minAllocationCost(std::numeric_limits<int>::max()),
          maxAllocationCost(std::numeric_limits<int>::min()),
          minSupplyDemandMult(std::numeric_limits<int>::max()),
          maxSupplyDemandMult(std::numeric_limits<int>::min()),
          minSC(std::numeric_limits<int>::max()),
          maxSC(std::numeric_limits<int>::min()) {}
};

enum class PenaltyCalculationStrategy {
    TwoMin,
    TwoMax,
    MinMax
};

enum class PenaltySelectionStrategy {
    MaxPenalty, 
    MinPenalty,
    TwoMax  
};

enum class PenaltyTieBreakerStrategy {
    AllPenalties,
    MaxTC,
    MinTC
};

enum class CellSelectionStrategy {
    MinCost,
    MinCA,
    MinSC,
    MaxSC
};

enum class CellTieBreakerStrategy {
    Index,
    MaxS,
    MinS,
    MaxD,
    MinD,
    MaxSoD,
    MinSoD,
    MaxSTB,
    MinSTB,
    MaxA,
    MinA,
    MaxCA,
    MinCA,
    MaxSDM,
    MinSDM
};

struct Result {
    int solution;
    int optimalSolution;
    bool isOptimal;
    std::string strategyUsed;
};

struct StrategyResult {
    string strategyCombination;
    int accuracy;
    int errorAccuracy;
    bool isValid;
};