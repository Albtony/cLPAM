## Overview  
<p align="justify">  
cLPAM (Customized LPAM) is a modified version of the LPAM algorithm designed to find an initial basic feasible solution (IBFS) for <b>balanced</b> transportation problems. This project explores modifications to LPAM's set of rules to improve solution quality.  
</p>  

This project includes three programs, each serving a different purpose:
- cLPAM: Uses one strategy to solve one test case, with detailed printed output
- cLPAM-All-Input: Uses one strategy to solve 20 test cases, without detailed output
- cLPAM-All-Strategy: Uses all strategies to solve 20 test cases, with results sorted

## How It Works  
<p align="justify">  
Transportation problems involve optimizing the distribution of resources from multiple suppliers to multiple consumers while minimizing cost. CLPAM modifies LPAM by redefining key strategy points, altering its core mechanism. Various configurations are tested, and the best one is selected based on discrete accuracy and near-optimality percentage.  

The accuracy is determined by comparing the IBFS solution with the known optimal solution produced by the TORA system, which obtains the optimal solution using the Stepping Stone or MODI method (computationally expensive).
</p>  

<p align="center">  
  <img src="https://github.com/user-attachments/assets/a5c16d15-33db-4c70-84d3-233300eee99f" alt="CLPAM Visualization">  
</p>  

## Key Points
- 1620 configuration combinations
- Evaluated to 20 academic test cases
- Best configuration: TwoMin, MaxPenalty, AllPenalties, MinCost, MinSTB
- Achieved 65% accuracy in finding the optimal IBFS, with a 99.05% near-optimality percentage

## Installation & Usage 
### Prerequisites
- C++ Compiler
- Standard C++ Libraries (bits/stdc++)
- ANSI supported terminal (ANSI escape codes are used for coloring)
### Usage
- Place input files in the ./input/ directory in .txt
- Compile and run the program using the following command:
```
g++ "cLPAM.cpp" -o "cLPAM.exe" && .\cLPAM.exe
```

## Example Input Output (cLPAM.cpp)
### Input
format 
```
[# of supply] [# of demand]

[cost] .... [cost]
 ....  ....  ....
[cost] .... [cost]

[supplies]
[demands]

[optimal solution]
```
example
```
3 3

6 4 1 
3 8 7
4 4 2

50 40 60
20 95 35

555
```

### Output
format
```
Alokasi unit pada matriks
[alloc]  ....  [alloc]
 .....   ....   ....
[alloc]  ....  [alloc]
Supply: current supply (initial supply)  ...  current supply (initial supply)
Demand: current demand (initial demand)  ...  current demand (initial demand)

Total cost dari pengiriman adalah: [total cost]
Hasil sudah optimal: [optimal cost]
```
example
```
Alokasi unit pada matriks
0       15      35
20      20      0
0       60      0
Supply: 0 (50)  0 (40)  0 (60)
Demand: 0 (20)  0 (95)  0 (35)

Total cost dari pengiriman adalah: 555
Hasil sudah optimal: 555
```

## Reference
https://repository.its.ac.id/117718/

