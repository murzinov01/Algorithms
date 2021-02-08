// Copyright 2020 GHA Test Team
#include "VNS.h"


VNS::VNS() {
  matrix = nullptr;
  machinesSolution = partsSolution = nullptr;
  machines = parts = all_ones = clustersNum = 0;
}
VNS::VNS(std::string file_name) {
  ReadData(file_name);
  CreateInitialDecision();
//  this->neighbours.push_back(&this->MoveColumns());
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
  std::string temp;
  std::istringstream line_(line);
  std::vector<int> tokens;
  while (getline(line_, temp, ' ')) {
    tokens.push_back(atoi(temp.c_str()));
  }
  this->machines = tokens[0];
  this->parts = tokens[1];

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
  this->machinesSolution = new unsigned[this->machines];
  this->partsSolution = new unsigned[this->parts];
}

double VNS::TargetFunction(unsigned* newMachinesSolution,
                           unsigned* newPartsSolution) {
  
  if (newMachinesSolution == nullptr)
    newMachinesSolution = this->machinesSolution;
  if (newPartsSolution == nullptr)
    newPartsSolution = this->partsSolution;
  
  
  unsigned zeroes_in = 0;
  unsigned ones_in = 0;
  for (int i = 0; i < this->machines; i++){
    for (int j = 0; j < this->parts; j++){
      if (newMachinesSolution[i] ==
          newPartsSolution[j]){
        if (this->matrix[i][j])
          ones_in++;
        else
          zeroes_in++;
      }
    }
  }
  
  return (double)ones_in/((double)this->all_ones + (double)zeroes_in);
}

void VNS::CreateInitialDecision() {
  unsigned avg_clusters = (2 + std::min(machines, parts) / 2);
  this->clustersNum = avg_clusters;
  unsigned cur_cluster = 1;
  for (int i = 0; i < machines; i++){
    if (cur_cluster > avg_clusters)
      cur_cluster = 1;
    machinesSolution[i] = cur_cluster++;
  }
  
  cur_cluster = 1;
  for (int i = 0; i < parts; i++){
    if (cur_cluster > avg_clusters)
      cur_cluster = 1;
    partsSolution[i] = cur_cluster++;
  }
  
  this->bestTarget = this->TargetFunction();
  
}

//MoveRows();
//MoveColumns();
// MergeClusters();
// MergeClusters();
// MergeClusters();

//PrintMachinesSolution();
//PrintPartsSolution();
//
//DivideClusters();
//  MoveColumns();
//  MoveRows();

void VNS::VND() {
  unsigned lMax = 2, l = 0;
  double curBestTarget = bestTarget;
  while (l != lMax) {
    if (l == 0) {
      //MoveRows();
      MoveColumns();
    }
    else if (l == 1) {
      MoveRows();
      //MoveColumns();
    }
    if (bestTarget <= curBestTarget)
      l++;
    else
      curBestTarget = bestTarget;
  }
}

void VNS::GeneralVNS() {
  unsigned kMax = 2, k = 0;
  double curBestTarget = bestTarget;
  while (k != kMax) {
    std::cout << "Shaking" << std::endl;
    if (k == 0) {
      MergeClusters();
      //DivideClusters();
    }
    else if (k == 1) {
      //MergeClusters();
      DivideClusters();
    }
    VND();
    PrintMachinesSolution();
    PrintPartsSolution();
    std::cout << "Best Target Function: "<< GetBestTarget() << std::endl;
    if (bestTarget <= curBestTarget)
      k++;
    else
      curBestTarget = bestTarget;
  }
  std::cout << "BEST TARGET: " << curBestTarget << std::endl;
}

// For Search in VND
void VNS::MoveRows() {
  Permutation(true);
}

void VNS::MoveColumns() {
  Permutation(false);
}

void VNS::Permutation(bool isRows){
  unsigned targetSize;
  unsigned* targetVector;
  if (isRows){
    targetSize = machines;
    targetVector = machinesSolution;
  } else {
    targetSize = parts;
    targetVector = partsSolution;
  }
  
  double bestTargetInSolution = this->bestTarget;
  unsigned* bestVectorSolution = new unsigned[targetSize];
  
  for (unsigned i = 0; i < targetSize - 1; i++){
    for (unsigned j = i + 1; j < targetSize; j++){
      // permutation
      unsigned* newVectorSolution = new unsigned[targetSize];
      for (unsigned n = 0; n < targetSize; n++)
        newVectorSolution[n] = targetVector[n];
      unsigned tmp = newVectorSolution[i];
      newVectorSolution[i] = newVectorSolution[j];
      newVectorSolution[j] = tmp;
      // calculate target function after permutation
      double target = isRows ? TargetFunction(newVectorSolution, nullptr) :
                               TargetFunction(nullptr, newVectorSolution);
      // check changes
      if (target > bestTargetInSolution) {
        delete [] bestVectorSolution;
        bestVectorSolution = newVectorSolution;
        bestTargetInSolution = target;
      } else {
        delete [] newVectorSolution;
      }
    }
  }
  // implement changes
  if (this->bestTarget < bestTargetInSolution){
    this->bestTarget = bestTargetInSolution;
    delete [] targetVector;
    if (isRows)
      this->machinesSolution = bestVectorSolution;
    else
      this->partsSolution = bestVectorSolution;
  } else {
    delete [] bestVectorSolution;
  }
  
}


// For shaking in General VNS
void VNS::DivideClusters() {
  if (clustersNum >= std::min(machines, parts))
    return;
  
  for (unsigned i = 1; i <= clustersNum; i++){
    unsigned* curMachinesSoultion = DivideInTwo(i, machinesSolution, machines);
    if (curMachinesSoultion == nullptr)
      continue;
    unsigned* curPartsSoultion = DivideInTwo(i, partsSolution, parts);
    if (curMachinesSoultion == nullptr)
      continue;
    
    delete [] machinesSolution;
    delete [] partsSolution;
    machinesSolution = curMachinesSoultion;
    partsSolution = curPartsSoultion;
    bestTarget = TargetFunction();
  }

}

unsigned* VNS::DivideInTwo(unsigned& c, unsigned* targetVectorSolution, unsigned& size, float percent){
  unsigned* curSolution = new unsigned[size];
  // count cluster's entities in vector
  unsigned clusterEntities = 0;
  for (int i = 0; i < size; i++)
    if (targetVectorSolution[i] == c)
      clusterEntities++;
  
  if (clusterEntities <= 1)
    return nullptr;
  
  unsigned newClusterEntites = percent * clusterEntities;
  // make division
  unsigned delimiterCounter = 0;
  for (unsigned i = 0; i < size; i++){
    if (targetVectorSolution[i] == c && delimiterCounter < newClusterEntites){
      curSolution[i] = clustersNum + 1;
      delimiterCounter++;
    } else {
      curSolution[i] = targetVectorSolution[i];
    }
  }
  
  return curSolution;
}


void VNS::MergeClusters(bool findBest) {
  if (clustersNum <= 2)
    return;
  
  double bestTargetInSolution = 0;
  unsigned best_c1 = 0;
  unsigned best_c2 = 0;
  
//  if (findBest){
    for (unsigned i = 1; i <= clustersNum; i++){
      for (unsigned j = i + 1; j <= clustersNum; j++){
        // (i , j) - clussters to be merged
        unsigned* curMachinesSoultion = MergeTwo(i, j, machinesSolution, machines);
        unsigned* curPartsSoultion = MergeTwo(i, j, partsSolution, parts);
        // check changes
        double target = TargetFunction(curMachinesSoultion, curPartsSoultion);
        if (target > bestTargetInSolution){
          bestTargetInSolution = target;
          best_c1 = i;
          best_c2 = j;
        }
        delete [] curMachinesSoultion;
        delete [] curPartsSoultion;
      }
    }
//  } else {
//    srand (std::time(NULL));
//    best_c1 = std::rand() %
//  }
  
  
  // implement changes
  if (best_c1 && best_c2){
    machinesSolution = MergeTwo(best_c1, best_c2, machinesSolution, machines);
    partsSolution = MergeTwo(best_c1, best_c2, partsSolution, parts);
    bestTarget = bestTargetInSolution;
    clustersNum--;
  }
}

unsigned* VNS::MergeTwo(unsigned& c1, unsigned& c2,
                        unsigned* targetVectorSolution, unsigned& size){
  unsigned* curSolution = new unsigned[size];
  
  for (unsigned m = 0; m < size; m++){
  if (targetVectorSolution[m] == c2)
    curSolution[m] = c1;
  else if (targetVectorSolution[m] > c1)
    curSolution[m] = targetVectorSolution[m] - 1;
  else
    curSolution[m] = targetVectorSolution[m];
  }
  
  return curSolution;
}
