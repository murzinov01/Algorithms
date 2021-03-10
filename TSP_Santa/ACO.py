import math
import random
import tsp_visualization as tspv


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

    def get_random_start_vertex(self):
        return list(self.vertexes.keys())[random.randint(0, len(self.vertexes)) - 1]

    def search(self):
        for iteration in range(self.ant_count):
            start_vertex = 0
            if self.init == "random":
                start_vertex = self.get_random_start_vertex()
            ant = Ant(self, start_vertex=start_vertex)
            ant.run()

    def get_pheromone(self, index: tuple):
        new_index = index if index[0] <= index[1] else (index[1], index[0])
        try:
            return self.pheromones[new_index]
        except KeyError:
            return 0

    def add_pheromone(self, index: tuple, value: float):
        new_index = index if index[0] <= index[1] else (index[1], index[0])
        self.pheromones[new_index] = value

    def recalculate_pheromones(self, path: list):
        edges = tspv.create_edges_from_path(path)

        for key in self.pheromones.keys():
            self.pheromones[key] = self.pheromones[key] * self._p
        for edge in edges:
            self.add_pheromone(index=edge, value=self.get_pheromone(edge) + self._q / self.get_distance(edge))

    def get_distance(self, index: tuple):
        x1, y1 = self.vertexes[index[0]]
        x2, y2 = self.vertexes[index[1]]
        return math.sqrt((x1 - x2)**2 + (y1 - y2)**2)

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

    def get_prob_numerator(self, index: tuple):
        return self.ant_colony.get_pheromone(index)**self.ant_colony.get_a() +\
            1 / (self.ant_colony.get_distance(index))**self.ant_colony.get_b()

    def calc_prob_denominator(self):
        result = 0
        vertexes = set(self.ant_colony.vertexes.keys()) - {self.start_vertex}
        for vertex in vertexes:
            result += self.get_prob_numerator((self.start_vertex, vertex))
        return result

    def calc_probability(self, index: tuple):
        return self.get_prob_numerator(index) / self.prob_denominator

    def choose_next_vertex(self):
        sum_ = 0
        rand_point = random.random()
        for vertex in self.ant_colony.vertexes.keys():
            if not self.path_memory[vertex]:
                prob = self.calc_probability((self.start_vertex, vertex))
                sum_ += prob
                if sum_ > rand_point:
                    return vertex
        return list(self.ant_colony.vertexes.keys())[-1]

    def run(self):
        path = [self.start_vertex]
        for i in range(len(self.ant_colony.vertexes.keys()) - 1):
            next_vertex = self.choose_next_vertex()
            self.path_memory[next_vertex] = True
            self.start_vertex = next_vertex
            path.append(next_vertex)

        # self.ant_colony.calc_path_distance(path)

        return path


def main():
    ant_colony = AntColony(1, tspv.read_tsp("cities.csv", top_banner=1, delimiter=','), a=0.5, b=0.5, p=0.5)
    ant_colony.search()


if __name__ == '__main__':
    main()
