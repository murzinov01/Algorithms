#include "TSPAlgorithm.h"
#include <Windows.h>


std::string findMinResult() {
  WIN32_FIND_DATAW wfd;

  std::string dir_name = "C:\\Users\\user\\source\\repos\\Local Search\\Local Search\\Mona\\";

  HANDLE const hFind = FindFirstFileW(L"C:\\Users\\user\\source\\repos\\Local Search\\Local Search\\Mona\\*Result*", &wfd);

  double best_cost = std::numeric_limits<double>::infinity();
  std::string best_file_name;

  if (INVALID_HANDLE_VALUE != hFind)
  {
    do
    {
      std::wstring ws(&wfd.cFileName[0]);
      std::string file_name(ws.begin(), ws.end());
      std::ifstream file(dir_name + file_name);
      std::string str;

      char min_cost[50] = { '\0' };
      std::getline(file, str);
      int i = str.size() - 1, j = 0;
      while (str[i] != ' ') {
        min_cost[j++] = str[i--];
      }
      int dec = 0, size = j - 1;
      char reverse_cost[50] = { '\0' };
      for (int i = 0; i <= size; i++) {
        reverse_cost[i] = min_cost[size - dec];
        dec++;
      }

      double cost = std::atof(reverse_cost);

      if (cost < best_cost) {
        best_cost = cost;
        best_file_name = file_name;
      }
    } while (NULL != FindNextFileW(hFind, &wfd));

    FindClose(hFind);
  }

  return best_file_name;
}

void FinderMona() {
  DataReader reader("mona_1000.txt");
  TSP tsp(reader.dist_matrix, reader.dist_pseudo_matrix, reader.node_num);
  tsp.first_step = false;
  std::string dir_name = "C:\\Users\\user\\source\\repos\\Local Search\\Local Search\\Mona\\";
  tsp.randomNodeStarter(&reader, "Result.txt", dir_name, 2, 1000, -1);
  //std::thread t1(&TSP::randomNodeStarter, tsp, &reader, "Result.txt", dir_name, 2, 1000, -1);
  //t1.join();
}

void FinderLu() {
  DataReader reader("lu980.txt");
  TSP tsp(reader.dist_matrix, reader.dist_pseudo_matrix, reader.node_num);
  tsp.first_step = false;
  std::string dir_name = "C:\\Users\\user\\source\\repos\\Local Search\\Local Search\\Lu980\\";
  tsp.randomNodeStarter(&reader, "Result.txt", dir_name, 0, 980, -1);
  //std::thread t2(&TSP::randomNodeStarter, tsp, &reader, "Result.txt", dir_name, 0, 980, -1);
}

void FinderJa() {
  DataReader reader("ja_1000.txt");
  TSP tsp(reader.dist_matrix, reader.dist_pseudo_matrix, reader.node_num);
  tsp.first_step = false;
  std::string dir_name = "C:\\Users\\user\\source\\repos\\Local Search\\Local Search\\Ja1000\\";
  tsp.randomNodeStarter(&reader, "Result.txt", dir_name, 0, 1000, -1);
  //std::thread t3(&TSP::randomNodeStarter, tsp, &reader, "Result.txt", dir_name, 0, 1000, -1);
  //t3.join();
}

void FinderRandom1() {
  DataReader reader("random_1.txt");
  TSP tsp(reader.dist_matrix, reader.dist_pseudo_matrix, reader.node_num);
  tsp.first_step = false;
  std::string dir_name = "C:\\Users\\user\\source\\repos\\Local Search\\Local Search\\Random1\\";
  tsp.randomNodeStarter(&reader, "Result.txt", dir_name, 0, 703, -1);
  //std::thread t4(&TSP::randomNodeStarter, tsp, &reader, "Result.txt", dir_name, 0, 703, -1);
}

void FinderRandom2() {
  DataReader reader("random_2.txt");
  TSP tsp(reader.dist_matrix, reader.dist_pseudo_matrix, reader.node_num);
  tsp.first_step = false;
  std::string dir_name = "C:\\Users\\user\\source\\repos\\Local Search\\Local Search\\Random2\\";
  tsp.randomNodeStarter(&reader, "Result.txt", dir_name, 0, 703, -1);
  //std::thread t5(&TSP::randomNodeStarter, tsp, &reader, "Result.txt", dir_name, 0, 703, -1);
  //t5.join();
}

void FinderRandom3() {
  DataReader reader("random_3.txt");
  TSP tsp(reader.dist_matrix, reader.dist_pseudo_matrix, reader.node_num);
  tsp.first_step = false;
  std::string dir_name = "C:\\Users\\user\\source\\repos\\Local Search\\Local Search\\Random3\\";
  tsp.randomNodeStarter(&reader, "Result.txt", dir_name, 0, 990, -1);
  //std::thread t6(&TSP::randomNodeStarter, tsp, &reader, "Result.txt", dir_name, 0, 990, -1);
  //t6.join();
}

int main() {
  DataReader reader("mona_1000.txt");
  TSP tsp(reader.dist_matrix, reader.dist_pseudo_matrix, reader.node_num);
  tsp.first_step = false;
  std::string dir_name = "C:\\Users\\user\\source\\repos\\Local Search\\Local Search\\Mona\\";
  //tsp.randomNodeStarter(&reader, "Result.txt", dir_name, 2, 1000, -1);
  //std::thread t1 (FinderMona);
  //std::thread t2 (FinderLu);
  //std::thread t3 (FinderJa);
  //std::thread t4 (FinderRandom1);
  //std::thread t5 (FinderRandom2);
  //std::thread t6 (FinderRandom3);
  //t1.join();
  //t2.join();
  //t3.join();
  //t4.join();
  //t5.join();
  //t6.join();
  //FinderJa();
  //FinderRandom1();
  //FinderRandom2();
  //FinderRandom3();
  //DataReader reader("ja_1000.txt");
  //TSP tsp(reader.dist_matrix, reader.dist_pseudo_matrix, reader.node_num);
  //tsp.first_step = false;

  //std::cout << "BEST RESULT: " << findMinResult() << std::endl;

  // tsp.randomNodeStarter(&reader, "Result.txt", 122, 123, -1);
  //tsp.finBestGreedy(1000);
  //tsp.iteratedLocalSearch(&reader, "FROM_BEST_GREEDY.txt", -1);
  std::thread t1(&TSP::randomNodeStarter, tsp, &reader, "Result.txt", dir_name, 98, 231, -1);
  std::thread t2(&TSP::randomNodeStarter, tsp, &reader, "Result.txt", dir_name, 248, 381, -1);
  std::thread t3(&TSP::randomNodeStarter, tsp, &reader, "Result.txt", dir_name, 398, 531, -1);

  t1.join();
  t2.join();
  t3.join();
  //reader.saveGraphEdges(tsp.getPath(), "Edges.txt");
  return 0;
}
