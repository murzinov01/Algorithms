// Copyright 2020 GHA Test Team
#ifndef INCLUDE_VNS_H_
#define INCLUDE_VNS_H_
#include <fstream>
#include <cmath>
#include <iostream>
#include <string>
#include <sstream>
#include <vector>
#include <time.h>
#include <algorithm>

struct RNG {
  int operator() (int n) {
    return std::rand() / (1.0 + RAND_MAX) * n;
  }
};

struct RowsPair {
  unsigned i, j;
  float score;
};

class VNS {
private:
  bool** matrix;
  unsigned* machinesSolution;
  unsigned* partsSolution;
  unsigned machines, parts, all_ones;
  double bestTarget = 0.0;
  double TargetFunction(unsigned* newMachinesSolution = nullptr,
                        unsigned* newPartsSolution = nullptr);
  unsigned clustersNum;
  // For Search in VND
  void MoveRows();
  void MoveColumns();
  void Permutation(bool isRows);
  // For shaking in General VNS
  void DivideClusters(bool findBest = false);
  unsigned* DivideInTwo(unsigned& c, unsigned* targetVectorSolution,
                        unsigned& size,  float percent = 0.5f);
  void MergeClusters(bool findBest = false);
  unsigned* MergeTwo(unsigned& c1, unsigned& c2,
                     unsigned* targetVectorSolution, unsigned& size);
  std::vector <void(*)()> neighbours;
  void GetShakingNeighbours(bool, int, int);

public:
  VNS();
  VNS(std::string file_name, bool findGreedy = false);

  unsigned GetMachinesNumber() const;
  unsigned GetPartsNumber() const;
  unsigned* GetMachinesSolution() const;
  unsigned* GetPartsSolution() const;
  bool** GetMatrix() const;
  double GetBestTarget() const { return bestTarget; }

  void PrintMatrix();
  void PrintMachinesSolution(unsigned* targetSoultion = nullptr);
  void PrintPartsSolution(unsigned* targetSoultion = nullptr);

  void ReadData(std::string file_name);
  void CreateInitialDecision();
  void CreateCleverInitialDecision(unsigned& targetClustersNum);
  void VND();
  void GeneralVNS(std::string resultFileName);
  void SmartGVNS(std::string resultFileName);
  void SaveData(std::string file_name);
};

#endif  // INCLUDE_VNS_H_
