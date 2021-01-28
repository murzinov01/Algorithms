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

double** TSP::getDistMatrix() const {
  return this->dist_matrix;
}

TSP::~TSP() {
  delete[] path;
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

TSP::TSP(double** dist_matrix, double** dist_pseudo_matrix, long node_num) {
  this->dist_matrix = dist_matrix;
  this->dist_pseudo_matrix = dist_pseudo_matrix;
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
  std::cout << this->path[0] + 1 << std::endl;
}

void TSP::printMatrixDist() {
  for (long i = 0; i < 25; i++) {
    for (long j = 0; j < 25; j++) {
      std::cout << dist_matrix[i][j] << " ";
    }
    std::cout << std::endl;
  }
}

void TSP::randomNodeStarter(DataReader* reader, std::string file_name, long node_start, long node_finish, long iterations) {
  long* best_path = nullptr;
  double best_cost = std::numeric_limits<double>::infinity();

  if (node_start == -1 && node_finish == -1) {
    node_start = 0;
    node_finish = this->size;
  }
  else if (node_start != -1 && node_finish == -1) {
    node_finish = this->size;
  }
  else if (node_start == -1 && node_finish != -1) {
    node_start = 0;
  }

  for (long i = node_start; i < node_finish; i++) {
    this->createInitialDecision(i);
    this->iteratedLocalSearch(reader, file_name, iterations);
    if (best_cost > this->path_cost) {
      best_cost = this->path_cost;
      best_path = this->path;
    }
  }
  this->path_cost = best_cost;
  this->path = best_path;
  reader->SavePath(file_name, this->path, this->path_cost, this->size);
  //std::cout << "Best Score: " << best_cost << std::endl;
  //std::cout << "Best route: " << std::endl;
  //this->printDecisionPath();
}

void TSP::createInitialDecision(int _start_vertex) {
  this->path = new long[this->size];
  double** dist_matrix = getDistMatrix();
  long start_vertex = _start_vertex, path_i = 0, initial_vertex;

  if (start_vertex == -1)
    start_vertex = std::rand() % size;

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
    start_vertex = min_i;
    this->path[path_i++] = start_vertex;
  }

  this->path_cost = calculatePathCost(this->path, this->size);
}

double TSP::calculatePathCost(long* path, long size, bool pseudo) {
  double cost = 0.0;
  long i;
  for (i = 0; i < size - 1; i++) {
    long j = i + 1;
    if (pseudo)
      cost += dist_pseudo_matrix[path[i]][path[j]];
    else
      cost += dist_matrix[path[i]][path[j]];
  }
  if (pseudo)
    cost += dist_pseudo_matrix[path[i]][path[0]];
  else
    cost += dist_matrix[path[i]][path[0]];
  return cost;
}

long* TSP::TwoOptSwap(long& i, long& j, long size) {
  long* new_path = new long[size] { -1 };
  
  for (long m = 0; m < i; m++)
    new_path[m] = this->path[m];

  long dec = 0;
  for (long c = i; c <= j; c++)
  {
    new_path[c] = this->path[j - dec];
    dec++;
  }

  for (long n = j + 1; n < size; n++)
    new_path[n] = this->path[n];

  return new_path;
}

bool TSP::localSearch() {
  std::vector<Change> change_list;
  double** dist_matrix = getDistMatrix();

  for (long i = 0; i < this->size - 1; i++) {
    for (long j = i + 1; j < this->size; j++) {
      long* new_path = TwoOptSwap(i, j, this->size);
      double cost = calculatePathCost(new_path, this->size);

      if (this->first_step) {

        if (cost < this->path_cost) {
          this->path_cost = cost;
          delete[] this->path;
          this->path = new_path;
          return true;
        }
      }
      else {
        Change change;
        change.cost = cost;
        change.node1 = i;
        change.node2 = j;
        change_list.push_back(change);
      }

      delete[] new_path;

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

  if (min_change.cost < this->path_cost) {
    long* new_path = TwoOptSwap(min_change.node1, min_change.node2, this->size);
    delete[] this->path;
    this->path = new_path;
    this->path_cost = min_change.cost;
    return true;
  }
  else 
    return false;
}

void TSP::iteratedLocalSearch(DataReader* reader, std::string file_name, long iterations) {
  if (iterations == -1) {
    long i = 0;
    while (this->localSearch()) {
      std::cout << "Iteration: " << i + 1 << " COST: " << this->path_cost << std::endl;
      i++;
    }
    reader->SavePath(file_name, this->path, this->path_cost, this->size);
  }
  else
    for (long i = 0; i < iterations; i++) {
      std::cout << "Iteration: " << i + 1 << " COST: " << this->path_cost << std::endl;
      bool result = this->localSearch();
      reader->SavePath(file_name, this->path, this->path_cost, this->size);
      if (!result)
        return;
    }
  //std::cout << "RESULT COST: " << this->path_cost << std::endl;
  //std::cout << "RESULT PATH: " << std::endl;
  //this->printDecisionPath();
}
