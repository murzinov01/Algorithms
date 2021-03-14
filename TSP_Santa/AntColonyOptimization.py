import tsp_visualization as tspv
import numpy as np
import math
import random


class ACO:
    def __init__(self, iterations: int, ants_num: int,
                 evaporation: float,  pheromone_increase: float,
                 alpha=1.0, beta=0.0,
                 beta_evaporation=0.0, prob_of_choose_best=0.1) -> None:
        self.iterations = iterations
        self.ants_num = ants_num
        self._evaporation = evaporation
        self._pheromone_increase = pheromone_increase
        self._alpha = alpha
        self._beta = beta
        self._beta_evaporation = beta_evaporation
        self._prob_of_choose_best = prob_of_choose_best

        self.pheromone_matrix = None
        self.distance_matrix = None
        self.reversed_distance_matrix = None
        self.probability_matrix = None
        self.vertexes_num = 0
        self.available_vertexes_num = 0
        self.best_path = None
        self.best_score = 10000000.0
        self.available_vertexes = list()

    def fit(self, distance_matrix) -> None:
        self.distance_matrix = distance_matrix
        self._initialize()

    def _initialize(self) -> None:
        self.vertexes_num = len(self.distance_matrix)
        self.pheromone_matrix = [[0.0 for j in range(self.vertexes_num)] for i in range(self.vertexes_num)]
        self.probability_matrix = [[0.0 for j in range(self.vertexes_num)] for i in range(self.vertexes_num)]
        self.reversed_distance_matrix = [[0.0 for j in range(self.vertexes_num)] for i in range(self.vertexes_num)]

        for i in range(self.vertexes_num):
            for j in range(self.vertexes_num):
                if self.distance_matrix[i][j] == 0:
                    self.reversed_distance_matrix[i][j] = 1
                else:
                    self.reversed_distance_matrix[i][j] = 1 / self.distance_matrix[i][j]
                if i != j:
                    self.pheromone_matrix[i][j] = 1.0

        self._recalculate_probabilities()
        self._update_available_vertexes()

    def _recalculate_probabilities(self) -> None:
        for i in range(self.vertexes_num):
            for j in range(self.vertexes_num):
                self.probability_matrix[i][j] = (self.pheromone_matrix[i][j] ** self._alpha) *\
                                                (self.reversed_distance_matrix[i][j] ** self._beta)

    def _update_available_vertexes(self) -> None:
        self.available_vertexes = list(range(1, self.vertexes_num + 1))
        self.available_vertexes_num = len(self.available_vertexes)

    def remove_available_vertex(self, value: int) -> None:
        # print("REMOVE:", value, " FROM:", self.available_vertexes)
        self.available_vertexes.remove(value)
        self.available_vertexes_num -= 1

    def _choose_next_vertex(self, start_vertex: int) -> int:
        probs_to_available_vertexes = [self.probability_matrix[start_vertex - 1][value - 1] for value in self.available_vertexes]
        max_prob_index = None
        max_prob = 0
        # Find max prob
        for i in range(len(probs_to_available_vertexes)):
            if probs_to_available_vertexes[i] > max_prob:
                max_prob = probs_to_available_vertexes[i]
                max_prob_index = i

        p = random.random()
        if p < self._prob_of_choose_best:
            next_vertex = self.available_vertexes[max_prob_index]
        else:
            denominator = sum(probs_to_available_vertexes)
            probabilities = [self.probability_matrix[start_vertex - 1][self.available_vertexes[i] - 1] / denominator for i in range(len(self.available_vertexes))]
            # if self.available_vertexes_num == 0:
            #     probabilities = [1]
            # print(probabilities)
            next_vertex = self.available_vertexes[np.random.choice(range(len(probabilities)), p=probabilities)]
        return next_vertex

    def _get_path_distance(self, path: list) -> float:
        path_len = len(path)
        score = 0.0
        for i in range(path_len - 1):
            score += self.distance_matrix[path[i] - 1][path[i + 1] - 1]
            # print("SCORE:", score)
        score += self.distance_matrix[path[path_len - 1] - 1][path[0] - 1]
        return score

    def _get_best_path_and_score(self, paths: list) -> tuple:
        best_path = paths[0]
        best_score = self._get_path_distance(paths[0])
        for i in range(1, len(paths)):
            score = self._get_path_distance(paths[i])
            if score < best_score:
                best_score = score
                best_path = paths[i]
        return best_path, best_score

    def _evaporate_pheromones(self) -> None:
        for i in range(self.vertexes_num):
            for j in range(self.vertexes_num):
                self.pheromone_matrix[i][j] *= (1 - self._evaporation)
                self.reversed_distance_matrix[i][j] *= (1 - self._beta_evaporation)

    def _increase_pheromones(self, best_path: list) -> None:
        for i in range(len(best_path) - 1):
            self.pheromone_matrix[best_path[i] - 1][best_path[i + 1] - 1] += self._pheromone_increase

    def _recalculate_pheromones(self, best_path: list) -> None:
        self._evaporate_pheromones()
        self._increase_pheromones(best_path)

    def run(self) -> tuple:
        number_of_equal_iterations = 0
        for i in range(self.iterations):
            paths = list()
            cur_path = list()
            print("ITERATION:", i)
            for ant in range(self.ants_num):
                # print("ANT:", ant)
                start_vertex = random.randint(0, self.available_vertexes_num - 1) + 1
                first_vertex = start_vertex
                while True:
                    cur_path.append(start_vertex)
                    self.remove_available_vertex(start_vertex)
                    if self.available_vertexes_num == 0:
                        break
                    start_vertex = self._choose_next_vertex(start_vertex)
                    # start_vertex = self.available_vertexes[next_vertex_i]
                self._update_available_vertexes()
                cur_path.append(first_vertex)
                paths.append(cur_path)
                cur_path = list()
            best_path, best_score = self._get_best_path_and_score(paths)

            if best_score == self.best_score:
                number_of_equal_iterations += 1
            else:
                number_of_equal_iterations = 0

            print("BEST SCORE:", best_score)

            if best_score < self.best_score:
                self.best_path = best_path
                self.best_score = best_score

            self._recalculate_pheromones(best_path)
            self._recalculate_probabilities()
            # add exit criterion

            if number_of_equal_iterations > 10:
                print("EXIT: There are more than 10 equal iterations")
                return self.best_path, self.best_score

        return self.best_path, self.best_score

    @staticmethod
    def get_distance(crds1: tuple, crds2: tuple) -> float:
        return math.sqrt((crds1[0] - crds2[0]) ** 2 + (crds1[1] - crds2[1]) ** 2)


def main():
    points = tspv.read_tsp("lu980.tsp", top_banner=0, delimiter=' ')
    size = len(points.keys())
    problem = [[0.0 for j in range(size)] for i in range(size)]
    keys_list = list(points.keys())
    for i in range(size - 1):
        for j in range(i + 1, size):
            problem[i][j] = ACO.get_distance(points[keys_list[i]], points[keys_list[j]])
            problem[j][i] = problem[i][j]

    colony = ACO(iterations=100, ants_num=100, evaporation=.1, pheromone_increase=2, alpha=1, beta=1,
                 beta_evaporation=0.01, prob_of_choose_best=.1)
    colony.fit(problem)
    best_path, best_score = colony.run()

    print("BEST SCORE:", best_score, " BEST PATH:", best_path)

    path = [value for value in best_path[:-2]]
    tspv.visualize_tsp(points, path=path)


if __name__ == '__main__':
    main()
