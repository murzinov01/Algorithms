import math
import random
import sys
from Algos import tsp_visualization as tspv


# import tsp_visualization as tspv

class AntColony:
    """
    Params:
    a: float - Information Elicitation Factor
    b: float - Expected Heuristic Factor
    p: float - Pheromone Evaporation Coefficient
    q: float - Pheromone Intensity
    """

    def __init__(self, ant_count: int, vertexes: dict, a: float, b: float, p: float, q=1.0, init="random"):
        self.ant_count = ant_count
        self.vertexes = vertexes
        self.pheromones = dict()
        self.init = init
        self._a = a
        self._b = b
        self._p = p
        self._q = q

        self.dist_memory = list()
        self.best_path = list()
        self.best_dist = sys.float_info.max

    def get_random_start_vertex(self):
        return list(self.vertexes.keys())[random.randint(0, len(self.vertexes)) - 1]

    def search(self):
        for iteration in range(self.ant_count):
            start_vertex = 1
            if iteration == 1:
                print('ENTER')
            if self.init == "random":
                start_vertex = self.get_random_start_vertex()
            ant = Ant(self, start_vertex=start_vertex)
            path, dist = ant.run()

            self.dist_memory.append(dist)
            if dist < self.best_dist:
                self.best_dist = dist
                self.best_path = path
            print('\nIteration:', iteration, ':', dist)
            # tspv.visualize_tsp(self.vertexes, path, show=True)

        tspv.save_answer_csv(self.best_path, headers=('Path',))
        print('BEST:', self.best_dist)
        # tspv.visualize_tsp(self.vertexes, self.best_path)

    def get_pheromone(self, index: tuple):
        new_index = index if index[0] <= index[1] else (index[1], index[0])
        try:
            return self.pheromones[new_index]
        except KeyError:
            return 0

    def add_pheromone(self, index: tuple, value: float):
        new_index = index if index[0] <= index[1] else (index[1], index[0])
        self.pheromones[new_index] = value

    def recalculate_pheromones(self, edges: list, path_distance: float):
        # evaporation
        for key in self.pheromones.keys():
            self.pheromones[key] = self.pheromones[key] * self._p
        # new layer
        for edge in edges:
            self.add_pheromone(index=edge, value=self.get_pheromone(edge) + self._q / path_distance)

        print(self.pheromones)

    def get_distance(self, index: tuple):
        x1, y1 = self.vertexes[index[0]]
        x2, y2 = self.vertexes[index[1]]
        dist = math.sqrt((x1 - x2) ** 2 + (y1 - y2) ** 2)
        # if dist == 0:
        #     return sys.float_info.min
        # else:
        return dist

    def calc_path_distance(self, edges: list) -> float:
        dist = 0
        for edge in edges:
            dist += self.get_distance(edge)
        return dist

    def get_a(self):
        return self._a

    def get_b(self):
        return self._b

    def get_p(self):
        return self._p

    def get_q(self):
        return self._q


class Ant:
    def __init__(self, ant_colony: AntColony, start_vertex: int):
        self.path_memory = {i: False for i in ant_colony.vertexes.keys()}
        self.path_memory[start_vertex] = True
        self.ant_colony = ant_colony
        self.start_vertex = start_vertex
        self.prob_denominator = self.calc_prob_denominator()

    def calc_prob_numerator(self, index: tuple):
        div = (self.ant_colony.get_distance(index)) ** self.ant_colony.get_b()
        if div == 0.0:
            div = sys.float_info.min
        return self.ant_colony.get_pheromone(index) ** self.ant_colony.get_a() + 1 / div

    def calc_prob_denominator(self):
        result = 0
        vertexes = set(self.ant_colony.vertexes.keys()) - \
            set(key for key in self.path_memory.keys() if self.path_memory[key])
        for vertex in vertexes:
            result += self.calc_prob_numerator((self.start_vertex, vertex))
        return result

    def calc_probability(self, index: tuple):
        return self.calc_prob_numerator(index) / self.prob_denominator

    def choose_next_vertex(self):
        sum_ = 0
        rand_point = random.random()
        available_vertexes = set(self.ant_colony.vertexes.keys()) - \
            set(key for key in self.path_memory.keys() if self.path_memory[key])
        for vertex in available_vertexes:
            prob = self.calc_probability((self.start_vertex, vertex))
            sum_ += prob
            if sum_ > rand_point:
                # print("Choice:", vertex, '(' + str(prob) + ')', 'when', rand_point)
                # print("Den:", self.prob_denominator)
                return vertex
        return list(available_vertexes)[-1]

    def run(self):
        path = [self.start_vertex]
        for i in range(len(self.ant_colony.vertexes.keys()) - 1):
            # print(i, end=', ')
            # print("I'm on", self.start_vertex)
            next_vertex = self.choose_next_vertex()
            self.path_memory[next_vertex] = True
            self.start_vertex = next_vertex
            path.append(next_vertex)
            self.prob_denominator = self.calc_prob_denominator()
            # tspv.visualize_tsp(self.ant_colony.vertexes, path, with_labels=True, dpi=500, show=True)
            # print("I go to", next_vertex)

        # print("Path:", path)
        edges = tspv.create_edges_from_path(path)
        # print("Edges:", edges)
        dist = self.ant_colony.calc_path_distance(edges)
        self.ant_colony.recalculate_pheromones(edges, dist)
        return path, dist


def main():
    # ant_colony = AntColony(1, tspv.read_tsp("cities.csv", top_banner=1, delimiter=','), a=0.5, b=0.5, p=0.5)
    ant_colony = AntColony(1000, tspv.read_tsp("/Users/sanduser/PycharmProjects/ML/Algos/dj38.tsp",
                                             top_banner=0, delimiter=' '), a=1, b=2, p=0.5, q=6656)
    ant_colony.search()


if __name__ == '__main__':
    main()
