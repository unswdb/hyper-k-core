//
// Created by _ on 2/11/2023.
//

#include "../Hypergraph.h"
#include <chrono>
#include <map>
#include <iomanip>

void fullNbrCoreDecomposition(Hypergraph &graph) {
    std::cout << "full " << std::endl;

    auto init_start = std::chrono::high_resolution_clock::now();

    for (std::size_t node = 1; node < graph.getGraphNodeSize(); ++node) {
        graph.nodes_core[node] = graph.getNbrCount(node);
    }

//    init core value for each edge, as the smallest core value of the nodes in the edge
    for (std::size_t edge_id = 1; edge_id < graph.edge_node_nvertx_offset.size(); ++edge_id) {
        std::pair<std::size_t, std::size_t> edge_offset = graph.getNodes(edge_id);
        for (std::size_t i = edge_offset.first; i < edge_offset.second; ++i) {
            graph.edges_core[edge_id] = std::min(graph.nodes_core[graph.edge_node_simplices[i]],
                                                 graph.edges_core[edge_id]);
        }
    }


    std::vector<std::size_t> support(graph.getGraphNodeSize() + 1, 0);
//    for (std::size_t node_id = 1; node_id < graph.getGraphNodeSize(); ++node_id) {
//        std::pair<std::size_t, std::size_t> deg_offset = graph.getEdges(node_id);
//        std::unordered_set<std::size_t> nbrs(100);
//        for (std::size_t i = deg_offset.first; i < deg_offset.second; ++i) {
//            std::pair<std::size_t, std::size_t> edge_offset = graph.getNodes(graph.node_edge_simplices[i]);
//            for (std::size_t j = edge_offset.first; j < edge_offset.second; ++j) {
//                if (graph.edge_node_simplices[j] != node_id) nbrs.insert(graph.edge_node_simplices[j]);
//            }
//        }
//
//        for (auto nbr: nbrs) {
//            if (graph.nodes_core[nbr] >= graph.nodes_core[node_id]) {
//                support[node_id]++;
//            }
//        }
//    }

    std::chrono::duration<double> diff = std::chrono::high_resolution_clock::now() - init_start;
    std::cout << "init_Time: " << diff.count() << " s" << std::endl;

    auto start = std::chrono::high_resolution_clock::now();
    bool updated = true;
    while (updated) {
        updated = false;
        for (std::size_t currentNodesId = 1; currentNodesId < graph.getGraphNodeSize(); ++currentNodesId) {
            std::size_t & currentNodeCore = graph.nodes_core[currentNodesId];
            if (support[currentNodesId] >= currentNodeCore) continue;


            std::unordered_set<std::size_t> nbrs;
            std::pair<std::size_t, std::size_t> edge_offset = graph.getEdges(currentNodesId);
            std::sort(graph.node_edge_simplices.begin() + static_cast<long>(edge_offset.first),
                      graph.node_edge_simplices.begin() + static_cast<long>(edge_offset.second),
                      [&graph](std::size_t a, std::size_t b) {
                          return graph.edges_core[a] > graph.edges_core[b];
                      });

            std::size_t core_edge_index = edge_offset.first;
            for (; core_edge_index < edge_offset.second; ++core_edge_index) {
                std::size_t edge_id = graph.node_edge_simplices[core_edge_index];
                std::size_t edge_core = graph.getEdgeCore(edge_id);
                if (nbrs.size() > edge_core) {
                    currentNodeCore = nbrs.size();
                    break;
                }

                std::pair<std::size_t, std::size_t> edge_nodes_offset = graph.getNodes(edge_id);
                for (std::size_t i = edge_nodes_offset.first; i < edge_nodes_offset.second; ++i) {
                    std::size_t current_node = graph.edge_node_simplices[i];
                    if (current_node == currentNodesId) continue;
                    nbrs.insert(current_node);
                }
                if (nbrs.size() >= edge_core) {
                    currentNodeCore = edge_core;
                    ++core_edge_index;
                    for (; core_edge_index < edge_offset.second; ++core_edge_index) {
                        edge_id = graph.node_edge_simplices[core_edge_index];
                        edge_core = graph.getEdgeCore(edge_id);
                        if (edge_core < currentNodeCore) {
                            break;
                        }
                        edge_nodes_offset = graph.getNodes(edge_id);
                        for (std::size_t i = edge_nodes_offset.first; i < edge_nodes_offset.second; ++i) {
                            std::size_t current_node = graph.edge_node_simplices[i];
                            if (current_node == currentNodesId) continue;
                            nbrs.insert(current_node);
                        }
                    }

                    break;
                }
            }

            support[currentNodesId] = nbrs.size();


            std::sort(graph.node_edge_simplices.begin() + static_cast<long>(edge_offset.first),
                      graph.node_edge_simplices.begin() + static_cast<long>(edge_offset.second),
                      [](std::size_t a, std::size_t b) {
                          return a < b;
                      });

            edge_offset = graph.getEdges(currentNodesId);

            for (auto edge_id_index = edge_offset.first; edge_id_index < edge_offset.second; ++edge_id_index) {
                std::size_t edge_id = graph.node_edge_simplices[edge_id_index];
                if (graph.getEdgeCore(edge_id) > currentNodeCore) {
                    std::pair<std::size_t, std::size_t> in_edge_node_offset = graph.getNodes(edge_id);
                    for (std::size_t node_in_edge_index = in_edge_node_offset.first;
                         node_in_edge_index < in_edge_node_offset.second; ++node_in_edge_index) {
                        std::size_t node_in_edge = graph.edge_node_simplices[node_in_edge_index];
                        std::pair<std::size_t, std::size_t> in_edge_node_nbr_offset = graph.getNodes(edge_id);
                        for (std::size_t node_in_edge_nbr_index = in_edge_node_nbr_offset.first;
                             node_in_edge_nbr_index < in_edge_node_nbr_offset.second; ++node_in_edge_nbr_index) {
                            std::size_t node_in_edge_nbr = graph.edge_node_simplices[node_in_edge_nbr_index];
                            if (node_in_edge_nbr == node_in_edge) continue;
                            if (support[node_in_edge_nbr] < graph.getNodeCore(node_in_edge_nbr)) continue;
                            if (graph.getEdgeCore(edge_id) != graph.getNodeCore(node_in_edge_nbr)) continue;

                            bool isBiggest = true;

                            std::vector<size_t> IntersectionEdges = graph.computeIntersectionEdge(node_in_edge,
                                                                                                  node_in_edge_nbr);

                            for (auto intersection_edge_id: IntersectionEdges) {
                                if (intersection_edge_id == edge_id) continue;
                                if (graph.getEdgeCore(intersection_edge_id) == graph.getNodeCore(node_in_edge_nbr)) {
                                    isBiggest = false;
                                    break;
                                }
                            }

                            if (isBiggest) {
                                --support[node_in_edge_nbr];
                                if (support[node_in_edge_nbr] < graph.getNodeCore(node_in_edge_nbr)) {
                                    updated = true;
                                }
                            }
                        }
                    }
                    graph.getEdgeCore(edge_id) = currentNodeCore;
                }
            }
        }
    }
    diff = std::chrono::high_resolution_clock::now() - start;
    std::cout << "exc_Time: " << diff.count() << " s" << std::endl;
}

