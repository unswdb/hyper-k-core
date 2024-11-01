//
// Created by _ on 21/7/2023.
//

#ifndef HYPERGRAPHKCORE_HYPERGRAPH_H
#define HYPERGRAPHKCORE_HYPERGRAPH_H

#include <unordered_map>
#include <unordered_set>
#include <vector>
#include <iostream>
#include <limits>
#include <algorithm>
#include <set>
#include <chrono>
#include <cmath>

struct Counter {
    struct value_type {
        template<typename T>
        value_type(const T &) {}
    };

    void push_back(const value_type &) { ++count; }

    size_t count = 0;
};


class Hypergraph {
public:

    Hypergraph() = default;

    Hypergraph(const Hypergraph &hg) = default;

    Hypergraph(Hypergraph &&hg) = default;

    Hypergraph &operator=(const Hypergraph &hg) = default;

    Hypergraph &operator=(Hypergraph &&hg) = default;

    ~Hypergraph() = default;

    [[nodiscard]] std::pair<std::size_t, std::size_t> getNbr(std::size_t node_id) const;

    [[nodiscard]] std::pair<std::size_t, std::size_t> getEdges(std::size_t node_id) const;

    [[nodiscard]] std::size_t &getNodeCore(std::size_t node_id);

    [[nodiscard]] std::pair<std::size_t, std::size_t> getNodes(std::size_t edge_id) const;

    [[nodiscard]] std::size_t getNbrCount(std::size_t node_id) const;

    [[nodiscard]] std::size_t &getEdgeCore(std::size_t edge_id);

    [[nodiscard]] std::size_t getGraphEdgeSize() const;

    [[nodiscard]] std::size_t getGraphNodeSize() const;

    [[nodiscard]] std::size_t getEdgeSize(std::size_t edge_id) const;

    [[nodiscard]] std::size_t getAvgDegree() const;

    [[nodiscard]] std::size_t getAvgEdgeSize() const;

    std::vector<std::size_t> findIntersectionEdge(std::size_t nbr_index, std::size_t node_a, std::size_t node_b);

    std::vector<std::size_t> findIntersectionEdge(std::size_t node_a, std::size_t node_b);

    std::vector<std::size_t> computeIntersectionEdge(std::size_t node_a, std::size_t node_b);

    void loadFromFiles(const std::string &nverts, const std::string &simplices);

    void loadFromFiles(const std::string &hyp);

    void computeNbrs();

    void printNodesCore() const;

    void printEdgesCore() const;

    static void bin_sort_hyperedge(Hypergraph &graph, const std::size_t node_id) {
        std::pair<std::size_t, std::size_t> edge_offset = graph.getEdges(node_id);
        std::size_t node_core = graph.getNodeCore(node_id);
        std::vector<size_t> bin_count(node_core + 1, 0);
        std::vector<size_t> bin(edge_offset.second - edge_offset.first, 0);
        for (std::size_t edge_index = edge_offset.first; edge_index < edge_offset.second; ++edge_index) {
            std::size_t edge_core = graph.getEdgeCore(graph.node_edge_simplices[edge_index]);
            bin_count[std::min(node_core, edge_core)]++;
        }
        for (std::size_t i = node_core - 1; i > 0; --i) {
            bin_count[i] += bin_count[i + 1];
        }

        for (std::size_t edge_index = edge_offset.first; edge_index < edge_offset.second; ++edge_index) {
            std::size_t edge_core = graph.getEdgeCore(graph.node_edge_simplices[edge_index]);
            bin[--bin_count[std::min(node_core, edge_core)]] = graph.node_edge_simplices[edge_index];
        }
        std::copy(bin.begin(), bin.end(), graph.node_edge_simplices.begin() + static_cast<long>(edge_offset.first));
    }

//    Alg 6 with implement optimization, using built-in sort
    static std::size_t CoreEvaluationFinal(Hypergraph &graph, const std::size_t node_id) {
        std::unordered_set<std::size_t> core_weight_set(graph.getNodeCore(node_id) + 1);
        std::pair<std::size_t, std::size_t> edge_offset = graph.getEdges(node_id);

////        You can add condition here to choose which sorting method to use to achieve better performance
//        std::size_t node_core = graph.getNodeCore(node_id);
//        auto edge_size = static_cast<double>(edge_offset.second -
//                                               edge_offset.first);
//        if ((static_cast<double>(node_core) + edge_size) * 10 < log2(edge_size) * edge_size) {
//            bin_sort_hyperedge(graph, node_id);
//        } else {
//            std::sort(graph.node_edge_simplices.begin() + static_cast<long>(edge_offset.first),
//                      graph.node_edge_simplices.begin() + static_cast<long>(edge_offset.second),
//                      [&graph](std::size_t a, std::size_t b) {
//                          return graph.edges_core[a] > graph.edges_core[b];
//                      });
//        }

        std::sort(graph.node_edge_simplices.begin() + static_cast<long>(edge_offset.first),
                  graph.node_edge_simplices.begin() + static_cast<long>(edge_offset.second),
                  [&graph](std::size_t a, std::size_t b) {
                      return graph.edges_core[a] > graph.edges_core[b];
                  });


        if (edge_offset.second - edge_offset.first == 0) {
            return 0;
        }

        if (edge_offset.second - edge_offset.first == 1) {
            std::size_t edge_id = graph.node_edge_simplices[edge_offset.first];
            return std::min(graph.getEdgeCore(edge_id), graph.getEdgeSize(edge_id) - 1);
        }

        if (edge_offset.second - edge_offset.first >= 2) {
            std::size_t first_edge_id = graph.node_edge_simplices[edge_offset.first];
            std::size_t second_edge_id = graph.node_edge_simplices[edge_offset.first + 1];
            std::size_t first_edge_core = graph.getEdgeCore(first_edge_id);
            std::size_t second_edge_core = graph.getEdgeCore(second_edge_id);
            std::size_t first_edge_size = graph.getEdgeSize(first_edge_id) - 1;
//            if first satisfy the condition, just return the core value
            if (first_edge_size >= first_edge_core) {
                return first_edge_core;
            }
            if (first_edge_size >= second_edge_core) {
                return first_edge_size;
            }
        }

        for (std::size_t edge_index = edge_offset.first; edge_index < edge_offset.second; ++edge_index) {
            std::size_t edge_id = graph.node_edge_simplices[edge_index];
            std::size_t edge_core = graph.getEdgeCore(edge_id);
            if (core_weight_set.size() >= edge_core) return core_weight_set.size();
            std::pair<std::size_t, std::size_t> edge_nodes_offset = graph.getNodes(edge_id);
            for (std::size_t i = edge_nodes_offset.first; i < edge_nodes_offset.second; ++i) {
                std::size_t current_node = graph.edge_node_simplices[i];
                if (current_node == node_id) continue;
                core_weight_set.insert(current_node);
                if (core_weight_set.size() >= edge_core) return edge_core;
            }
        }

//        should never reach here
        std::cout << "node_id: " << node_id << "edge_offset: " << edge_offset.first << " " << edge_offset.second
                  << " core_weight_set: " << core_weight_set.size() << " node_core: " << graph.getNodeCore(node_id)
                  << std::endl;
        throw std::runtime_error("CoreEvaluationFinal error");
    }

//    exact same as Alg 5 in the paper
    static std::size_t CoreEvaluation(Hypergraph &graph, std::size_t node_id) {
        std::size_t currentNodeCore = graph.nodes_core[node_id];
        std::size_t count_core_nbr = 0;
        std::vector<std::size_t> count(currentNodeCore + 1, 0);
        std::pair<std::size_t, std::size_t> nbr_offset = graph.getNbr(node_id);
        for (std::size_t nbr = nbr_offset.first; nbr < nbr_offset.second; ++nbr) {
            std::vector<size_t> IntersectionEdges = graph.findIntersectionEdge(nbr, node_id, graph.nbr_simplices[nbr]);
            std::size_t max_core = 0;
            for (auto edge_id: IntersectionEdges) {
                max_core = std::max(max_core, graph.getEdgeCore(edge_id));
                if (max_core >= currentNodeCore) {
                    ++count_core_nbr;
                    if (count_core_nbr >= currentNodeCore) {
                        return currentNodeCore;
                    }
                    break;
                }
            }
            ++count[std::min(currentNodeCore, max_core)];
        }
        std::size_t sum = 0;
        for (auto k = currentNodeCore; k > 0; --k) {
            sum += count[k];
            if (sum >= k) { // check if there are k edges with core value >= k
                currentNodeCore = k;
                break; // break the inner loop if the core value is updated
            }
        }
        return currentNodeCore;
    }


    std::vector<std::size_t> nodes_core; // for node

//  edges_edge_simplices[nodes_edge_nvertx_offset[v] : nodes_edge_nvertx_offset[v+1]] is all edge id that contains node v
    std::vector<std::size_t> nodes_edge_nvertx_offset;
    std::vector<std::size_t> node_edge_simplices;

//    edge_simplices[edge_nvertx_offset[v] : edge_nvertx_offset[v+1]] is all node id that is connected to edge v
    std::vector<std::size_t> edge_node_nvertx_offset;
    std::vector<std::size_t> edge_node_simplices;

//    edges_core[e] is edge data of edge e
    std::vector<std::size_t> edges_core;

//    neighbors of node v is in nbr_simplices[nbr_nvertx_offset[v] : nbr_nvertx_offset[v+1]]
    std::vector<std::size_t> nbr_nvertx_offset;
    std::vector<std::size_t> nbr_simplices;

    std::vector<std::vector<size_t>> nbr_intersection;

    void resetCore();

private:
    void loadFromFiles_helper(std::size_t numNodes);

};


#endif //HYPERGRAPHKCORE_HYPERGRAPH_H
