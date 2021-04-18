import sys
from tabulate import tabulate as tb
from scipy.spatial import distance
import functools
import random
from tsp_visualization import visualize_vrp
from data_parser import DataParser


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

    class TabuCandidate:
        def __init__(self, y, added_edges: set, deleted_edges: set):
            self.y = y
            self.added_edges = added_edges
            self.deleted_edges = deleted_edges

    def __init__(self, tabu_lifetime=10, max_iter=10000,
                 lateness_penalty_coefficient=1,
                 capacity_overload_penalty_coefficient=1):
        """
        Initiate VRP Tabu Search with hyperparameters
        :param tabu_lifetime:
        :param max_iter:
        """
        # hyperparameters
        self._tabu_lifetime = tabu_lifetime
        self._max_iter = max_iter
        self._lateness_penalty_coefficient = lateness_penalty_coefficient
        self._capacity_overload_penalty_coefficient = capacity_overload_penalty_coefficient

        # changed via fit
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

        # initial decision
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

    def _save_decision(self, y: list, file_name: str, extension='.txt') -> None:
        with open(file_name+extension, 'w', encoding='utf-8') as file:
            for route in y:
                file.write(", ".join([str(el) for el in route]) + '\n')

        transformed_solution = self.transform_decision(y)
        with open(file_name+'_transformed'+extension, 'w', encoding='utf-8') as file:
            for route in transformed_solution:
                line = " ".join([" ".join([str(el) for el in vertex_info]) for vertex_info in route])
                file.write(line + '\n')

    @staticmethod
    def _load_decision(file_name: str) -> list:
        """
        Load decision from file in a format of list of routes
        :param file_name: source file path
        :return: list of routes
        """
        y = list()
        with open(file_name, 'r', encoding='utf-8') as file:
            for line in file:
                y.append(tuple(map(int, line.split(', '))))
        return y

    """
    Greedy initial decision
    """

    def _create_initial_decision(self, mode="greedy"):
        if mode == "greedy":
            self._initial_decision, self._init_truck_number = self._init_greedy()
            # print(self.initial_decision, self.init_truck_number)

    def _find_next_node(self, customer, time, capacity, path_memory) -> tuple:
        distances = self.distance_matrix_[customer]
        min_dist = sys.float_info.max
        min_customer = -1

        for next_customer, dist in enumerate(distances):
            if not path_memory[next_customer]:
                time_open = self.data_["TimeOpen"][next_customer]
                if time + dist < time_open:
                    wait_dist = time_open - time
                else:
                    wait_dist = dist
                if wait_dist < min_dist \
                    and time + self.distance_matrix_[0][customer] + wait_dist < self.data_["TimeClose"][0] \
                    and time + wait_dist < self.data_["TimeClose"][next_customer] \
                        and capacity > self.data_["Demand"][next_customer]:
                    min_dist = wait_dist
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
        decisions = list()

        # Create decisions for all number of trucks
        for truck_num in range(1, self.trucks_ + 1):
            invalid_count = 0
            add_invalid = False
            print(f"Greedy with truck number = {truck_num}")
            decision = [{"path": [0], "cost": 0, "capacity": self.truck_capacity_, "invalid_count": 0}
                        for i in range(truck_num)]
            visited_customers = [False for i in range(self.shops_ + 1)]
            visited_counter = self.shops_ + 1
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
                decision[i]["cost"] += self.distance_matrix_[0][decision[i]["path"][-1]]
                decision[i]["path"].append(0)
            decisions.append(decision)

        # Find the best decision
        return self._find_best_greedy_decision(decisions)

    @staticmethod
    def _find_best_greedy_decision(decisions: list):
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

    """
    Estimators and objective function
    """

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
                opening_time, closing_time = self.data_.iloc[node2][['TimeOpen', 'TimeClose']]
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
            kwargs['exponent'] = self._invalid_counter
            stimulating_penalty = stimulating_func(*args, **kwargs)
        # return float(stimulating_penalty)
        # ANOTHER VARIANT OF PENALTY
        # return float(decision_penalty + stimulating_penalty)
        return float(decision_penalty)

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
        vehicle_valid = False if len(y) > self.trucks_ else True
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

    """
    Neighborhoods
    """

    @classmethod
    def move_customer(cls, y):
        """
        Generates neighborhood of current decision by moving 1 vertex in a path
        :param y: current decision
        :return: list of TabuCandidate
        """
        neighborhood = []
        decisions_hash_map = dict()
        # concatenate all routes in to one path deleting double zero
        common_path = list(functools.reduce(lambda x1, x2: x1[:-1] + x2, y))
        decisions_hash_map[tuple(common_path)] = True
        length = len(common_path)
        for vertex_index in range(length):
            moved_value = common_path[vertex_index]
            # we do not move the '0' vertex
            if moved_value == 0:
                continue
            for gap_index in range(length - 1):
                # gap_index = after what vertex the gap is located
                new_common_path = common_path[:vertex_index] + common_path[vertex_index + 1:]
                new_common_path.insert(gap_index if gap_index >= vertex_index else gap_index + 1, moved_value)
                new_common_path = tuple(new_common_path)
                # check repeated decisions
                decision_exists = decisions_hash_map.get(new_common_path, False)
                if decision_exists:
                    continue
                else:
                    decisions_hash_map[new_common_path] = True
                deleted_edges = list()
                deleted_edges.append((common_path[vertex_index - 1], common_path[vertex_index]))
                deleted_edges.append((common_path[vertex_index], common_path[vertex_index + 1]))
                deleted_edges.append((common_path[gap_index], common_path[gap_index + 1]))
                added_edges = list()
                added_edges.append((new_common_path[vertex_index - 1], new_common_path[vertex_index]))
                added_edges.append((new_common_path[gap_index - 1], new_common_path[gap_index]))
                added_edges.append((new_common_path[gap_index], new_common_path[gap_index + 1]))
                # divide common_path into routes
                zero_indexes = [i for i, value in enumerate(new_common_path) if value == 0]
                decision = [new_common_path[start: end + 1] for start, end in zip(zero_indexes, zero_indexes[1:])]
                # candidate
                candidate = cls.TabuCandidate(decision, set(deleted_edges), set(added_edges))
                neighborhood.append(candidate)

        return neighborhood

    """
    General Tabu Search
    """

    def _stop_condition(self, iteration: int) -> bool:
        """
        Indicates if the algorithm should be stopped
        :param iteration: current iteration of Tabu search
        :return: True if should be stopped else False (bool)
        """
        return iteration > self._max_iter

    def _check_tabu_list(self, tabu_values: set) -> bool:
        """
        Check value existence in tabu list
        :param tabu_values: (set) value to check
        :return: (bool) True if exists, else False
        """
        if len(self._tabu_list) < 1:
            return False

        tabu_map = [sum([tabu.compare(tabu_value) for tabu_value in tabu_values]) > 1
                    for tabu in self._tabu_list]
        return functools.reduce(lambda x1, x2: x1 or x2, tabu_map)

    @staticmethod
    def _random_sample_neighborhood(neighborhood, target_size):
        return random.choices(neighborhood, k=target_size)

    def _run_tabu_search(self, neighborhood_func, limit_neighborhood=-1):
        """
        Run general Tabu Search algorithm
        :param neighborhood_func: function to create neighborhood variety
        :return: None
        """
        iteration = 0

        best_solution = self._initial_decision
        _, penalties, _ = self._check_valid_decision(best_solution)
        best_solution_fitness = self._fitness(best_solution) + penalties
        best_candidate = best_solution
        while not self._stop_condition(iteration):
            iteration += 1
            print(f'Iteration {iteration}')
            neighborhood = neighborhood_func(best_candidate)
            if limit_neighborhood > 0:
                neighborhood = self._random_sample_neighborhood(neighborhood, limit_neighborhood)

            if len(neighborhood) < 1:
                print(f"* LOG *: neighborhood for '{best_candidate}' is empty")
                return best_solution

            best_candidate = neighborhood[0].y
            is_valid_best_candidate, penalties, _ = self._check_valid_decision(best_candidate)
            best_candidate_fitness = self._fitness(best_candidate) + penalties
            best_candidate_tabu_values = set()

            candidate_index = 0
            for candidate in neighborhood[1:]:
                candidate_index += 1
                print(f"\rcurrent candidate {candidate_index}             ", end='')
                edges_moved = candidate.added_edges.union(candidate.deleted_edges)
                is_valid, penalties, _ = self._check_valid_decision(candidate.y)
                cur_fitness = self._fitness(candidate.y) + penalties

                if not self._check_tabu_list(edges_moved) and cur_fitness < best_candidate_fitness:
                    best_candidate = candidate.y
                    best_candidate_fitness = cur_fitness
                    best_candidate_tabu_values = edges_moved
                    is_valid_best_candidate = is_valid

            if not is_valid_best_candidate:
                self._invalid_counter += 1
            else:
                self._invalid_counter = 0

            if best_candidate_fitness < best_solution_fitness and is_valid_best_candidate:
                best_solution = best_candidate
                best_solution_fitness = best_candidate_fitness
                self._save_decision(best_solution, "best_solution")

            to_delete = list()
            # decrement lifetime
            for tabu in self._tabu_list:
                lifetime = tabu.dec_lifetime()
                if lifetime <= 0:
                    to_delete.append(tabu)
            # delete dead
            for tabu in to_delete:
                self._tabu_list.remove(tabu)
            # add new
            for tabu_value in best_candidate_tabu_values:
                self._tabu_list.append(self.TabuContainer(tabu_value, self._tabu_lifetime))

            """
            Logging
            """
            def log_iteration():
                header = ('Iteration', 'Objective', 'Is valid', 'Penalties')
                raw_fitness = self._fitness(best_candidate)
                values = [(iteration,
                          best_candidate_fitness,
                          is_valid_best_candidate,
                          best_candidate_fitness - raw_fitness)]
                print('\n')
                print(tb(values, headers=header, tablefmt="github"))
                # print('--- Tabu List ---')
                # tabu_values = [(tabu.x, tabu.counter,) for tabu in self._tabu_list]
                # print(tb(tabu_values, headers=('value', 'lifetime'), tablefmt="github"))
                # print('\n')

            log_iteration()

        return best_solution

    def fit(self, x: dict, load_initial=False, initial_file_name='init.txt'):
        """
        Minimize objective function of VRP
        :param x: dict ("trucks": int, "shops": int, "truck_capacity": int, "data": pd.DataFrame)
        :param load_initial: should we load initial decision from saved file, True if we should
        :param initial_file_name: file name with initial decision
        :return: self
        """
        # Set params
        self._set_params(x)
        self.distance_matrix_ = distance.squareform(
            distance.pdist(self.data_[['X', 'Y']], metric='euclidean'))

        # Create initial decision
        if load_initial:
            self._initial_decision = self._load_decision(initial_file_name)
        else:
            self._create_initial_decision()
            self._save_decision(self._initial_decision, "init")

        # Start Tabu Search
        best_solution = self._run_tabu_search(TabuVRP.move_customer, 300)
        print(" * END * : best solution is")
        print(best_solution)

    def transform_decision(self, y):
        """
        Transform decision into 'Bychkov Ilya' form
        :param y: list of routes
        :return: path matrix with delivery time to each customer
        """
        path_matrix = list()
        for route in y:
            customer_time_delivery = [(0, 0.0)]
            route_time = 0.0
            for vertex1, vertex2 in zip(route, route[1:]):
                route_time += self.distance_matrix_[vertex1][vertex2] + self.data_['Unloading'][vertex2]
                id_ = self.data_['Id'][vertex2]
                customer_time_delivery.append((id_, route_time))
            path_matrix.append(customer_time_delivery)
        return path_matrix


def main():
    # Parse data
    parser = DataParser("I1.txt")
    print(parser.get_data_frame())

    # Tabu Search
    my_tabu = TabuVRP()
    my_tabu.fit(parser.get_params(), load_initial=True)

    # Show initial decision
    # data_frame = parser.get_data_frame()
    # vertexes = {data_frame["Id"][i]: [data_frame["X"][i], data_frame["Y"][i]] for i in range(len(data_frame["Id"]))}
    # init_decision = my_tabu.get_initial_decision()
    # visualize_vrp(vertexes, init_decision, node_size=20, edge_size=0.8, font_size=4, dpi=1000)


if __name__ == '__main__':
    main()
