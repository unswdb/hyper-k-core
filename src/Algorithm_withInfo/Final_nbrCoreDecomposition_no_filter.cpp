//
// Created by _ on 19/9/2023.
//

#include "../Hypergraph.h"
#include <chrono>
#include <map>
#include "NbrCD.h"

void withInfo::nbrCoreDecomposition_no_filter(Hypergraph &graph) {
    std::cout << "final no filter" << std::endl;

    auto init_start = std::chrono::high_resolution_clock::now();
//    init core
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
            std::size_t oldCoreV = graph.nodes_core[currentNodesId];
            ++loopedNode;
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
                    if (graph.getEdgeCore(edge_id) > currentNodeCore) {
                        graph.getEdgeCore(edge_id) = currentNodeCore;
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

