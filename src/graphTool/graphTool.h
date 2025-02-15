//
// Created by _ on 20/11/2023.
//

#ifndef HYPERGRAPHKCORE_GRAPHTOOL_H
#define HYPERGRAPHKCORE_GRAPHTOOL_H

#include "../Hypergraph.h"
#include <cstring>

enum sortType {
    DEGREE,
    NBR
};

enum orderType {
    ASC,
    DESC
};

orderType cover_to_orderType(const std::string &order);

std::string orderType_to_string(orderType order);

void printGraphInfo(Hypergraph &graph);

void generateHypergraph_random(
        std::size_t
        numNodes,
        std::size_t numEdges,
        std::size_t
        maxEdgeSize,
        const std::string &graph_path
);

void generateHypergraph_deg_WeiBull(
        std::size_t
        numNodes,
        std::size_t numEdges,
        const std::string &graph_path
);

void generateHypergraph_deg_PowerLaw(
        std::size_t
        numNodes,
        std::size_t numEdges,
        const std::string &graph_path
);

void generateHypergraph_nbr_PowerLaw(
        std::size_t
        numNodes,
        std::size_t numEdges,
        const std::string &graph_path
);

void organize_nodes(const std::string &graph_path);

void vary_nodes(const std::string &graph_path);

void vary_edges(const std::string &graph_path);

void sortNodes(const std::string &graph_path, sortType type, orderType order);

void sortEdges(const std::string &graph_path, orderType order);

void sortEdgeNodes(const std::string &graph_path, orderType node_order, orderType edge_order);

void getGraphDetiles(Hypergraph &graph, const std::string &output_path);

#endif //HYPERGRAPHKCORE_GRAPHTOOL_H
