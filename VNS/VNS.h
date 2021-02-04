// Copyright 2020 GHA Test Team
#ifndef INCLUDE_VNS_H_
#define INCLUDE_VNS_H_
#include <fstream>
#include <cmath>
#include <iostream>
#include <string>
#include <sstream>
#include <vector>


class VNS {
private:
  bool** matrix;
  unsigned* machinesSolution;
  unsigned* partsSolution;
  unsigned machines, parts, all_ones;
  double TargetFunction();



public:
  VNS();
  VNS(std::string file_name);

  unsigned GetMachinesNumber() const;
  unsigned GetPartsNumber() const;
  unsigned* GetMachinesSolution() const;
  unsigned* GetPartsSolution() const;
  bool** GetMatrix() const;

  void PrintMatrix();
  void PrintMachinesSolution();
  void PrintPartsSolution();

  void ReadData(std::string file_name);
  void CreateInitialDecision();
  void VND();
  void GeneralVNS();

  // For Search in VND
  void MoveRows();
  void MoveColumns();


  // For shaking in General VNS
  void DevideClusters();
  void MergeClusters();
};

#endif  // INCLUDE_VNS_H_