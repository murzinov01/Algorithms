// Copyright 2020 GHA Test Team
#include "VNS.h"


VNS::VNS() {
  matrix = nullptr;
  machinesSolution = partsSolution = nullptr;
  machines = parts = all_ones = 0;
}
VNS::VNS(std::string file_name) {
  ReadData(file_name);
  machinesSolution = partsSolution = nullptr;
}


unsigned VNS::GetMachinesNumber() const {
  return machines;
}
unsigned VNS::GetPartsNumber() const {
  return parts;
}
unsigned* VNS::GetMachinesSolution() const {
  return machinesSolution;
}
unsigned* VNS::GetPartsSolution() const {
  return partsSolution;
}
bool** VNS::GetMatrix() const {
  return matrix;
}


void VNS::PrintMatrix() {
  std::cout << "Shape: " << machines << "x" << parts
    << "\nAll ones: " << all_ones << std::endl;
  for (unsigned i = 0; i < machines; i++) {
    for (unsigned j = 0; j < parts; j++) {
      std::cout << matrix[i][j] << " ";
    }
    std::cout << std::endl;
  }
}
void VNS::PrintMachinesSolution() {
  std::cout << "(";
  for (unsigned i = 0; i < machines; i++)
    std::cout << machinesSolution[i] << " ";
  std::cout << ")";
}
void VNS::PrintPartsSolution() {
  std::cout << "(";
  for (unsigned i = 0; i < parts; i++)
    std::cout << partsSolution[i] << " ";
  std::cout << ")";
}


void VNS::ReadData(std::string file_name) {
  std::string line;
  std::ifstream in(file_name);
  std::string machines = "";
  std::string parts = "";

  // Get size of matrix
  getline(in, line);
  bool find_firts_num = false;
  unsigned j = 0;
  for (unsigned i = 0; i < line.size(); i++) {

    if (line[i] == ' ') {
      find_firts_num = true;
      continue;
    }

    if (!find_firts_num)
      machines[i] = line[i];
    else
      parts[j++] = line[i];
  }
  this->machines = atoi(machines.c_str());
  this->parts = atoi(parts.c_str());

  // Create matrix;
  matrix = new bool* [this->machines];
  for (unsigned i = 0; i < this->machines; i++)
    matrix[i] = new bool[this->parts]{ false };

  // Initialize matrix;
  if (in.is_open()) {
    while (getline(in, line)) {
      std::string temp;
      std::istringstream line_(line);
      std::vector<int> tokens;
      while (getline(line_, temp, ' ')) {
        tokens.push_back(atoi(temp.c_str()));
      }
      for (unsigned i = 1; i < tokens.size(); i++) {
        matrix[tokens[0] - 1][tokens[i] - 1] = true;
        all_ones++;
      }
    }
  }
  in.close();
}

double VNS::TargetFunction() {
  return 0.0;
}

void VNS::CreateInitialDecision() {

}

void VNS::VND() {

}

void VNS::GeneralVNS() {

}

// For Search in VND
void VNS::MoveRows() {

}

void VNS::MoveColumns() {

}


// For shaking in General VNS
void VNS::DevideClusters() {

}

void VNS::MergeClusters() {

}
