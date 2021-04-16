# from Algos.data_parser import DataParser
from data_parser import DataParser
from scipy.spatial import distance
import itertools



class TabuVRP:

    class TabuContainer:
        def __init__(self, x, counter: int):
            self.x = x
            self.counter = counter

        def dec_counter(self):
            self.counter -= 1
            return self.counter

        def compare(self, compare_x):
            return compare_x == self.x

    def __init__(self, tabu_lifetime=10):
        self.tabu_list = list()
        self.decision_ = list()
        self.trucks_ = None
        self.shops_ = None
        self.truck_capacity_ = None
        self.data_ = None
        self.distance_matrix_ = None

    def _check_valid_decision(self, decision):
        pass

    def move_customer(self, current_routes):
        pass

    def _fitness(self, y):
        pass

    def _set_params(self, x):
        self.trucks_ = x['trucks']
        self.shops_ = x['shops']
        self.truck_capacity_ = x['truck_capacity']
        self.data_ = x['data']

    def fit(self, x: dict):
        """
        Minimize objective function of VRP
        :param x: dict ("trucks": int, "shops": int, "truck_capacity": int, "data": pd.DataFrame)
        :return: self
        """
        self._set_params(x)
        self.distance_matrix_ = distance.squareform(
            distance.pdist(self.data_[['X', 'Y']], metric='euclidean'))

    def transform_decision(self, y):
        pass


def main():
    parser = DataParser("I1.txt")
    print(parser.get_data_frame())
    my_tabu = TabuVRP()
    my_tabu.fit(parser.get_params())


if __name__ == '__main__':
    main()
