// Copyright 2020 GHA Test Team
#include "VNS.h"


int main() {
  // VNS vns = VNS("data6.txt", true);
  // VNS vns = VNS("/Users/sanduser/Documents/Projects/Xcode/Algorithms/VNS/data.txt");
  //vns.PrintMatrix();
  //vns.PrintMachinesSolution();
  //vns.PrintPartsSolution();
  std::cout << "DATA1" << std::endl;
  VNS vns = VNS("data1.txt", true);
  std::cout << "Initial result: " << vns.GetBestTarget() << std::endl;
  vns.GeneralVNS("result_data1.txt");
  //vns.SmartGVNS("result_data1.txt");
  std::cout << std::endl;

  std::cout << "DATA2" << std::endl;
  VNS vns1 = VNS("data2.txt", true);
  std::cout << "Initial result: " << vns1.GetBestTarget() << std::endl;
  vns1.GeneralVNS("result_data2.txt");
  //vns1.SmartGVNS("result_data2.txt");
  std::cout << std::endl;

  std::cout << "DATA3" << std::endl;
  VNS vns2 = VNS("data3.txt", true);
  std::cout << "Initial result: " << vns2.GetBestTarget() << std::endl;
  vns2.GeneralVNS("result_data3.txt");
  //vns2.SmartGVNS("result_data3.txt");
  std::cout << std::endl;

  std::cout << "DATA4" << std::endl;
  VNS vns3 = VNS("data4.txt", true);
  std::cout << "Initial result: " << vns3.GetBestTarget() << std::endl;
  vns3.GeneralVNS("result_data4.txt");
  //vns3.SmartGVNS("result_data4.txt");
  std::cout << std::endl;

  std::cout << "DATA5" << std::endl;
  VNS vns4 = VNS("data5.txt", true);
  std::cout << "Initial result: " << vns4.GetBestTarget() << std::endl;
  vns4.GeneralVNS("result_data5.txt");
  //vns4.SmartGVNS("result_data5.txt");
  std::cout << std::endl;

  std::cout << "DATA6" << std::endl;
  VNS vns5 = VNS("data6.txt", true);
  std::cout << "Initial result: " << vns5.GetBestTarget() << std::endl;
  vns5.GeneralVNS("result_data6.txt");
  //vns5.SmartGVNS("result_data6.txt");
  std::cout << std::endl;

  //vns.PrintMachinesSolution();
  //vns.PrintPartsSolution();
  //std::cout << "Best Target Function: "
  //<< vns.GetBestTarget() << std::endl;
  return 0;
}
