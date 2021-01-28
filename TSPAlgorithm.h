// Copyright 2020 GHA Test Team
#ifndef INCLUDE_TSPALGORITHM_H_
#define INCLUDE_TSPALGORITHM_H_
#include <fstream>
#include <cmath>
#include <stdio.h>
#include <iostream>
#include <string>
#include <vector>
#include <thread>

struct node_info {
  long id;
  double x;
  double y;
};


struct Change {
  double cost;
  long node1;
  long node2;
};


class DataReader {
public:
  double** dist_matrix;
  double** dist_pseudo_matrix;
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
    dist_pseudo_matrix = new double* [node_num];
    for (long i = 0; i < node_num; i++) {
      dist_matrix[i] = new double[node_num];
      dist_pseudo_matrix[i] = new double[node_num];
    }

    for (long i = 0; i < node_num; i++) {
      for (long j = i; j < node_num; j++) {
        // 0 in diag.
        if (i == j) {
          dist_matrix[i][j] = 0.0;
        }
        double length = std::sqrt(std::pow(data[j].x - data[i].x, 2) + std::pow(data[j].y - data[i].y, 2));
        double pseudo_length = std::sqrt((std::pow(data[j].x - data[i].x, 2) + std::pow(data[j].y - data[i].y, 2)) / 10.0);
        dist_matrix[i][j] = length;
        dist_matrix[j][i] = length;

        dist_pseudo_matrix[i][j] = pseudo_length;
        dist_pseudo_matrix[j][i] = pseudo_length;
      }
    }
  }

  void saveGraphEdges(long* path, std::string file_name) {
    std::ofstream out;
    out.open(file_name);

    if (out.is_open())
    {
      for (long i = 0; i < node_num - 1; i++)
        out << data[path[i]].id << " " << data[path[i + 1]].id << std::endl;

      out << data[path[node_num - 1]].id << " " << data[path[0]].id << std::endl;
    }
  }

  void SavePath(std::string file_name, long* path, double& cost, long& size) {
    std::ofstream out;
    out.open(file_name);

    if (out.is_open())
    {
      for (long i = 0; i < size; i++)
        out << data[path[i]].id << " ";

      out << "COST: " << " " << cost << std::endl;
    }
  }
};


class TSP {
private:
  long size;
  double path_cost;
  long* path;
  double** dist_matrix;
  double** dist_pseudo_matrix;

public:
  bool first_step = false;
  long getSize() const;
  long* getPath() const;
  double getPathCost() const;
  double** getDistMatrix() const;
  TSP(double** dist_matrix, double** dist_pseudo_matrix, long node_num);
  ~TSP();

  void printDecisionPath();
  void printMatrixDist();
  template <class T>
  void mirror_matrix(T** matrix, long size_t);
  double calculatePathCost(long* path, long size, bool pseudo=false);

  void createInitialDecision(int start_vertex=-1);
  bool localSearch();
  long* TwoOptSwap(long& i, long& j, long size);
  void iteratedLocalSearch(DataReader* reader, std::string file_name, long interations=-1);
  void randomNodeStarter(DataReader* reader, std::string file_name, long node1=-1, long node2=-1, long iterations=-1);
};
#endif  // INCLUDE_TSPALGORITHM_H_
