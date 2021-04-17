import sys
import math
from data_parser import DataParser
from scipy.spatial import distance
import functools
import tsp_visualization as tspv


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
        self.trucks_ = None
        self.shops_ = None
        self.truck_capacity_ = None
        self.data_ = None
        self.distance_matrix_ = None

        # protected
        self._best_decision = list()
        self._tabu_list = list()
        self._lateness_penalty_coefficient = 1
        self._capacity_overload_penalty_coefficient = 1
        self._invalid_counter = 0

        # Storage
        self._storage_time_close = 0
        self._storage_y = 0
        self._storage_x = 0

        # Initial decision
        self._initial_decision = None
        self._init_truck_number = 0

    def get_initial_decision(self):
        return self._initial_decision

    def get_best_decision(self):
        return self._best_decision

    def _set_params(self, x):
        self.trucks_ = x['trucks']
        self.shops_ = x['shops']
        self.truck_capacity_ = x['truck_capacity']
        self.data_ = x['data']

    @staticmethod
    def _save_decision(file_name: str, decision: list) -> None:
        with open(file_name, 'w', encoding='utf-8') as file:
            for path in decision:
                # print(", ".join([str(el) for el in path]) + '\n')
                file.write(", ".join([str(el) for el in path]) + '\n')

    def _create_initial_decision(self, mode="greedy"):
        if mode == "greedy":
            self._initial_decision, self._init_truck_number = self._init_greedy()
            # print(self.initial_decision, self.init_truck_number)

    def _dist_to_storage(self, customer):
        x_c, y_c = tuple(self.data_[["X", "Y"]].iloc[customer])
        return math.sqrt((y_c - self._storage_y) ** 2 + (x_c - self._storage_x) ** 2)

    def _find_next_node(self, customer, time, capacity, path_memory) -> tuple:
        distances = self.distance_matrix_[customer]
        min_dist = sys.float_info.max
        min_customer = -1

        for next_customer, dist in enumerate(distances):
            if not path_memory[next_customer]:
                time_open = self.data_["TimeOpen"][next_customer]
                if time + dist < time_open:
                    dist += time_open - time - dist
                if dist < min_dist \
                    and time + self._dist_to_storage(next_customer) + dist < self.data_["TimeClose"][0] \
                    and time + dist < self.data_["TimeClose"][next_customer] \
                        and capacity > self.data_["Demand"][next_customer]:
                    min_dist = dist
                    min_customer = next_customer

        return min_customer, min_dist

    def _find_next_invalid_node(self, customer, path_memory) -> tuple:
        distances = self.distance_matrix_[customer]
        min_dist = sys.float_info.max
        min_customer = -1

        for next_customer, dist in enumerate(distances):
            if not path_memory[next_customer] and dist < min_dist:
                min_dist = dist
                min_customer = next_customer
        return min_customer, min_dist

    def _init_greedy(self):
        self.storage_time_close = self.data_["TimeClose"][0]
        self._storage_x, self._storage_y = tuple(self.data_[["X", "Y"]].iloc[0])
        decisions = list()

        # Create decisions for all number of trucks
        for truck_num in range(1, self.trucks_ + 1):
            invalid_count = 0
            add_invalid = False
            print(f"Greedy with truck number = {truck_num}")
            decision = [{"path": [0], "cost": 0, "capacity": self.truck_capacity_, "invalid_count": 0}
                        for i in range(truck_num)]
            visited_customers = [False for i in range(self.shops_)]
            visited_counter = self.shops_
            visited_customers[0] = True
            visited_counter -= 1

            # Create path for each truck
            while visited_counter > 0:
                for i in range(truck_num):
                    # Find the next customer
                    if not add_invalid:
                        target_node, dist = self._find_next_node(decision[i]["path"][-1],
                                                                 decision[i]["cost"],
                                                                 decision[i]["capacity"],
                                                                 visited_customers)
                    else:
                        target_node, dist = self._find_next_invalid_node(decision[i]["path"][-1],
                                                                         visited_customers)
                        decision[i]["invalid_count"] += 1

                    if target_node == -1:
                        invalid_count += 1
                        continue

                    visited_customers[target_node] = True
                    visited_counter -= 1
                    decision[i]["path"].append(target_node)
                    decision[i]["cost"] += dist + self.data_["Unloading"][target_node]
                    decision[i]["capacity"] -= self.data_["Demand"][target_node]
                    if visited_counter == 0:
                        break
                if invalid_count >= truck_num:
                    add_invalid = True
                else:
                    invalid_count = 0

            # Go to the storage
            for i in range(truck_num):
                decision[i]["cost"] += self._dist_to_storage(decision[i]["path"][-1])
                decision[i]["path"].append(0)
            decisions.append(decision)

        # Find the best decision
        return self._find_best_decision(decisions)

    @staticmethod
    def _find_best_decision(decisions: list):
        best_decision = []
        best_fitness = sys.float_info.max
        best_truck_number = 0

        for i, decision in enumerate(decisions):
            invalid_count = sum([decision[i]['invalid_count'] for i in range(len(decision))])
            if invalid_count > 0:
                continue

            fitness = sum([decision[i]["cost"] for i in range(len(decision))])

            # print(f"Best Fitness={best_fitness}, fitness={fitness},"
            #       f" invalid_count={sum([decision[i]['invalid_count'] for i in range(len(decision))])},"
            #      f"truck_number={i + 1}")

            if fitness < best_fitness:
                best_fitness = fitness
                best_decision = decision
                best_truck_number = i + 1
        return [best_decision[i]["path"] for i in range(len(best_decision))], best_truck_number

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
            for gap_index in range(length - 1):
                # gap_index = after what vertex that gap is located
                candidate = dict()
                new_common_path = common_path[:vertex_index] + common_path[vertex_index + 1:]
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

    def run_tabu_search(self):
        best_solution = self._initial_decision
        best_candidate = self._initial_decision


        return best_solution

    def fit(self, x: dict):
        """
        Minimize objective function of VRP
        :param x: dict ("trucks": int, "shops": int, "truck_capacity": int, "data": pd.DataFrame)
        :return: self
        """
        # Set params
        self._set_params(x)
        self.distance_matrix_ = distance.squareform(
            distance.pdist(self.data_.loc[1:, ['X', 'Y']], metric='euclidean'))

        # Create initial decision
        self._create_initial_decision()
        self._save_decision("init.txt", self._initial_decision)

        # Start Tabu Search
        # self.run_tabu_search()

    def transform_decision(self, y):
        pass


def main():
    # Parse data
    parser = DataParser("I1.txt")
    print(parser.get_data_frame())

    # Preprocessing
    data_frame = parser.get_data_frame()
    vertexes = {data_frame["Id"][i]: [data_frame["X"][i], data_frame["Y"][i]] for i in range(len(data_frame["Id"]))}

    # Tabu Search
    my_tabu = TabuVRP()
    my_tabu.fit(parser.get_params())

    # Show initial decision
    init_decision = my_tabu.get_initial_decision()
    tspv.visualize_vrp(vertexes, init_decision, node_size=20, edge_size=0.8, font_size=4, dpi=1000)


if __name__ == '__main__':
    main()
