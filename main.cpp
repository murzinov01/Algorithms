#include "TSPAlgorithm.h"


int main() {
  DataReader reader("980.txt");
  TSP tsp(reader.dist_matrix, reader.dist_pseudo_matrix, reader.node_num);
  tsp.first_step = true;

  std::thread t1(&TSP::randomNodeStarter, tsp, &reader, "Result1.txt", 0, 3, -1);
  std::thread t2(&TSP::randomNodeStarter, tsp, &reader, "Result2.txt", 3, 5, -1);
  std::thread t3(&TSP::randomNodeStarter, tsp, &reader, "Result3.txt", 5, 7, -1);

  t1.join();
  t2.join();
  t3.join();

  //tsp.randomNodeStarter(&reader, 1, 3, 40);

  //tsp.createInitialDecision(12);
  //tsp.iteratedLocalSearch();

  //reader.saveGraphEdges(tsp.getPath(), "Edges.txt");
  return 0;
}
