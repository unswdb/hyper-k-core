//
// Created by _ on 19/9/2023.
//

#include "../Hypergraph.h"
#include <chrono>
#include <omp.h>
#include <atomic>
#include <cmath>
#include <numeric>
#include "NbrCD.h"


void withInfo::ParNbrCoreDecomposition(Hypergraph &graph, int numThreads, int computeThreshold) {
    std::cout << "par final " << std::endl;

    std::cout << "numThreads: " << numThreads << std::endl;
    std::cout << "computeThreshold: " << computeThreshold << std::endl;
    const std::size_t avgEdgeSize = graph.getAvgEdgeSize();
    const std::size_t avgNodeSize = graph.getAvgDegree();
    std::size_t avgComputePreNode = avgEdgeSize * avgNodeSize;
    auto nodePerThread = std::max(static_cast<size_t>(computeThreshold) / avgComputePreNode, 1ul);
    auto edgePerThread = std::max(static_cast<size_t>(computeThreshold) / avgEdgeSize, 1ul);
    std::cout << "nodePerThread: " << nodePerThread << std::endl;
    std::cout << "edgePerThread: " << edgePerThread << std::endl;

    omp_set_num_threads(numThreads);

    auto init_start = omp_get_wtime();

    auto nodes_lower_bound = std::vector<std::size_t>(graph.getGraphNodeSize() + 1, 0);

    auto init_node_time = omp_get_wtime();

#pragma omp parallel for schedule(dynamic, nodePerThread) default(shared) shared(graph)
    for (std::size_t node = 1; node < graph.getGraphNodeSize(); ++node) {
        std::pair<std::size_t, std::size_t> deg_offset = graph.getEdges(node);
        if (deg_offset.second - deg_offset.first == 1) {
//            if only one edge, the core value is the number of nodes in the edge
            std::pair<std::size_t, std::size_t> edge_offset = graph.getNodes(
                    graph.node_edge_simplices[deg_offset.first]);
            graph.nodes_core[node] = edge_offset.second - edge_offset.first - 1;
            nodes_lower_bound[node] = graph.nodes_core[node];
            continue;
        }

        if (deg_offset.second - deg_offset.first == 2) {
            std::pair<std::size_t, std::size_t> edge_offset = graph.getNodes(
                    graph.node_edge_simplices[deg_offset.first]);
            std::pair<std::size_t, std::size_t> edge_offset2 = graph.getNodes(
                    graph.node_edge_simplices[deg_offset.first + 1]);
            Counter c;
            std::set_union(graph.edge_node_simplices.begin() + static_cast<long>(edge_offset.first),
                           graph.edge_node_simplices.begin() + static_cast<long>(edge_offset.second),
                           graph.edge_node_simplices.begin() + static_cast<long>(edge_offset2.first),
                           graph.edge_node_simplices.begin() + static_cast<long>(edge_offset2.second),
                           std::back_inserter(c));
            graph.nodes_core[node] = c.count - 1;
            if (edge_offset.second - edge_offset.first > edge_offset2.second - edge_offset2.first) {
                nodes_lower_bound[node] = edge_offset.second - edge_offset.first - 1;
            } else {
                nodes_lower_bound[node] = edge_offset2.second - edge_offset2.first - 1;
            }
            continue;
        }

        std::unordered_set<std::size_t> nbrs(100);
        for (std::size_t i = deg_offset.first; i < deg_offset.second; ++i) {
            std::pair<std::size_t, std::size_t> edge_offset = graph.getNodes(graph.node_edge_simplices[i]);
            for (std::size_t j = edge_offset.first; j < edge_offset.second; ++j) {
                if (graph.edge_node_simplices[j] != node) nbrs.insert(graph.edge_node_simplices[j]);
            }
            nodes_lower_bound[node] = std::max(nodes_lower_bound[node],
                                               edge_offset.second - edge_offset.first - 1);
        }
        graph.nodes_core[node] = nbrs.size();
    }
    std::cout << "init_node_time: " << omp_get_wtime() - init_node_time << " s" << std::endl;

    auto init_edge_time = omp_get_wtime();
#pragma omp parallel for schedule(dynamic, edgePerThread) default(shared) shared(graph)
    for (std::size_t edge_id = 1; edge_id < graph.edge_node_nvertx_offset.size(); ++edge_id) {
        std::pair<std::size_t, std::size_t> edge_offset = graph.getNodes(edge_id);
        for (std::size_t i = edge_offset.first; i < edge_offset.second; ++i) {
            graph.edges_core[edge_id] = std::min(graph.nodes_core[graph.edge_node_simplices[i]],
                                                 graph.edges_core[edge_id]);
        }
    }
    std::cout << "init_edge_time: " << omp_get_wtime() - init_edge_time << " s" << std::endl;

    std::vector<std::atomic<bool>> changed(graph.getGraphNodeSize());
#pragma omp parallel for schedule(dynamic, nodePerThread) default(shared) shared(changed)
    for (auto &change: changed) {
        change = true;
    }

    auto diff = omp_get_wtime() - init_start;
    std::cout << "init_Time: " << diff << " s" << std::endl;

    bool updated = true;
    std::vector<std::size_t> edgeOldCore = graph.edges_core;

    double start = omp_get_wtime();


    while (updated) {
        std::atomic<int> num_updated = 0;
        updated = false;
#pragma omp parallel for schedule(dynamic, nodePerThread) default(shared) shared(graph, edgeOldCore, updated)
        for (std::size_t currentNodesId = 1; currentNodesId < graph.getGraphNodeSize(); ++currentNodesId) {
            if (!changed[currentNodesId]) continue;
            changed[currentNodesId] = false;
            if (nodes_lower_bound[currentNodesId] >= graph.nodes_core[currentNodesId]) continue;
            std::size_t oldCoreV = graph.nodes_core[currentNodesId];
            std::size_t & currentNodeCore = graph.nodes_core[currentNodesId];
            currentNodeCore = Hypergraph::CoreEvaluationFinal(graph, currentNodesId);

            if (oldCoreV > currentNodeCore) { // if the core value is updated, update the core value of the edgesgai
                updated = true;
                std::pair<std::size_t, std::size_t> edge_offset = graph.getEdges(currentNodesId);
                for (auto edge_id_index = edge_offset.first; edge_id_index < edge_offset.second; ++edge_id_index) {
                    std::size_t edge_id = graph.node_edge_simplices[edge_id_index];

                    std::size_t & edge_core = graph.getEdgeCore(edge_id);
                    if (edge_core > currentNodeCore) {
                        while (true) {
                            std::size_t oldCore = edgeOldCore[edge_id];
                            if (currentNodeCore >= oldCore) break;
                            if (__atomic_compare_exchange_n(&edgeOldCore[edge_id], &oldCore, currentNodeCore,
                                                            false, __ATOMIC_RELAXED, __ATOMIC_RELAXED)) {
                                break;
                            }
                        }
                    }
                }
            }
        }

#pragma omp parallel for schedule(dynamic, edgePerThread) default(shared) shared(graph, edgeOldCore, changed)
        for (std::size_t currentEdgeId = 1; currentEdgeId < graph.edge_node_nvertx_offset.size(); ++currentEdgeId) {
            if (edgeOldCore[currentEdgeId] >= graph.edges_core[currentEdgeId]) continue;
            std::pair<std::size_t, std::size_t> in_edge_node_offset = graph.getNodes(currentEdgeId);
            for (std::size_t node_in_edge_index = in_edge_node_offset.first;
                 node_in_edge_index < in_edge_node_offset.second; ++node_in_edge_index) {
                std::size_t node_in_edge = graph.edge_node_simplices[node_in_edge_index];
                if (graph.nodes_core[node_in_edge] <= graph.edges_core[currentEdgeId] &&
                    graph.nodes_core[node_in_edge] > edgeOldCore[currentEdgeId]) {
                    updated = true;
                    changed[node_in_edge] = true;
                    num_updated++;
                }
            }
            graph.edges_core[currentEdgeId] = edgeOldCore[currentEdgeId];
        }
        std::cout << "num_updated: " << num_updated << std::endl;
    }
    diff = omp_get_wtime() - start;
    std::cout << "exc_Time: " << diff << " s" << std::endl;
}