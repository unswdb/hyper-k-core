//
// Created by _ on 21/7/2023.
//

#include <fstream>
#include <algorithm>
#include <sstream>
#include <queue>
#include "Hypergraph.h"

std::pair<std::size_t, std::size_t> Hypergraph::getEdges(std::size_t node_id) const {
    return {nodes_edge_nvertx_offset[node_id - 1], nodes_edge_nvertx_offset[node_id]};
}

std::size_t &Hypergraph::getNodeCore(std::size_t node_id) {
    return nodes_core[node_id];
}

std::pair<std::size_t, std::size_t> Hypergraph::getNodes(std::size_t edge_id) const {
    return {edge_node_nvertx_offset[edge_id - 1], edge_node_nvertx_offset[edge_id]};
}

std::size_t &Hypergraph::getEdgeCore(std::size_t edge_id) {
    return edges_core[edge_id];
}

//struct Counter {
//    struct value_type {
//        template<typename T>
//        value_type(const T &) {}
//    };
//
//    void push_back(const value_type &) { ++count; }
//
//    size_t count = 0;
//};

std::size_t Hypergraph::getNbrCount(std::size_t node_id) const {
    std::pair<std::size_t, std::size_t> deg_offset = getEdges(node_id);

    if (deg_offset.second - deg_offset.first == 1) {
        std::pair<std::size_t, std::size_t> edge_offset = getNodes(node_edge_simplices[deg_offset.first]);
        return edge_offset.second - edge_offset.first - 1;
    }

    if (deg_offset.second - deg_offset.first == 2) {
        std::pair<std::size_t, std::size_t> edge_offset = getNodes(node_edge_simplices[deg_offset.first]);
        std::pair<std::size_t, std::size_t> edge_offset2 = getNodes(node_edge_simplices[deg_offset.first + 1]);
        Counter c;
        std::set_union(edge_node_simplices.begin() + static_cast<long>(edge_offset.first),
                       edge_node_simplices.begin() + static_cast<long>(edge_offset.second),
                       edge_node_simplices.begin() + static_cast<long>(edge_offset2.first),
                       edge_node_simplices.begin() + static_cast<long>(edge_offset2.second),
                       std::back_inserter(c));
        return c.count - 1;
    }

    std::unordered_set<std::size_t> nbrs(100);
    for (std::size_t i = deg_offset.first; i < deg_offset.second; ++i) {
        std::pair<std::size_t, std::size_t> edge_offset = getNodes(node_edge_simplices[i]);
        for (std::size_t j = edge_offset.first; j < edge_offset.second; ++j) {
            if (edge_node_simplices[j] != node_id) nbrs.insert(edge_node_simplices[j]);
        }
    }
    return nbrs.size();
}

void Hypergraph::loadFromFiles(const std::string &nverts, const std::string &simplices) {

    std::ifstream nvertFile(nverts);
    std::ifstream simplexFile(simplices);

    if (!nvertFile.is_open()) {
        throw std::runtime_error("Error to open file " + nverts);
    }
    if (!simplexFile.is_open()) {
        throw std::runtime_error("Error to open file " + simplices);
    }
    std::size_t numNodes = 0;

    std::size_t prev = 0;
    edge_node_nvertx_offset.push_back(prev);
    while (!nvertFile.eof()) {
        std::size_t temp;
        nvertFile >> temp;
        prev += temp;
        edge_node_nvertx_offset.push_back(prev);
    }

    while (!simplexFile.eof()) {
        std::size_t temp;
        simplexFile >> temp;
        edge_node_simplices.push_back(temp);
        numNodes = std::max(numNodes, temp);
    }

    loadFromFiles_helper(numNodes);

    std::cout << "loaded" << std::endl;
}

void Hypergraph::loadFromFiles(const std::string &hyp) {
    std::ifstream hypFile(hyp);
    if (!hypFile.is_open()) {
        throw std::runtime_error("Error to open file " + hyp);
    }
    std::size_t prev = 0;
    std::size_t numNodes = 0;
    edge_node_nvertx_offset.push_back(prev);
    while (!hypFile.eof()) {
        std::string line;
        std::getline(hypFile, line);
        std::replace(line.begin(), line.end(), ',', ' ');
        std::stringstream ss(line);
        std::size_t temp;
        std::vector<std::size_t> edge;
        while (ss >> temp) {
            edge.push_back(temp);
            prev++;
            numNodes = std::max(numNodes, temp);
        }
        std::sort(edge.begin(), edge.end());
        for (std::size_t i: edge) {
            edge_node_simplices.push_back(i);
        }
        edge_node_nvertx_offset.push_back(prev);
    }

    loadFromFiles_helper(numNodes);

    std::cout << "loaded" << std::endl;

}

void Hypergraph::loadFromFiles_helper(std::size_t numNodes) {

    std::vector<std::size_t> node_edge_size(numNodes + 1);
    for (std::size_t i = 1; i < edge_node_nvertx_offset.size(); ++i) {
        for (std::size_t j = edge_node_nvertx_offset[i - 1]; j < edge_node_nvertx_offset[i]; ++j) {
            node_edge_size[edge_node_simplices[j]]++;
        }
    }

    std::size_t node_prev = 0;
//    init nodes_edge_nvertx_offset
    for (auto &node_size: node_edge_size) {
        auto old_node_prev = node_prev;
        node_prev += node_size;
        nodes_edge_nvertx_offset.push_back(node_prev);
        node_size = old_node_prev;
    }

    node_edge_simplices.resize(node_prev);
//    fill in node_edge_simplices
    for (std::size_t i = 1; i < edge_node_nvertx_offset.size(); ++i) {
        for (std::size_t j = edge_node_nvertx_offset[i - 1]; j < edge_node_nvertx_offset[i]; ++j) {
            node_edge_simplices[node_edge_size[edge_node_simplices[j]]++] = i;
        }
    }

    nodes_core.resize(nodes_edge_nvertx_offset.size());
    edges_core.resize(edge_node_nvertx_offset.size());

    for (auto &edgeCore: edges_core) {
        edgeCore = std::numeric_limits<std::size_t>::max();
    }

    for (auto &nodeCore: nodes_core) {
        nodeCore = std::numeric_limits<std::size_t>::max();
    }

}

std::size_t Hypergraph::getGraphEdgeSize() const {
    return edges_core.size();
}

std::size_t Hypergraph::getGraphNodeSize() const {
    return nodes_core.size();
}

std::vector<std::size_t>
Hypergraph::findIntersectionEdge(std::size_t nbr_index, std::size_t node_a, std::size_t node_b) {
    if (nbr_intersection[nbr_index].empty()) {
        std::pair<std::size_t, std::size_t> edge_a = getEdges(node_a);
        std::pair<std::size_t, std::size_t> edge_b = getEdges(node_b);
        std::set_intersection(node_edge_simplices.begin() + static_cast<long>(edge_a.first),
                              node_edge_simplices.begin() +
                              static_cast<long>(edge_a.second),
                              node_edge_simplices.begin() + static_cast<long>(edge_b.first),
                              node_edge_simplices.begin() +
                              static_cast<long>(edge_b.second),
                              std::back_inserter(nbr_intersection[nbr_index]));
    }

    return nbr_intersection[nbr_index];

}

std::vector<std::size_t> Hypergraph::findIntersectionEdge(std::size_t node_a, std::size_t node_b) {
    auto begin = nbr_simplices.begin() + static_cast<long>(nbr_nvertx_offset[node_a - 1]);
    auto end = nbr_simplices.begin() + static_cast<long>(nbr_nvertx_offset[node_a]);
    auto it = std::lower_bound(begin, end, node_b);
    auto nbr_index = static_cast<std::size_t>(it - nbr_simplices.begin());
    return findIntersectionEdge(nbr_index, node_a, node_b);
}


std::vector<std::size_t> Hypergraph::computeIntersectionEdge(std::size_t node_a, std::size_t node_b) {
    std::vector<std::size_t> result;
    std::pair<std::size_t, std::size_t> edge_a = getEdges(node_a);
    std::pair<std::size_t, std::size_t> edge_b = getEdges(node_b);
    std::set_intersection(node_edge_simplices.begin() + static_cast<long>(edge_a.first),
                          node_edge_simplices.begin() +
                          static_cast<long>(edge_a.second),
                          node_edge_simplices.begin() + static_cast<long>(edge_b.first),
                          node_edge_simplices.begin() +
                          static_cast<long>(edge_b.second),
                          std::back_inserter(result));
    return result;
}

void Hypergraph::computeNbrs() {
    std::vector<std::unordered_set<std::size_t>> node_nbr(getGraphNodeSize() + 1);
    for (std::size_t i = 1; i < edge_node_nvertx_offset.size(); ++i) {
        for (std::size_t j = edge_node_nvertx_offset[i - 1]; j < edge_node_nvertx_offset[i]; ++j) {
            for (std::size_t k = edge_node_nvertx_offset[i - 1]; k < edge_node_nvertx_offset[i]; ++k) {
                if (edge_node_simplices[k] != edge_node_simplices[j]) {
                    node_nbr[edge_node_simplices[j]].insert(edge_node_simplices[k]);
                }
            }
        }
    }

    std::size_t nbr_prev = 0;
    for (const auto &nbr: node_nbr) {
        nbr_prev += nbr.size();
        nbr_nvertx_offset.push_back(nbr_prev);
        std::vector<std::size_t> nbr_vec(nbr.begin(), nbr.end());
        std::sort(nbr_vec.begin(), nbr_vec.end());
        for (std::size_t i: nbr_vec) {
            nbr_simplices.push_back(i);
        }
    }

}

std::pair<std::size_t, std::size_t> Hypergraph::getNbr(std::size_t node_id) const {
    return {nbr_nvertx_offset[node_id - 1], nbr_nvertx_offset[node_id]};
}

void Hypergraph::printNodesCore() const {
    std::cout << "node ";
    for (std::size_t i = 1; i < nodes_core.size(); ++i) {
        std::cout << i << " ";
    }
    std::cout << std::endl;
    std::cout << "core ";
    for (std::size_t i = 1; i < nodes_core.size(); ++i) {
        std::cout << nodes_core[i] << " & ";
//        if (i >= 10) std::cout << " ";
    }
    std::cout << std::endl;
}

void Hypergraph::printEdgesCore() const {
    std::cout << "edge ";
    for (std::size_t i = 1; i < edges_core.size(); ++i) {
        std::cout << i << " ";
    }
    std::cout << std::endl;
    std::cout << "core ";
    for (std::size_t i = 1; i < edges_core.size(); ++i) {
        std::cout << edges_core[i] << " & ";
    }
    std::cout << std::endl;
}

std::size_t Hypergraph::getAvgDegree() const {
    std::size_t sum = 0;
    for (std::size_t node_id = 1; node_id < getGraphNodeSize(); ++node_id) {
        auto edge_offset = getEdges(node_id);
        sum += edge_offset.second - edge_offset.first;
    }
    return sum / getGraphNodeSize();
}

std::size_t Hypergraph::getAvgEdgeSize() const {
    std::size_t sum = 0;
    for (std::size_t edge_id = 1; edge_id < getGraphEdgeSize(); ++edge_id) {
        auto node_offset = getNodes(edge_id);
        sum += node_offset.second - node_offset.first;
    }
    return sum / getGraphEdgeSize();
}

void Hypergraph::resetCore() {
    for (auto &nodeCore: nodes_core) {
        nodeCore = std::numeric_limits<std::size_t>::max();
    }

    for (auto &edgeCore: edges_core) {
        edgeCore = std::numeric_limits<std::size_t>::max();
    }
}

std::size_t Hypergraph::getEdgeSize(std::size_t edge_id) const {
    auto node_offset = getNodes(edge_id);
    return node_offset.second - node_offset.first;
}
