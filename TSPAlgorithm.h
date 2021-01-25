// Copyright 2020 GHA Test Team
#ifndef INCLUDE_TSPALGORITHM_H_
#define INCLUDE_TSPALGORITHM_H_
#include <fstream>
#include <cmath>
#include <stdio.h>
#include <iostream>
#include <string>
#include <vector>


struct node_info {
  long id;
  double x;
  double y;
};


struct Change {
  double cost;
 // long lvertex_1;
  long rvertex_1_index;
  long rvertex_1;
  long lvertex_2;
  long lvertex_2_index;
 // long rvertex_2;
};


class DataReader {
public:
  double** dist_matrix;
  long node_num;
  std::vector<node_info> data;

  DataReader(std::string file_name) {
    std::ifstream infile(file_name);

    long id;
    double x, y;
    while (infile >> id >> x >> y) {
      node_info nf;
      nf.id = id;
      nf.x = x;
      nf.y = y;
      data.push_back(nf);
    }

    node_num = data.size();
    dist_matrix = new double* [node_num];
    for (long i = 0; i < node_num; i++)
      dist_matrix[i] = new double[node_num];

    for (long i = 0; i < node_num; i++) {
      for (long j = i; j < node_num; j++) {
        // 0 in diag.
        if (i == j) {
          dist_matrix[i][j] = 0.0;
        }
        double length = std::sqrt(std::pow(data[j].x - data[i].x, 2) + std::pow(data[j].y - data[i].y, 2));
        dist_matrix[i][j] = length;
        dist_matrix[j][i] = length;
      }
    }
  }

  void printGraphEdges(bool** path) {
    for (long i = 0; i < node_num; i++)
      for (long j = 0; j < node_num; j++)
        if (path[i][j])
          std::cout << data[i].id << " " << data[j].id << std::endl;
  }

  void saveGraphEdges(bool** path, std::string file_name) {
    std::ofstream out;
    out.open(file_name);

    if (out.is_open())
    {
      for (long i = 0; i < node_num; i++)
        for (long j = 0; j < node_num; j++)
          if (path[i][j])
            out << data[i].id << " " << data[j].id << std::endl;
    }
  }
};


class TSP {
private:
  long size;
  double path_cost;
  long* path;
  bool** matrix_path;
  double** dist_matrix;

public:
  bool first_step = false;
  long getSize() const;
  long* getPath() const;
  double getPathCost() const;
  bool** getMatrixPath() const;
  double** getDistMatrix() const;
  TSP(double** dist_matrix, long node_num);
  ~TSP();

  void printDecisionPath();
  void printMatrixPath();
  void printMatrixDist();
  template <class T>
  void mirror_matrix(T** matrix, long size_t);
  void createMatrixPath();
  double calculatePathCost(double** matrix, long* path, long size);

  void createInitialDecision();
  bool localSearch();
  void iteratedLocalSearch();
};
#endif  // INCLUDE_TSPALGORITHM_H_
