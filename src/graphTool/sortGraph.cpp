//
// Created by _ on 3/4/2024.
//
#include "../Hypergraph.h"
#include "graphTool.h"
#include <iostream>
#include <vector>
#include <fstream>
#include <algorithm>
#include <sstream>

std::vector<std::size_t> sortNodeByDegreeMap(Hypergraph &graph, orderType order) {
    graph.edge_node_nvertx_offset.clear();
    graph.edge_node_simplices.clear();
    std::vector<std::pair<std::size_t, std::size_t>> degree(graph.getGraphNodeSize());
    for (std::size_t node = 1; node < graph.getGraphNodeSize(); ++node) {
        auto degree_pair = graph.getEdges(node);
        degree[node] = std::make_pair(degree_pair.second - degree_pair.first, node);
    }
    if (order == ASC) {
        degree[0] = std::make_pair(0, 0);
    } else {
        degree[0] = std::make_pair(graph.getGraphNodeSize(), 0);
    }
    std::sort(degree.begin(), degree.end(), [&order](const std::pair<std::size_t, std::size_t> &a,
                                                     const std::pair<std::size_t, std::size_t> &b) {
        if (order == ASC) {
            return a.first < b.first;
        } else {
            return a.first > b.first;
        }
    });
    std::vector<std::size_t> index_map(graph.getGraphNodeSize());
    for (std::size_t i = 0; i < graph.getGraphNodeSize(); ++i) {
        index_map[degree[i].second] = i;
    }

    return index_map;
}

std::vector<std::size_t> sortNodeByNbrMap(Hypergraph &graph, orderType order) {
    std::vector<std::pair<std::size_t, std::size_t>> nbrCount(graph.getGraphNodeSize());
    for (std::size_t node = 1; node < graph.getGraphNodeSize(); ++node) {
        nbrCount[node] = std::make_pair(graph.getNbrCount(node), node);
    }
    if (order == ASC) {
        nbrCount[0] = std::make_pair(0, 0);
    } else {
        nbrCount[0] = std::make_pair(graph.getGraphNodeSize(), 0);
    }
    std::sort(nbrCount.begin(), nbrCount.end(), [&order](const std::pair<std::size_t, std::size_t> &a,
                                                         const std::pair<std::size_t, std::size_t> &b) {
        if (order == ASC) {
            return a.first < b.first;
        } else {
            return a.first > b.first;
        }

    });
    std::vector<std::size_t> index_map(graph.getGraphNodeSize());
    for (std::size_t i = 1; i < graph.getGraphNodeSize(); ++i) {
        index_map[nbrCount[i].second] = i;
    }

    return index_map;
}

void outputGraph(const std::string &graph_path, const std::vector<std::size_t> &index_map,
                 const std::string &outputPath) {
    std::ifstream original(graph_path);
    std::ofstream output(outputPath);
    std::cout << "Graph generation completed. writing to " << outputPath << std::endl;
    while (!original.eof()) {
        std::string line;
        std::getline(original, line);
        std::replace(line.begin(), line.end(), ',', ' ');
        std::stringstream ss(line);
        std::size_t temp;
        std::vector<std::size_t> temp_vec;
        while (ss >> temp) {
            temp_vec.push_back(index_map[temp] + 1);
        }

        std::sort(temp_vec.begin(), temp_vec.end());
        for (std::size_t i = 0; i < temp_vec.size(); ++i) {
            output << temp_vec[i];
            if (i != temp_vec.size() - 1) {
                output << ",";
            }
        }
        output << "\n";
    }
    output.close();
}

void sortNodeByDegree(Hypergraph &graph, const std::string &graph_path, orderType order) {
    std::cout << "sorting by degree, order: " << (order == ASC ? "asc" : "desc") << std::endl;
    std::vector<std::size_t> index_map = sortNodeByDegreeMap(graph, order);

    std::string outputPath = graph_path + "_sorted_by_degree_" + (order == ASC ? "asc" : "desc") + ".txt";
    outputGraph(graph_path, index_map, outputPath);
    std::cout << "Graph written to " << graph_path << "_sorted_by_degree.txt" << std::endl;
}

void sortNodeByNbr(Hypergraph &graph, const std::string &graph_path, orderType order) {
    std::cout << "sorting by nbr, order: " << (order == ASC ? "asc" : "desc") << std::endl;
    std::vector<std::size_t> index_map = sortNodeByNbrMap(graph, order);
    std::string outputPath = graph_path + "_sorted_by_nbr_" + (order == ASC ? "asc" : "desc") + ".txt";
    outputGraph(graph_path, index_map, outputPath);
    std::cout << "Graph written to " << graph_path << "_sorted_by_nbr.txt" << std::endl;
}

std::vector<std::size_t> edgeOrdered(Hypergraph &graph, orderType order) {
    std::vector<std::pair<std::size_t, std::size_t>> edgeCount(graph.getGraphEdgeSize());
    for (std::size_t edge = 1; edge < graph.getGraphEdgeSize(); ++edge) {
        edgeCount[edge] = std::make_pair(graph.getEdgeSize(edge), edge);
    }

    if (order == ASC) {
        edgeCount[0] = std::make_pair(0, 0);
    } else {
        edgeCount[0] = std::make_pair(std::numeric_limits<std::size_t>::max(), 0);
    }

    std::sort(edgeCount.begin(), edgeCount.end(), [&order](const std::pair<std::size_t, std::size_t> &a,
                                                           const std::pair<std::size_t, std::size_t> &b) {
        if (order == ASC) {
            return a.first < b.first;
        } else {
            return a.first > b.first;
        }
    });

    std::vector<std::size_t> index;
    for (auto sortedEdge: edgeCount) {
        if (sortedEdge.second == 0) {
            continue;
        }
        index.push_back(sortedEdge.second);
    }

    return index;
}

void sortEdge(Hypergraph &graph, const std::string &graph_path, orderType order) {
    std::cout << "sort by edge size, order: " << (order == ASC ? "asc" : "desc") << std::endl;

    std::vector<std::size_t> index = edgeOrdered(graph, order);

    std::ifstream original(graph_path);
    std::string outputPath = graph_path + "_sorted_by_edge_" + (order == ASC ? "asc" : "desc") + ".txt";
    std::ofstream output(outputPath);
    std::cout << "Graph generation completed. writing to " << outputPath << std::endl;
    for (auto edge_id: index) {
        std::pair<std::size_t, std::size_t> edge_nodes_offset = graph.getNodes(edge_id);
        for (std::size_t i = edge_nodes_offset.first; i < edge_nodes_offset.second; ++i) {
            output << graph.edge_node_simplices[i];
            if (i != edge_nodes_offset.second - 1) {
                output << ",";
            }
        }
        output << "\n";
    }

    output.close();
}

orderType cover_to_orderType(const std::string &order) {
    if (order == "asc") {
        return ASC;
    } else if (order == "desc") {
        return DESC;
    } else {
        return ASC;
    }
}

std::string orderType_to_string(orderType order) {
    if (order == ASC) {
        return "asc";
    } else if (order == DESC) {
        return "desc";
    } else {
        return "asc";
    }
}

void sortEdgeNodes(Hypergraph &graph, const std::string &graph_path, orderType node_order, orderType edge_order) {
    std::vector<std::size_t> edge_index = edgeOrdered(graph, edge_order);
    std::vector<std::size_t> node_index = sortNodeByNbrMap(graph, node_order);

    std::string outputPath = graph_path + "_sorted_by_edge_" + orderType_to_string(edge_order) + "_node_" +
                             orderType_to_string(node_order) + ".txt";
    std::ofstream output(outputPath);
    std::cout << "Graph generation completed. writing to " << outputPath << std::endl;
    for (auto edge_id: edge_index) {
        std::pair<std::size_t, std::size_t> edge_nodes_offset = graph.getNodes(edge_id);
        std::vector<std::size_t> temp_edge;
        for (std::size_t i = edge_nodes_offset.first; i < edge_nodes_offset.second; ++i) {
            temp_edge.push_back(node_index[graph.edge_node_simplices[i]]);
        }
        if (temp_edge.empty()) {
            continue;
        }
        std::sort(temp_edge.begin(), temp_edge.end());
        for (std::size_t i = 0; i < temp_edge.size(); ++i) {
            output << temp_edge[i];
            if (i != temp_edge.size() - 1) {
                output << ",";
            }
        }
        output << "\n";
    }

    output.close();
}

void sortNodes(const std::string &graph_path, sortType type, orderType order) {
    Hypergraph hg;
    hg.loadFromFiles(graph_path);
    switch (type) {
        case DEGREE:
            sortNodeByDegree(hg, graph_path, order);
            break;
        case NBR:
            sortNodeByNbr(hg, graph_path, order);
            break;
    }
}

void sortEdges(const std::string &graph_path, orderType order) {
    Hypergraph hg;
    hg.loadFromFiles(graph_path);
    sortEdge(hg, graph_path, order);
}

void sortEdgeNodes(const std::string &graph_path, orderType node_order, orderType edge_order) {
    Hypergraph hg;
    hg.loadFromFiles(graph_path);
    sortEdgeNodes(hg, graph_path, node_order, edge_order);
}