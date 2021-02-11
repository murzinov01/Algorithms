// Copyright 2020 GHA Test Team
#include "VNS.h"


int main() {
  
  VNS vns = VNS("/Users/sanduser/Documents/Projects/Xcode/Algorithms/VNS/king30x90.txt", true);
  // VNS vns = VNS("/Users/sanduser/Documents/Projects/Xcode/Algorithms/VNS/data.txt");
  vns.PrintMatrix();
  vns.PrintMachinesSolution();
  vns.PrintPartsSolution();
  std::cout << "Best Target Function: "
  << vns.GetBestTarget() << std::endl;
  vns.GeneralVNS();
  //vns.PrintMachinesSolution();
  //vns.PrintPartsSolution();
  //std::cout << "Best Target Function: "
  //<< vns.GetBestTarget() << std::endl;
  vns.PrintMachinesSolution();
  vns.PrintPartsSolution();
  return 0;
}
