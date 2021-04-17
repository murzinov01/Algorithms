from Algos.data_parser import DataParser
# from data_parser import DataParser
from scipy.spatial import distance
import functools


def square_func(init, exponent):
    """
    Squares an init number exponent-multiple times (recursion)
    :param init: init number to square
    :param exponent: iterations of square
    :return: int
    """
    if exponent <= 1:
        return init
    return square_func(init, exponent - 1) ** 2


def power_of_two(exponent):
    return 2 ** exponent


class TabuVRP:
    class TabuContainer:
        def __init__(self, x, counter: int):
            self.x = x
            self.counter = counter

        def set_lifetime(self, lifetime: int):
            self.counter = lifetime

        def dec_lifetime(self) -> int:
            self.counter -= 1
            return self.counter

        def compare(self, compare_x) -> bool:
            return compare_x == self.x

        def __eq__(self, other):
            return self.x == other.x

    def __init__(self, tabu_lifetime=10):
        self.decision_ = list()
        self.trucks_ = None
        self.shops_ = None
        self.truck_capacity_ = None
        self.data_ = None
        self.distance_matrix_ = None
        # protected
        self._tabu_list = list()
        self._lateness_penalty_coefficient = 1
        self._capacity_overload_penalty_coefficient = 1
        self._invalid_counter = 0

    def _estimate_excess(self, y) -> (float, int):
        """
        Estimate decision's overall time delay and capacity overload
        :param y: nested list of routes (for each vehicle)
        :return: lateness(float),
        """
        lateness = 0.0
        capacity_overload = 0
        for route in y:
            route_time = 0.0
            route_demand = 0
            for node1, node2 in zip(route, route[1:]):
                route_time += self.distance_matrix_[node1, node2]
                route_demand += self.data_['Demand'][node2]
                opening_time, closing_time = self.data_[['TimeOpen', 'TimeClose']].iloc[node2]
                if route_time < opening_time:
                    # wait for opening
                    route_time = opening_time
                elif route_time > closing_time:
                    # we're late
                    lateness += route_time - closing_time
                route_time += self.data_['Unloading'][node2]

            if route_demand > self.truck_capacity_:
                capacity_overload += route_demand - self.truck_capacity_

        return lateness, capacity_overload

    def _estimate_penalty(self, lateness, capacity_overload, stimulating_func, *args, **kwargs) -> float:
        """
        Estimate penalty for invalid decision
        :param lateness: overall lateness for the decision
        :param capacity_overload: overall overload of vehicle capacity for the decision
        :return: penalty(float)
        """
        decision_penalty = lateness * self._lateness_penalty_coefficient \
            + capacity_overload * self._capacity_overload_penalty_coefficient
        stimulating_penalty = 0.0
        if decision_penalty > 0:
            self._invalid_counter += 1
            kwargs['exponent'] = self._invalid_counter
            stimulating_penalty = stimulating_func(*args, **kwargs)
        else:
            self._invalid_counter = 0
        return float(stimulating_penalty)
        # ANOTHER VARIANT OF PENALTY
        # return float(decision_penalty + stimulating_penalty)

    def _check_all_shops_visited(self, y) -> (bool, bool):
        """
        Check if all shops have been visited in decision
        :param y: nested list of routes (for each vehicle)
        :return: is_all_shops_visited(bool), is_all_shops_visited_once(bool)
        """
        all_shops = set(self.data_['Id'])
        visited_shops = []
        for route in y:
            visited_shops += route
        visited_shops_set = set(visited_shops)
        dif = all_shops - visited_shops_set
        visited_more_once = visited_shops[:]
        for shop in visited_shops_set:
            visited_more_once.remove(shop)
        visited_more_once = list(filter(lambda a: a != 0, visited_more_once))
        if len(dif) != 0:
            print("*LOG*: shops NOT visited", dif)
        if len(visited_more_once) != 0:
            print("*LOG*: shops visited MORE than ONCE", visited_more_once)
        return len(dif) == 0, len(visited_more_once) == 0

    def _check_valid_decision(self, y):
        """
        Check if the decision of VRP is valid
        :param y: nested list of routes (for each vehicle)
        :return: is_valid(bool), amount_of_penalty(float), result_data(dict)
        """
        lateness, capacity_overload = self._estimate_excess(y)
        penalties = self._estimate_penalty(lateness, capacity_overload, power_of_two)
        penalty_valid = False if penalties != 0.0 else True
        vehicle_valid = False if len(y) <= self.trucks_ else True
        is_all_shops_visited, is_all_shops_visited_once = self._check_all_shops_visited(y)
        start_end_valid = functools.reduce(lambda x1, x2: x1 and x2,
                                           [route[0] == route[-1] == 0 for route in y])
        reasons = [penalty_valid, vehicle_valid, is_all_shops_visited,
                   is_all_shops_visited_once, start_end_valid]
        reasons_str = ['penalty_valid', 'vehicle_valid', 'is_all_shops_visited',
                       'is_all_shops_visited_once', 'start_end_valid']
        result_data = {
            "lateness": lateness,
            "capacity_overload": capacity_overload,
            "reason": None
        }
        is_valid = functools.reduce(lambda x1, x2: x1 and x2, reasons)
        if not is_valid:
            result_data['reason'] = {string: reason for reason, string in zip(reasons, reasons_str)}
        return is_valid, penalties, result_data

    @staticmethod
    def move_customer(y):
        neighborhood = []
        # concatenate all routes in to one path, delete double zero
        common_path = list(functools.reduce(lambda x1, x2: x1[:-1] + x2, y))
        length = len(common_path)
        for vertex_index in range(length):
            moved_value = common_path[vertex_index]
            # we do not move the 0
            if moved_value == 0:
                continue
            for gap_index in range(length-1):
                # gap_index = after what vertex that gap is located
                candidate = dict()
                new_common_path = common_path[:vertex_index] + common_path[vertex_index + 1 :]
                new_common_path.insert(gap_index, moved_value)
                delete_edges = []
                delete_edges.append()




    def _fitness(self, y) -> float:
        """
        Objective function od VRP algorithm
        :param y: nested list of routes (each vehicle)
        :return: summarized distance of all routes with time of unloading (float)
        """
        objective_value = 0
        for route in y:
            for node1, node2 in zip(route, route[1:]):
                node_distance = self.distance_matrix_[node1, node2]
                unloading = self.data_['Unloading'][node2]
                # print(f'Node {node1}, {node2}:', node_distance, unloading)
                objective_value += node_distance + unloading
        return float(objective_value)

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
    # print(square_func(init=2, exponent=1))
    # parser = DataParser("I1.txt")
    # print(parser.get_data_frame())
    # my_tabu = TabuVRP()
    # my_tabu.fit(parser.get_params())
    y = [[0, 1, 3, 0], [0, 5, 10, 0], [0, 4, 6, 0]]
    # obj_f = my_tabu._fitness(y)
    # valid, penalty, result_data = my_tabu._check_valid_decision(y)
    # print(obj_f)
    # print(valid, penalty, result_data)
    #
    # route1 = [0, 1, 2, 3, 0, 4, 5, 6, 0]
    # edges1 = [(node1, node2) for node1, node2 in zip(route1, route1[1:])]
    # route2 = [0, 2, 3, 0, 1, 4, 5, 6, 0]
    # edges2 = [(node1, node2) for node1, node2 in zip(route2, route2[1:])]
    # print(edges1)
    # print(edges2)
    # print("Deleted:", set(edges1) - set(edges2))
    # print("Added:", set(edges2) - set(edges1))


if __name__ == '__main__':
    main()
