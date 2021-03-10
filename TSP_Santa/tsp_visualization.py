import csv
import networkx as nx
import matplotlib.pyplot as plt


def read_tsp(filename: str, dimensions=2, delimiter=' ',
             top_banner=0, return_top_info=False):
    """
    Read tsp dataset
    :param filename: name of file with dataset
    :param dimensions: how many dimensions a vertex has
    :param delimiter: delimiter in rows
    :param top_banner: number of strings which do not represent vertex positions
    :param return_top_info: should the function return info from the top of file
    :return: return_top_info == False: vertexes {id: (pos_1, ... pos_n)}
             return_top_info == True: top_info (list of lines), vertexes {id: (pos_1, ... pos_n)}
    """
    with open(filename, 'r') as f:
        # read tsp info
        top_info = list()
        for i in range(top_banner):
            top_info.append(f.readline())

        # read vertexes
        vertexes = dict()
        for line in f:
            line = line.split(delimiter)[:dimensions+1]
            vertexes[int(line[0])] = tuple([float(line[i+1]) for i in range(dimensions)])

    if return_top_info:
        return top_info, vertexes
    else:
        return vertexes


def save_answer_csv(path: list, filename='tsp_answer_path.csv', headers=tuple()):
    """
    Create (if needed) the csv file with tsp answer
    :param path: Hamiltonian way in list
    :param filename: name of csv file to write the answer
    :param headers: headers of csv file
    :return: None
    """
    if headers is None:
        headers = []
    with open(filename, 'w') as f:
        writer = csv.writer(f)
        if len(headers):
            writer.writerow(headers)
        for vertex in path:
            writer.writerow([vertex])


def create_edges_from_path(path: list):
    edges = list()
    for i in range(len(path)):
        # iter += 1
        if i == len(path) - 1:
            edges.append((path[0], path[i]))
        else:
            if path[i] == path[i + 1]:
                print("ERROR <DUBLICATED>: ", (path[i], path[i+1]))
            edges.append((path[i], path[i+1]))

    # errors check
    for edge1 in edges:
        for edge2 in edges:
            if edge1[0] == edge2[1] and edge1[1] == edge2[0]:
                print("ERROR <DUBLICATED>: ", edge1, edge2)

    return edges


def visualize_tsp(vertexes: dict, path: list, filename='tsp_answer_path.jpg',
                  dpi=500, node_size=1.0, font_size=1.0, with_labels=False):
    """
    Draw graph of tsp answer and save it as JPG
    :param vertexes: vertexes dict (get it from read_tsp() function)
    :param path: Hamiltonian way in list
    :param filename: name of picture
    :param dpi: picture resolution
    :param node_size: size of vertex in picture
    :param font_size: size of font in picture
    :param with_labels: are there id of every vertex in picture
    :return: None
    """
    edges = create_edges_from_path(path)
    graph = nx.Graph()
    # add vertexes
    for v_key in vertexes.keys():
        graph.add_node(v_key, pos=(vertexes[v_key][0], vertexes[v_key][1]))
    # add edges
    for edge in edges:
        graph.add_edge(edge[0], edge[1])
    # draw in file
    nx.draw(graph,
            nx.get_node_attributes(graph, 'pos'),
            with_labels=with_labels, node_size=node_size, font_size=font_size,
            font_color='r', linewidths=0.0, node_color='g')
    plt.savefig(filename, dpi=dpi)

# v = read_tsp('/Users/sanduser/Downloads/traveling-santa-2018-prime-paths/cities.csv',
#              top_banner=1, delimiter=',')

# save_answer_csv([1, 2, 3, 4, 5], headers=['Path'])

# visualize_tsp(v, [i for i in range(len(v.keys()))], node_size=0.1)
