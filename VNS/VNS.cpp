// Copyright 2020 GHA Test Team
#include "VNS.h"


template<typename T>
void copyArray(T* arrayTo, T* arrayFrom, unsigned size) {
  for (int i = 0; i < size; i++)
    arrayTo[i] = arrayFrom[i];
}

VNS::VNS() {
  matrix = nullptr;
  machinesSolution = partsSolution = nullptr;
  machines = parts = all_ones = clustersNum = 0;
}
VNS::VNS(std::string file_name, bool findGreedy) {
  ReadData(file_name);
  if (findGreedy){
    unsigned targetClustersNum = std::min(machines, parts);
    CreateCleverInitialDecision(targetClustersNum);
  } else {
    CreateInitialDecision();
  }
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
void VNS::PrintMachinesSolution(unsigned* targetSoultion) {
  std::cout << "Machines: (";
  for (unsigned i = 0; i < machines; i++){
    if (targetSoultion == nullptr)
      std::cout << machinesSolution[i] << " ";
    else
      std::cout << targetSoultion[i] << " ";
  }

  std::cout << "); ClustersNum: " << clustersNum << std::endl;
}
void VNS::PrintPartsSolution(unsigned* targetSoultion) {
  std::cout << "Parts: (";
  for (unsigned i = 0; i < parts; i++){
    if (targetSoultion == nullptr)
      std::cout << partsSolution[i] << " ";
    else
      std::cout << targetSoultion[i] << " ";
  }
  std::cout << "); ClustersNum: " << clustersNum << std::endl;
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

void VNS::CreateCleverInitialDecision(unsigned& targetClustersNum) {
  unsigned localTarget = targetClustersNum;
  if (targetClustersNum > std::min(machines, parts)){
    std::cout<< "ERROR: too big 'targetCLustersNum' ("
    << targetClustersNum << ")" << std::endl;
    localTarget = std::min(machines, parts);
  } else {
    localTarget = targetClustersNum;
  }
  
  // *Define compatibility of rows*
  float** compatibilityMatrix = new float* [machines];
  for (unsigned i = 0; i < machines; i++)
    compatibilityMatrix[i] = new float[machines] { 0.0 };
  std::vector<RowsPair> pairs;
  
  for (unsigned i = 0; i < machines; i++){
    compatibilityMatrix[i][i] = -1;
    for (unsigned j = i + 1; j < machines; j ++){
      // we have chosen two raws to compare - raw i and j
      unsigned oneScore = 0;
      unsigned zeroScore = 0;
      for (unsigned n = 0; n < parts; n++){
        if (matrix[i][n] && matrix[j][n])
          oneScore++;
        if (!matrix[i][n] && !matrix[j][n])
          zeroScore++;
      }
      float score = (float)oneScore / parts;
      compatibilityMatrix[i][j] = score;
      compatibilityMatrix[j][i] = score;
      
//      float score = oneScore + zeroScore / (parts * 2);
      RowsPair curPair;
      curPair.i = i;
      curPair.j = j;
      curPair.score = score;
      
      // insert pair with sorting
      if (!pairs.size()) {
        pairs.push_back(curPair);
        continue;
      }

      int position = -1;
      for (unsigned n = 0; n < pairs.size(); n++){
        if (score >= pairs[n].score){
          continue;
        }
        position = n;
        break;
      }
      if (position == -1)
        position = pairs.size();
      pairs.insert(pairs.begin() + position, curPair); 
    }
  }
  
  unsigned* curMachinesSolution = new unsigned[machines] { 0 };
  unsigned curClustersNum = 0;
  unsigned pairsIter = 0;
  //PrintMachinesSolution(curMachinesSolution);
  // *Create maximum of clusters using greedy*
  while (curClustersNum < localTarget && pairsIter < pairs.size()){ //!
    RowsPair curPair = pairs[pairs.size() - pairsIter - 1];
    if (!curMachinesSolution[curPair.i] && !curMachinesSolution[curPair.j]){
      curMachinesSolution[curPair.i] = curClustersNum + 1;
      curMachinesSolution[curPair.j] = curClustersNum + 1;
      //PrintMachinesSolution(curMachinesSolution); 
      curClustersNum++;
    }
    pairsIter++;
  }
  
  // *Distribute other free machines*
  for (int i = 0; i < machines; i++){
    if (curMachinesSolution[i])
      continue;
    float* applicantClusters = new float[localTarget] { 0.0 };
    // calculate compatibility with each cluster
    for (int j = 0; j < machines; j++){
      if (!curMachinesSolution[j])
        continue;
      applicantClusters[curMachinesSolution[j] - 1] += compatibilityMatrix[i][j];
    }
    unsigned best_c = 0;
    float bestCompatibility = 0.0;
    for (unsigned j = 0; j < localTarget; j++){
      if (applicantClusters[j] >= bestCompatibility){
        bestCompatibility = applicantClusters[j];
        best_c = j + 1;
      }
    }
    // add to best cluster
    curMachinesSolution[i] = best_c;
    delete [] applicantClusters;
  }
  
  // *Distribute parts to the clusters
  unsigned* curPartsSolution = new unsigned[parts] { 0 };
  
  for (unsigned i = 0; i < parts; i++){
    // every part
    // choose best cluster
    unsigned* applicantClusters = new unsigned[localTarget] { 0 };
    for (unsigned j = 0; j < machines; j++){
      // every machnine
      if (!matrix[j][i])
        continue;
      unsigned clusterName = curMachinesSolution[j];
      applicantClusters[clusterName - 1]++;
    }
    
    unsigned best_c = 0;
    float bestCompatibility = 0;
    for (unsigned j = 0; j < localTarget; j++){
      if (applicantClusters[j] >= bestCompatibility){
        bestCompatibility = applicantClusters[j];
        best_c = j + 1;
      }
    }
    // add to best cluster
    curPartsSolution[i] = best_c;
    delete [] applicantClusters;
  }
  
  // *Free memory*
  for (int i = 0; i < machines; i++)
    delete [] compatibilityMatrix[i];
  delete [] compatibilityMatrix;
  
  clustersNum = curClustersNum;
  delete [] machinesSolution;
  delete [] partsSolution;
  machinesSolution = curMachinesSolution;
  partsSolution = curPartsSolution;
  bestTarget = TargetFunction();
}

void VNS::VND() {
  unsigned* bestMachinesSolution = new unsigned[machines] {0};
  unsigned* bestPartsSolution = new unsigned[parts] {0};
  unsigned lMax = 2, l = 0;
  double curBestTarget = bestTarget;
  copyArray(bestMachinesSolution, this->machinesSolution, machines);
  copyArray(bestPartsSolution, this->partsSolution, parts);
  while (l != lMax) {
    unsigned* curMachinesSolution = new unsigned[machines] {0};
    unsigned* curPartsSolution = new unsigned[parts] {0};
    copyArray(curMachinesSolution, this->machinesSolution, machines);
    copyArray(curPartsSolution, this->partsSolution, parts);
    double curBestTarget_ = this->bestTarget;
    if (l == 0) {
      //MoveRows();
      //std::cout << "MOVE COLUMNS" << std::endl;
      MoveColumns();
    }
    else if (l == 1) {
      //std::cout << "MOVE ROWS" << std::endl;
      MoveRows();
      //MoveColumns();
    }
    if (bestTarget > curBestTarget) {
      copyArray(bestMachinesSolution, this->machinesSolution, machines);
      copyArray(bestPartsSolution, this->partsSolution, parts);
      curBestTarget = bestTarget;
      l = 0;
    }
    else {
      l++;
      copyArray(this->machinesSolution, curMachinesSolution, machines);
      copyArray(this->partsSolution, curPartsSolution, parts);
      this->bestTarget = curBestTarget_;
    }
    delete[] curMachinesSolution;
    delete[] curPartsSolution;
  }
  copyArray(this->machinesSolution, bestMachinesSolution, machines);
  copyArray(this->partsSolution, bestPartsSolution, parts);
  bestTarget = curBestTarget;
  // std::cout << "BEST TARGET ***: " << curBestTarget << std::endl;
}

int compare(const void* x1, const void* x2) {
  return (int)(*(double*)x1 - *(double*)x2);       
}

void VNS::GetShakingNeighbours(bool merge, int numberOfShakes, int topN) {
  unsigned** machinesSolutions = new unsigned* [numberOfShakes];
  unsigned** partsSolutions = new unsigned* [numberOfShakes];
  for (int i = 0; i < numberOfShakes; i++) {
    machinesSolutions[i] = new unsigned[machines];
    partsSolutions[i] = new unsigned[parts];
  }
  double* costs = new double[numberOfShakes] { 0.0 };
  double* sorted_costs = new double[numberOfShakes] { 0.0 };
  unsigned** machinesSolutionsAfterVnd = new unsigned* [topN];
  unsigned** partsSolutionsAfterVnd = new unsigned* [topN];
  for (int i = 0; i < topN; i++) {
    machinesSolutionsAfterVnd[i] = new unsigned[machines];
    partsSolutionsAfterVnd[i] = new unsigned[parts];
  }
  double* costsAfterVnd = new double[topN] { 0.0 };
  unsigned* curMachinesSolution = new unsigned[machines];
  unsigned* curpartsSolution = new unsigned[parts];
  int curClustersNum = this->clustersNum;

  copyArray(curMachinesSolution, this->machinesSolution, machines);
  copyArray(curpartsSolution, this->partsSolution, parts);
  for (int i = 0; i < numberOfShakes; i++) {
    //std::cout << "Shake number " << i << std::endl;
    this->clustersNum = curClustersNum;
    copyArray(this->machinesSolution, curMachinesSolution, machines);
    copyArray(this->partsSolution, curpartsSolution, parts);
    if (merge)
      MergeClusters();
    else
      DivideClusters();
    copyArray(machinesSolutions[i], this->machinesSolution, machines);
    copyArray(partsSolutions[i], this->partsSolution, parts);
    costs[i] = this->bestTarget;
    sorted_costs[i] = this->bestTarget;
  }
  this->clustersNum--;
  qsort(sorted_costs, numberOfShakes, sizeof(double), compare);

  for (int i = numberOfShakes - 1; i >= numberOfShakes - topN; i--) {
    for (int j = 0; j < numberOfShakes; j++) {
      if (costs[j] == sorted_costs[i]) {
        copyArray(this->machinesSolution, machinesSolutions[j], machines);
        copyArray(this->partsSolution, partsSolutions[j], parts);
        this->bestTarget = costs[j];
        std::cout << "VND number " << numberOfShakes - 1 - i << std::endl;
        VND();
        copyArray(machinesSolutionsAfterVnd[numberOfShakes - 1 - i], this->machinesSolution, machines);
        copyArray(partsSolutionsAfterVnd[numberOfShakes - 1 - i], this->partsSolution, parts);
        costsAfterVnd[numberOfShakes - 1 - i] = this->bestTarget;
        break;
      }
    }
  }
  double best_cost = 0.0;
  int best_i = 0;
  for (int i = 0; i < topN; i++) {
    if (best_cost < costsAfterVnd[i]) {
      best_cost = costsAfterVnd[i];
      best_i = i;
    }
  }
  copyArray(this->machinesSolution, machinesSolutionsAfterVnd[best_i], machines);
  copyArray(this->partsSolution, partsSolutionsAfterVnd[best_i], parts);
  this->bestTarget = best_cost;

  // Delete data
  for (int i = 0; i < numberOfShakes; i++) {
    delete[] machinesSolutions[i];
    delete[] partsSolutions[i];;
  }
  for (int i = 0; i < topN; i++) {
    delete machinesSolutionsAfterVnd[i];
    delete partsSolutionsAfterVnd[i];
  }
  delete[] machinesSolutions;
  delete[] partsSolutions;
  delete[] machinesSolutionsAfterVnd;
  delete[] partsSolutionsAfterVnd;
}

void VNS::SmartGVNS(std::string resultFileName) {
  unsigned* bestMachinesSolution = new unsigned[machines] {0};
  unsigned* bestPartsSolution = new unsigned[parts] {0};
  VND();
  double curBestTarget = bestTarget;
  while (true) {
    unsigned* curMachinesSolution = new unsigned[machines] {0};
    unsigned* curPartsSolution = new unsigned[parts] {0};
    unsigned* MergeMachinesSolution = new unsigned[machines] {0};
    unsigned* MergePartsSolution = new unsigned[parts] {0};
    double MergeBestTarget = 0.0;

    copyArray(curMachinesSolution, this->machinesSolution, machines);
    copyArray(curPartsSolution, this->partsSolution, parts);

    MergeClusters(true);
    MergeBestTarget = this->bestTarget;
    copyArray(MergeMachinesSolution, this->machinesSolution, machines);
    copyArray(MergePartsSolution, this->partsSolution, parts);
    copyArray(this->machinesSolution, curMachinesSolution, machines);
    copyArray(this->partsSolution, curPartsSolution, parts);
    this->bestTarget = curBestTarget;

    DivideClusters(true);
    std::cout << "MERGE COST: " << MergeBestTarget << "   DIVIDE COST: " << this->bestTarget << "   ";
    if (this->bestTarget < MergeBestTarget) {
      //std::cout << "MERGE    ";
      copyArray(this->machinesSolution, MergeMachinesSolution, machines);
      copyArray(this->partsSolution, MergePartsSolution, parts);
      this->bestTarget = MergeBestTarget;
      delete[] MergeMachinesSolution;
      delete[] MergePartsSolution;
    }
    else {
      //std::cout << "DIVIDE    ";
    }
    delete[] curMachinesSolution;
    delete[] curPartsSolution;

    VND();
    if (this->bestTarget > curBestTarget) {
      copyArray(bestMachinesSolution, this->machinesSolution, machines);
      copyArray(bestPartsSolution, this->partsSolution, parts);
      curBestTarget = this->bestTarget;
      std::cout << "BEST TARGET: " << this->bestTarget << std::endl;
    }
    else {
      break;
    }
  }
  copyArray(this->machinesSolution, bestMachinesSolution, machines);
  copyArray(this->partsSolution, bestPartsSolution, parts);
  bestTarget = curBestTarget;
  SaveData(resultFileName);
}

void VNS::GeneralVNS(std::string resultFileName) {
  unsigned kMax = 2, k = 0;
  unsigned* bestMachinesSolution = new unsigned[machines] {0};
  unsigned* bestPartsSolution = new unsigned[parts] {0};
  copyArray(bestMachinesSolution, this->machinesSolution, machines);
  copyArray(bestPartsSolution, this->partsSolution, parts);
  VND();
  double curBestTarget = this->bestTarget;
  while (k != kMax) {
    unsigned* curMachinesSolution = new unsigned[machines] {0};
    unsigned* curPartsSolution = new unsigned[parts] {0};
    copyArray(curMachinesSolution, this->machinesSolution, machines);
    copyArray(curPartsSolution, this->partsSolution, parts);
    double curBestTarget_ = this->bestTarget;
    if (k == 0) {
      std::cout << "*MERGE*    ";
      MergeClusters(true);
      //std::cout << "*DIVIDE*    ";
      //DivideClusters();
    }
    else if (k == 1) {
      std::cout << "*DIVIDE*    ";
      DivideClusters(true);
      //std::cout << "*MERGE*    ";
      //MergeClusters();
    }
    VND();
    if (this->bestTarget > curBestTarget) {
      copyArray(bestMachinesSolution, this->machinesSolution, machines);
      copyArray(bestPartsSolution, this->partsSolution, parts);
      curBestTarget = this->bestTarget;
      std::cout << "BEST TARGET: " << this->bestTarget << std::endl;
      k = 0;
    }
    else {
      k++;
      copyArray(this->machinesSolution, curMachinesSolution, machines);
      copyArray(this->partsSolution, curPartsSolution, parts);
      this->bestTarget = curBestTarget_;
    }
    delete[] curMachinesSolution;
    delete[] curPartsSolution;
  }
  copyArray(this->machinesSolution, bestMachinesSolution, machines);
  copyArray(this->partsSolution, bestPartsSolution, parts);
  bestTarget = curBestTarget;
  SaveData(resultFileName);
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
void VNS::DivideClusters(bool findBest){
  if (clustersNum >= std::min(machines, parts))
    return;
  
  double bestTargetInSolution = 0;
  unsigned best_c = 0;
  std::vector<unsigned> allValidClusters;
  
  for (unsigned i = 1; i <= clustersNum; i++){
    unsigned* curMachinesSoultion = DivideInTwo(i, machinesSolution, machines);
    if (curMachinesSoultion == nullptr)
      continue;
    unsigned* curPartsSoultion = DivideInTwo(i, partsSolution, parts);
    if (curPartsSoultion == nullptr)
      continue;
    double target = TargetFunction(curMachinesSoultion, curPartsSoultion);
    allValidClusters.push_back(i);
    // check changes
    if (target > bestTargetInSolution){
      bestTargetInSolution = target;
      best_c = i;
    }
    delete [] curMachinesSoultion;
    delete [] curPartsSoultion;
  }
  
  if (!findBest && allValidClusters.size() != 0){
    srand (time(NULL));
    unsigned random = rand() % allValidClusters.size();
    best_c = allValidClusters[random];
  }
  
  // implement changes
  if (best_c){
    unsigned* newMachinesSoultion = DivideInTwo(best_c, machinesSolution, machines);
    unsigned* newPartsSoultion = DivideInTwo(best_c, partsSolution, parts);
    delete [] machinesSolution;
    delete [] partsSolution;
    machinesSolution = newMachinesSoultion;
    partsSolution = newPartsSoultion;
    
    bestTarget = bestTargetInSolution;
    clustersNum++;
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
  
  if (findBest){
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
  } else {

    unsigned* clustersArray = new unsigned[clustersNum];
    for (int i = 0; i < clustersNum; i++)
      clustersArray[i] = i + 1;
    std::srand(time(NULL) + rand());
    std::random_shuffle<unsigned int *>(&clustersArray[0], &clustersArray[clustersNum], RNG());
    
    //std::cout << "Clusters shuffle: "<< std::endl;
    //for (int i = 0; i < clustersNum; i++)
    //  std::cout << clustersArray[i] << " ";
    //std::cout << std::endl;

    best_c1 = clustersArray[0];
    best_c2 = clustersArray[1];
  }
  
  
  // implement changes
  if (best_c1 && best_c2){
    unsigned* newMachinesSolution = MergeTwo(best_c1, best_c2, machinesSolution, machines);
    unsigned* newPartsSolution = MergeTwo(best_c1, best_c2, partsSolution, parts);
    delete [] machinesSolution;
    delete [] partsSolution;
    machinesSolution = newMachinesSolution;
    partsSolution = newPartsSolution;
    
    bestTarget = TargetFunction();
    clustersNum--;
  }
}

unsigned* VNS::MergeTwo(unsigned& c1, unsigned& c2,
                        unsigned* targetVectorSolution, unsigned& size){
  unsigned* curSolution = new unsigned[size];
  
  if (c1 > c2){
    unsigned tmp = c2;
    c2 = c1;
    c1 = tmp;
  }
  
  for (unsigned m = 0; m < size; m++){
    if (targetVectorSolution[m] == c2)
      curSolution[m] = c1;
    else if (targetVectorSolution[m] > c2)
      curSolution[m] = targetVectorSolution[m] - 1;
    else
      curSolution[m] = targetVectorSolution[m];
  }
  
  return curSolution;
}

void VNS::SaveData(std::string file_name) {
  std::ofstream out;
  out.open(file_name);

  if (out.is_open())
  {
    for (long i = 0; i < machines; i++)
      out << machinesSolution[i] << " ";
    out << std::endl;
    for (long i = 0; i < parts; i++)
      out << partsSolution[i] << " ";
    out << std::endl;
    out << bestTarget << std::endl;
  }
}