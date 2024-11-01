//
// Created by _ on 2/11/2023.
//

#include "../Hypergraph.h"
#include <map>
#include <iomanip>
#include "NbrCD.h"


static std::vector<std::size_t> getCount(Hypergraph &graph, std::size_t node_id) {
    std::size_t & currentNodeCore = graph.nodes_core[node_id];
    std::vector<std::size_t> count(currentNodeCore + 1, 0);
    std::pair<std::size_t, std::size_t> nbr_offset = graph.getNbr(node_id);
    for (std::size_t nbr = nbr_offset.first; nbr < nbr_offset.second; ++nbr) {
        std::vector<size_t> IntersectionEdges = graph.findIntersectionEdge(nbr, node_id, graph.nbr_simplices[nbr]);
        std::size_t max_core = 0;
        for (auto edge_id: IntersectionEdges) {
            max_core = std::max(max_core, graph.getEdgeCore(edge_id));
            if (max_core >= currentNodeCore) break;
        }
        ++count[std::min(currentNodeCore, max_core)];
    }
    return count;
}

void withInfo::fullNbrCoreDecomposition_withNbrs(Hypergraph &graph) {
    std::cout << "full Nbrs " << std::endl;

    auto init_start = std::chrono::high_resolution_clock::now();

    graph.computeNbrs();

    graph.nbr_intersection.resize(graph.nbr_simplices.size());
    for (std::size_t node_id = 1; node_id < graph.getGraphNodeSize(); ++node_id) {
        std::pair<std::size_t, std::size_t> nbr_offset = graph.getNbr(node_id);
        for (std::size_t nbr = nbr_offset.first; nbr < nbr_offset.second; ++nbr) {
            std::vector<size_t> IntersectionEdges = graph.findIntersectionEdge(nbr, node_id, graph.nbr_simplices[nbr]);
        }
    }

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


    std::vector<std::size_t> support(graph.getGraphNodeSize() + 1, 0);
    for (std::size_t node = 1; node < graph.getGraphNodeSize(); ++node) {
        std::pair<std::size_t, std::size_t> nbr_offset = graph.getNbr(node);
        for (std::size_t nbr_index = nbr_offset.first; nbr_index < nbr_offset.second; ++nbr_index) {
            if (graph.nodes_core[graph.nbr_simplices[nbr_index]] >= graph.nodes_core[node]) {
                support[node]++;
            }
        }
    }

    std::chrono::duration<double> diff = std::chrono::high_resolution_clock::now() - init_start;
    std::cout << "init_Time: " << diff.count() << " s" << std::endl;

    auto decide_time = 0.0;
    auto compute_time = 0.0;

    auto start_time = std::chrono::high_resolution_clock::now();
    bool updated = true;
    std::size_t iter = 0;
    std::size_t numLooped = 0;
    std::size_t numUpdated = 0;
//    graph.printEdgesCore();
    while (updated) {
        ++iter;
        updated = false;
        std::size_t looped_node = 0;
        std::size_t updated_node = 0;
        for (std::size_t currentNodesId = 1; currentNodesId < graph.getGraphNodeSize(); ++currentNodesId) {
            std::size_t & currentNodeCore = graph.nodes_core[currentNodesId];
            if (support[currentNodesId] >= currentNodeCore) continue;
            looped_node++;
//            computedNode.insert(currentNodesId);
            auto compute_start = std::chrono::high_resolution_clock::now();
            std::vector<std::size_t> count = getCount(graph, currentNodesId);
            std::size_t sum = 0;
            for (auto k = currentNodeCore; k > 0; --k) {
                sum += count[k];
                if (sum >= k) { // check if there are k edges with core value >= k
                    currentNodeCore = k;
                    support[currentNodesId] = sum;
                    break; // break the inner loop if the core value is updated
                }
            }

            compute_time += std::chrono::duration<double>(
                    std::chrono::high_resolution_clock::now() - compute_start).count();
            auto decide_start = std::chrono::high_resolution_clock::now();
            updated_node++;
            std::pair<std::size_t, std::size_t> edge_offset = graph.getEdges(currentNodesId);
            for (auto edge_id_index = edge_offset.first; edge_id_index < edge_offset.second; ++edge_id_index) {
                std::size_t edge_id = graph.node_edge_simplices[edge_id_index];
                if (graph.getEdgeCore(edge_id) > currentNodeCore) {
//                                遍历点边内所有的点，看这条边的core值的变化会不会影响到点对于邻居的core值,因为邻居在计算count的时候取得是最小值
                    std::pair<std::size_t, std::size_t> in_edge_node_offset = graph.getNodes(edge_id);
                    for (std::size_t node_in_edge_index = in_edge_node_offset.first;
                         node_in_edge_index < in_edge_node_offset.second; ++node_in_edge_index) {

                        std::size_t node_in_edge = graph.edge_node_simplices[node_in_edge_index];
                        if (node_in_edge == currentNodesId) continue;
                        std::pair<std::size_t, std::size_t> in_edge_node_nbr_offset = graph.getNodes(edge_id);
                        for (std::size_t node_in_edge_nbr_index = in_edge_node_nbr_offset.first;
                             node_in_edge_nbr_index < in_edge_node_nbr_offset.second; ++node_in_edge_nbr_index) {
                            std::size_t node_in_edge_nbr = graph.edge_node_simplices[node_in_edge_nbr_index];
                            if (node_in_edge_nbr == node_in_edge) continue;
                            if (node_in_edge_nbr == currentNodesId) continue;
                            if (support[node_in_edge_nbr] < graph.getNodeCore(node_in_edge_nbr)) continue;
                            if (graph.getEdgeCore(edge_id) != graph.getNodeCore(node_in_edge_nbr)) continue;

                            bool isBiggest = true;

                            std::vector<size_t> IntersectionEdges = graph.findIntersectionEdge(node_in_edge,
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
            decide_time += std::chrono::duration<double>(
                    std::chrono::high_resolution_clock::now() - decide_start).count();
        }


        numLooped += looped_node;
        numUpdated += updated_node;
        std::cout << iter << "," << looped_node << "," << updated_node << std::endl;
    }
    std::cout << "iter: " << iter << std::endl;
    diff = std::chrono::high_resolution_clock::now() - start_time;
    std::cout << "exc_Time: " << diff.count() << " s" << std::endl;
    std::cout << "numLooped: " << numLooped << std::endl;
    std::cout << "numUpdated: " << numUpdated << std::endl;
    std::cout << "percent: " << static_cast<double>(numUpdated) / static_cast<double>(numLooped) << std::endl;
    std::cout << "decide_time: " << decide_time << std::endl;
    std::cout << "compute_time: " << compute_time << std::endl;
}

