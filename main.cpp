#include "TSPAlgorithm.h"


int main() {
  DataReader reader("980.txt");
  TSP tsp(reader.dist_matrix, reader.dist_pseudo_matrix, reader.node_num);
  tsp.first_step = false;

  tsp.randomNodeStarter(3, 40);

  //tsp.createInitialDecision(12);
  //tsp.iteratedLocalSearch();

  reader.saveGraphEdges(tsp.getPath(), "Edges.txt");
  return 0;
}
