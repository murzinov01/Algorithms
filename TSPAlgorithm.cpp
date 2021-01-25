#include "TSPAlgorithm.h"


long TSP::getSize() const {
  return this->size;
}

double TSP::getPathCost() const {
  return path_cost;
}

long* TSP::getPath() const {
  return this->path;
}

bool** TSP::getMatrixPath() const {
  return this->matrix_path;
}

double** TSP::getDistMatrix() const {
  return this->dist_matrix;
}

TSP::~TSP() {
  delete[] path;
  for (long i = 0; i < size; i++)
    delete[] matrix_path[i];
  delete[] matrix_path;
}

template <class T>
void TSP::mirror_matrix(T** matrix, long size_t) {
  for (int i = 0; i < size_t; i++) {
    for (int j = 0; j < size_t; j++) {
      if (matrix[i][j])
        matrix[j][i] = matrix[i][j];
    }
  }
}

TSP::TSP(double** dist_matrix, long node_num) {
  this->dist_matrix = dist_matrix;
  this->size = node_num;
}

bool checkInArray(long* array, long size, double el) {
  for (long i = 0; i < size; i++)
    if (array[i] == el)
      return true;
  return false;
}

void TSP::printDecisionPath() {
  for (long i = 0; i < this->size; i++) {
    std::cout << this->path[i] + 1 << " -> ";
  }
  std::cout << this->path[this->size] + 1 << std::endl;
}

void TSP::printMatrixPath() {
  for (long i = 0; i < this->size; i++) {
  for (long j = 0; j < this->size; j++) {
    std::cout << matrix_path[i][j] << " ";
  }
  std::cout << std::endl;
  }
}

void TSP::printMatrixDist() {
  for (long i = 0; i < 25; i++) {
    for (long j = 0; j < 25; j++) {
      std::cout << dist_matrix[i][j] << " ";
    }
    std::cout << std::endl;
  }
}

void TSP::createInitialDecision() {
  this->path = new long[size + 1];
  this->matrix_path = new bool* [size];
  double** dist_matrix = getDistMatrix();
  long start_vertex = std::rand() % size, path_i = 0, initial_vertex;

  for (long i = 0; i < size; i++) {
    this->matrix_path[i] = new bool[size] {false};
    //for (long j = 0; j < size; j++)
    //  this->matrix_path[i][j] = false;
  }

  this->path[path_i++] = start_vertex;
  initial_vertex = start_vertex;

  while (path_i != size) {
    double min_distance = std::numeric_limits<double>::infinity();
    long min_i = 0;
    for (long i = 0; i < size; i++) {
      if (!checkInArray(path, size, i) && dist_matrix[start_vertex][i] < min_distance) {
        min_distance = dist_matrix[start_vertex][i];
        min_i = i;
      }
    }
    this->matrix_path[start_vertex][min_i] = true;
    start_vertex = min_i;
    this->path[path_i++] = start_vertex;
  }

  this->matrix_path[this->path[path_i - 1]][initial_vertex] = true;
  this->path[path_i] = initial_vertex;
  this->path_cost = calculatePathCost(dist_matrix, this->path, size + 1);
}

double TSP::calculatePathCost(double** dist_matrix, long* path, long path_size) {
  double cost = 0.0;
  for (long i = 0; i < path_size; i++) {
    long j = i + 1;
    if (i == path_size - 1)
      j = 0;
    cost += dist_matrix[path[i]][path[j]];
  }
  return cost;
}

void TSP::createMatrixPath() {
  delete[] this->matrix_path;
  this->matrix_path = new bool* [this->size];
  for (long i = 0; i < this->size; i++) {
    this->matrix_path[i] = new bool[this->size]{false};
  }
  for (long i = 0; i < this->size; i++) {
    this->matrix_path[this->path[i]][this->path[i + 1]] = true;
  }
}

bool TSP::localSearch() {
  std::vector<Change> change_list;
  double** dist_matrix = getDistMatrix();

  for (long i = 0; i < size - 1; i++) {
    for (long j = i + 1; j < size - 1; j++) {
      if (this->path[i] == this->path[j + 1] || this->path[i + 1] == this->path[j])
        continue;

      //std::cout << this->path[i] + 1 << " -> " << this->path[i + 1] + 1 << ", " << this->path[j] + 1 << " -> " << this->path[j + 1] + 1 << std::endl;

      Change change;
      change.rvertex_1 = path[i + 1];
      change.lvertex_2 = path[j];
      //change.rvertex_2 = path[j + 1];

      bool flag = false;

      if ((this->path[i] == 1) && (this->path[i + 1] == 28) && (this->path[j] == 47) && (this->path[j + 1] == 38)) {
        for (int i = 0; i < size + 1; i++)
          std::cout << path[i] << " ";
        std::cout << " COST: " <<this->path_cost << std::endl;
        flag = true;
      }

      //// change by 2-opt
      this->path[i + 1] = change.lvertex_2;
      this->path[j] = change.rvertex_1;

      change.cost = calculatePathCost(dist_matrix, path, size + 1);

      if (flag) {
        for (int i = 0; i < size + 1; i++)
          std::cout << path[i] << " ";
        std::cout << " COST: " << change.cost << std::endl;
      }

      if (this->first_step) {

        if (change.cost < this->path_cost) {
          this->path_cost = change.cost;
          std::cout << "FIRST STEP: NEW COST: " << this->path_cost << std::endl;
          return true;
        }
      }
      else {
        //if ((this->path[i] == 1) && (this->path[i + 1] == 28) && (this->path[j] == 47) && (this->path[j + 1] == 38))
        //{
        //  std::cout << "PREV COST: " << this->path_cost << std::endl;
        //  std::cout << "NEW COST: " << change.cost << std::endl;
        //}
        //std::cout << "PREV: " << this->path[i] + 1 << " -> " << change.rvertex_1 + 1<< std::endl;
        //std::cout << "NEW: " << change.lvertex_2 + 1 << " -> " << this->path[j + 1] + 1 << std::endl;
        change.lvertex_2_index = i + 1;
        change.rvertex_1_index = j;
        change_list.push_back(change);
      }

      this->path[i + 1] = change.rvertex_1;
      this->path[j] = change.lvertex_2;
    }
  }
  if (change_list.size() == 0)
    return false;
  Change min_change;

  min_change.cost = std::numeric_limits<double>::infinity();
  for (auto& change : change_list) {
    if (change.cost < min_change.cost)
      min_change = change;
  }

  if (min_change.cost <= this->path_cost) {
    this->path[min_change.lvertex_2_index] = min_change.lvertex_2;
    this->path[min_change.rvertex_1_index] = min_change.rvertex_1;

    this->path_cost = min_change.cost;

    // std::cout << this->path[i + 1] + 1 << " -> " << this->path[j] + 1 << std::endl;

    std::cout << "STEPEST: NEW COST: " << this->path_cost << std::endl;
    return true;
  }
  else 
    return false;
}

void TSP::iteratedLocalSearch() {
  //createInitialDecision();
  //localSearch();
  while (this->localSearch());
  std::cout << "RESULT PATH: " << std::endl;
  this->printDecisionPath();
  std::cout << "RESULT COST: " << this->path_cost << std::endl;
}

