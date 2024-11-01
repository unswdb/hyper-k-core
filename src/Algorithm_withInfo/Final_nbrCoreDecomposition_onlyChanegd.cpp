//
// Created by _ on 19/9/2023.
//

#include "../Hypergraph.h"
#include <chrono>
#include <map>
#include "NbrCD.h"

void withInfo::nbrCoreDecomposition_onlyChanged(Hypergraph &graph) {
    std::cout << "final only changed " << std::endl;

    auto init_start = std::chrono::high_resolution_clock::now();
//    init core
    std::vector<bool> changed(graph.getGraphNodeSize(), true);
    for (std::size_t node = 1; node < graph.getGraphNodeSize(); ++node) {
        auto numNbr = graph.getNbrCount(node);
        graph.nodes_core[node] = numNbr;

        auto edge_offset = graph.getEdges(node);
        for (std::size_t edge_index = edge_offset.first; edge_index < edge_offset.second; ++edge_index) {
            auto edge = graph.node_edge_simplices[edge_index];
            auto edge_size = graph.getNodes(edge).second - graph.getNodes(edge).first;
            if (edge_size - 1 >= numNbr) {
                changed[node] = false;
                break;
            }
        }
    }

//    init core value for each edge, as the smallest core value of the nodes in the edge
    for (std::size_t edge_id = 1; edge_id < graph.edge_node_nvertx_offset.size(); ++edge_id) {
        std::pair<std::size_t, std::size_t> edge_offset = graph.getNodes(edge_id);
        for (std::size_t i = edge_offset.first; i < edge_offset.second; ++i) {
            graph.edges_core[edge_id] = std::min(graph.nodes_core[graph.edge_node_simplices[i]],
                                                 graph.edges_core[edge_id]);
        }
    }


    std::chrono::duration<double> diff = std::chrono::high_resolution_clock::now() - init_start;
    std::cout << "init_Time: " << diff.count() << " s" << std::endl;

    auto start = std::chrono::high_resolution_clock::now();
    bool updated = true;
    auto decide_time = 0.0;
    auto compute_time = 0.0;
    std::size_t iter = 0;
    while (updated) {
        ++iter;
        updated = false;
        std::size_t loopedNode = 0;
        std::size_t changedNode = 0;
        for (std::size_t currentNodesId = 1; currentNodesId < graph.getGraphNodeSize(); ++currentNodesId) {
            if (!changed[currentNodesId]) {
                continue;
            }
            changed[currentNodesId] = false;
            ++loopedNode;
            std::size_t oldCoreV = graph.nodes_core[currentNodesId];
            std::size_t & currentNodeCore = graph.nodes_core[currentNodesId];
            auto time_start = std::chrono::high_resolution_clock::now();
            currentNodeCore = Hypergraph::CoreEvaluationFinal(graph, currentNodesId);
            compute_time += std::chrono::duration<double>(
                    std::chrono::high_resolution_clock::now() - time_start).count();
            if (oldCoreV > currentNodeCore) { // if the core value is updated, update the core value of the edge
                ++changedNode;
                updated = true;
                time_start = std::chrono::high_resolution_clock::now();
                std::pair<std::size_t, std::size_t> edge_offset = graph.getEdges(currentNodesId);
                for (auto edge_id_index = edge_offset.first; edge_id_index < edge_offset.second; ++edge_id_index) {
                    std::size_t edge_id = graph.node_edge_simplices[edge_id_index];
                    std::size_t & edge_core = graph.getEdgeCore(edge_id);
                    if (edge_core > currentNodeCore) {
                        std::pair<std::size_t, std::size_t> in_edge_node_offset = graph.getNodes(edge_id);
                        for (std::size_t node_in_edge_index = in_edge_node_offset.first;
                             node_in_edge_index < in_edge_node_offset.second; ++node_in_edge_index) {
                            std::size_t node_in_edge = graph.edge_node_simplices[node_in_edge_index];
                            if (graph.getNodeCore(node_in_edge) == edge_core) {
                                changed[node_in_edge] = true;
                            }
                        }
                        edge_core = currentNodeCore;
                    }
                }
                decide_time += std::chrono::duration<double>(
                        std::chrono::high_resolution_clock::now() - time_start).count();
            }
        }

        std::cout << iter << "," << loopedNode << "," << changedNode << std::endl;

    }


    std::cout << "iter: " << iter << std::endl;
    diff = std::chrono::high_resolution_clock::now() - start;
    std::cout << "decide_time: " << decide_time << " s" << std::endl;
    std::cout << "compute_time: " << compute_time << " s" << std::endl;
    std::cout << "exc_Time: " << diff.count() << " s" << std::endl;
}

