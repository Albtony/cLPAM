#pragma once

#include <limits>
#include <string>
#include <cstdint>
#include <vector>
#include <fstream>
#include <iostream>
#include <unordered_map>
#include <set>

struct Cell {
    std::unordered_map<std::string, int> attributes;
    bool included = true; 
};

struct RowCol {
    std::unordered_map<std::string, int> attributes;
};

namespace PenaltyStrategy {
    enum class Calculation : uint8_t { TwoMin, TwoMax, MinMax };
    enum class Selection : uint8_t { MaxPenalty, MinPenalty, TwoMax };
    enum class TieBreaker : uint8_t { AllPenalties, MaxTC, MinTC };

    inline std::string toString(Calculation strategy) {
        switch (strategy) {
            case Calculation::TwoMin: return "TwoMin";
            case Calculation::TwoMax: return "TwoMax";
            case Calculation::MinMax: return "MinMax";
            default: return "Unknown";
        }
    }

    inline std::string toString(Selection strategy) {
        switch (strategy) {
            case Selection::MaxPenalty: return "MaxPenalty";
            case Selection::MinPenalty: return "MinPenalty";
            case Selection::TwoMax: return "TwoMax";
            default: return "Unknown";
        }
    }

    inline std::string toString(TieBreaker strategy) {
        switch (strategy) {
            case TieBreaker::AllPenalties: return "AllPenalties";
            case TieBreaker::MaxTC: return "MaxTC";
            case TieBreaker::MinTC: return "MinTC";
            default: return "Unknown";
        }
    }
}

namespace CellStrategy {
    enum class Selection : uint8_t { MinCost, MinCA, MinSC, MaxSC };
    enum class TieBreaker : uint8_t {
        Index, MaxS, MinS, MaxD, MinD, MaxSoD, MinSoD, 
        MaxSTB, MinSTB, MaxA, MinA, MaxCA, MinCA, MaxSDM, MinSDM
    };

    inline std::string toString(Selection strategy) {
        switch (strategy) {
            case Selection::MinCost: return "MinCost";
            case Selection::MinCA: return "MinCA";
            case Selection::MinSC: return "MinSC";
            case Selection::MaxSC: return "MaxSC";
            default: return "Unknown";
        }
    }

    inline std::string toString(TieBreaker strategy) {
        switch (strategy) {
            case TieBreaker::Index: return "Index";
            case TieBreaker::MaxS: return "MaxS";
            case TieBreaker::MinS: return "MinS";
            case TieBreaker::MaxD: return "MaxD";
            case TieBreaker::MinD: return "MinD";
            case TieBreaker::MaxSoD: return "MaxSoD";
            case TieBreaker::MinSoD: return "MinSoD";
            case TieBreaker::MaxSTB: return "MaxSTB";
            case TieBreaker::MinSTB: return "MinSTB";
            case TieBreaker::MaxA: return "MaxA";
            case TieBreaker::MinA: return "MinA";
            case TieBreaker::MaxCA: return "MaxCA";
            case TieBreaker::MinCA: return "MinCA";
            case TieBreaker::MaxSDM: return "MaxSDM";
            case TieBreaker::MinSDM: return "MinSDM";
            default: return "Unknown";
        }
    }
}

struct Result {
    std::string strategyCombination;
    int solution;
    int optimalSolution;
    bool isOptimal() const { return solution == optimalSolution; } 
};

struct StrategyResult {
    std::string strategyCombination; 
    double correctnessPercentage;  
    double nearOptimalityPercentage; 
};

class TransportationProblem {
    public:
        std::vector<std::vector<Cell>> costMatrix;
        std::vector<std::vector<int>> allocationMatrix;
        std::vector<int> supplies, demands;
        std::vector<int> initSupplies, initDemands;
        std::vector<RowCol> rows, cols;
        std::vector<int> selectedPenalties;                                         // from selectPenalties
        std::vector<int> bestRowPenaltyIndices, bestColPenaltyIndices;              // from tieBreakPenalties
        std::vector<Cell> selectedCells;                                            // from selectCells
        Cell bestCell;                                                              // from tieBreakCell 
        int numSources, numDestinations;
        int numRow, numCol;
        int optimalSolution;
        int totalCost;

        // initialization
        void initialize(std::ifstream &inputFile);
        void calculateAttributes();
        void calculateTopBottom();
        void calculateCostAllocation();
        void calculateRowColTotalCost();
        void calculateSupremeCell();

        // computation
        void calculatePenalties(PenaltyStrategy::Calculation strategy);
        void selectPenalties(PenaltyStrategy::Selection strategy);
        void tieBreakPenalties(PenaltyStrategy::TieBreaker strategy);
        void selectCells(CellStrategy::Selection strategy);
        void tieBreakCells(CellStrategy::TieBreaker strategy);
        void allocateUnits();
        void updateSupplyDemand();
        bool isSolved();

        // result processing
        void calculateTotalCost();

        // util
        int calculatePenalty(const std::set<int>& sortedValues, PenaltyStrategy::Calculation strategy); 
};