//
// Created by _ on 2/11/2023.
//

#include "../Hypergraph.h"
#include "NbrCD.h"
#include <chrono>
#include <map>

void withInfo::nbrCoreDecomposition_withNbrs(Hypergraph &graph) {
    std::cout << "final Nbrs " << std::endl;

    auto init_start = std::chrono::high_resolution_clock::now();

    graph.computeNbrs();

    graph.nbr_intersection.resize(graph.nbr_simplices.size());
    // for (std::size_t node_id = 1; node_id < graph.getGraphNodeSize(); ++node_id) {
    //     for (std::size_t nbr_index = graph.nbr_nvertx_offset[node_id - 1];
    //          nbr_index < graph.nbr_nvertx_offset[node_id]; ++nbr_index) {
    //          graph.findIntersectionEdge(node_id, graph.nbr_simplices[nbr_index],graph.nbr_intersection[nbr_index]);
    //     }
    // }

//    init nbrs map
    for (std::size_t node = 1; node < graph.getGraphNodeSize(); ++node) {
        std::pair<std::size_t, std::size_t> nbr_offset = graph.getNbr(node);
        graph.nodes_core[node] = nbr_offset.second - nbr_offset.first;
    }

//    init core value for each edge, as the smallest core value of the nodes in the edge
    for (std::size_t edge_id = 1; edge_id < graph.edge_node_nvertx_offset.size(); ++edge_id) {
        std::pair<std::size_t, std::size_t> edge_offset = graph.getNodes(edge_id);
        for (std::size_t i = edge_offset.first; i < edge_offset.second; ++i) {
            graph.edges_core[edge_id] = std::min(graph.nodes_core[graph.edge_node_simplices[i]],
                                                 graph.edges_core[edge_id]);
        }
    }

    std::vector<bool> changed(graph.getGraphNodeSize(), true);

    std::chrono::duration<double> diff = std::chrono::high_resolution_clock::now() - init_start;
    std::cout << "init_Time: " << diff.count() << " s" << std::endl;


    auto start = std::chrono::high_resolution_clock::now();
    bool updated = true;
    std::size_t iter = 0;
    while (updated) {
        ++iter;
        updated = false;
        for (std::size_t currentNodesId = 1; currentNodesId < graph.getGraphNodeSize(); ++currentNodesId) {
            if (!changed[currentNodesId]) {
                continue;
            }
            changed[currentNodesId] = false;
            std::size_t & currentNodeCore = graph.nodes_core[currentNodesId];
            std::size_t oldCoreV = currentNodeCore;
            currentNodeCore = Hypergraph::CoreEvaluation(graph, currentNodesId);

            if (oldCoreV > currentNodeCore) { // if the core value is updated, update the core value of the edgesgai
                std::pair<std::size_t, std::size_t> edge_offset = graph.getEdges(currentNodesId);
                std::unordered_set<std::size_t> t_level_nbr;
                for (auto edge_id_index = edge_offset.first; edge_id_index < edge_offset.second; ++edge_id_index) {
                    std::size_t edge_id = graph.node_edge_simplices[edge_id_index];
                    if (graph.getEdgeCore(edge_id) > currentNodeCore) {
                        std::pair<std::size_t, std::size_t> in_edge_node_offset = graph.getNodes(edge_id);
                        for (std::size_t node_in_edge_index = in_edge_node_offset.first;
                             node_in_edge_index < in_edge_node_offset.second; ++node_in_edge_index) {
                            std::size_t node_in_edge = graph.edge_node_simplices[node_in_edge_index];
                            if (graph.getNodeCore(node_in_edge) == graph.getEdgeCore(edge_id)) {
                                updated = true;
                                changed[node_in_edge] = true;
                            }
                        }
                        graph.getEdgeCore(edge_id) = currentNodeCore;
                    }
                }
            }
        }
    }
    std::cout << "iter: " << iter << std::endl;
    diff = std::chrono::high_resolution_clock::now() - start;
    std::cout << "exc_Time: " << diff.count() << " s" << std::endl;
}