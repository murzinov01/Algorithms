// Copyright 2020 GHA Test Team
#include "VNS.h"


int main() {
//  VNS vns = VNS("data.txt");
  VNS vns = VNS("/Users/sanduser/Documents/Projects/Xcode/Algorithms/VNS/data.txt");
  vns.PrintMatrix();
  vns.PrintMachinesSolution();
  vns.PrintPartsSolution();
  std::cout << "Best Target Function: "
  << vns.GetBestTarget() << std::endl;
  vns.VND();
  vns.PrintMachinesSolution();
  vns.PrintPartsSolution();
  std::cout << "Best Target Function: "
  << vns.GetBestTarget() << std::endl;
  return 0;
}
