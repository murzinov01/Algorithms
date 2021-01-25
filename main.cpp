#include "TSPAlgorithm.h"


int main() {
  DataReader reader("48tsp.txt");
  TSP tsp(reader.dist_matrix, reader.node_num);
  //bool** path = 
  tsp.first_step = true;
  tsp.createInitialDecision();

  tsp.printMatrixDist();
  double** matrix_dist = tsp.getDistMatrix();
  std::cout << "PREV: " << matrix_dist[1][28] << " NEW: " << matrix_dist[1][47] << std::endl;
  std::cout << "PREV: " << matrix_dist[47][38] << " NEW: " << matrix_dist[28][38] << std::endl;
  //bool** matrix_path = tsp.getMatrixPath();

  //for (long i = 0; i < reader.node_num; i++) {
  //  for (long j = 0; j < reader.node_num; j++) {
  //    std::cout << matrix_path[i][j] << " ";
  //  }
  //  std::cout << std::endl;
  //}

  //for (unsigned i = 0; i < reader.node_num; i++)
  //  for (unsigned j = i + 1; j < reader.node_num; j++)
  //    if (path[i][j]) {
  //      std::cout << reader.data[i].id << " " << reader.data[j].id << std::endl;
  //    }


  
  //reader.printGraphEdges(tsp.getMatrixPath());
  //reader.saveGraphEdges(tsp.getMatrixPath(), "Edges.txt");
  std::cout << "INITIAL PATH: " << std::endl;
  tsp.printDecisionPath();
  //tsp.printMatrixPath();
  std::cout << "INITIAL COST: " << tsp.getPathCost() << std::endl;
  std::cout << std::endl;

  tsp.iteratedLocalSearch();
  tsp.createMatrixPath();
  //tsp.printMatrixPath();
  reader.saveGraphEdges(tsp.getMatrixPath(), "Edges.txt");
  return 0;
}
