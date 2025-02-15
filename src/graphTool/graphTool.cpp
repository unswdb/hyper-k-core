//
// Created by _ on 20/11/2023.
//

#include <iomanip>
#include <omp.h>
#include "graphTool.h"
#include <iostream>
#include <vector>
#include <random>
#include <fstream>
#include <algorithm>
#include <sstream>

void getGraphDetiles(Hypergraph &graph, const std::string &output_path) {
    std::cout << std::fixed << std::setprecision(2); // set the format to fixed and 2 decimal places

    std::cout << "node size: " << graph.getGraphNodeSize() << std::endl;
    std::cout << "edge size: " << graph.getGraphEdgeSize() << std::endl;

    const std::string nbrPath = output_path + "_nbr.txt";
    auto nbrFile = std::ofstream(nbrPath);

    std::size_t averageNbrSize = 0;
    std::size_t maxNbrSize = 0;

    for (std::size_t node = 1; node < graph.getGraphNodeSize(); ++node) {
        auto nbrSize = graph.getNbrCount(node);
        averageNbrSize += nbrSize;
        maxNbrSize = std::max(maxNbrSize, nbrSize);
        nbrFile << nbrSize << std::endl;
    }
    nbrFile.close();
    std::cout << "average nbr size: "
              << static_cast<double>(averageNbrSize) / static_cast<double>(graph.getGraphNodeSize() - 1) << std::endl;
    std::cout << "max nbr size: " << maxNbrSize << std::endl;

    const std::string edgePath = output_path + "_edge.txt";
    auto edgeFile = std::ofstream(edgePath);

    std::size_t averageEdgeSize = 0;
    std::size_t maxEdgeSize = 0;
    for (std::size_t edge = 1; edge < graph.getGraphEdgeSize(); ++edge) {
        auto edgeSize = graph.getNodes(edge).second - graph.getNodes(edge).first;
        averageEdgeSize += edgeSize;
        maxEdgeSize = std::max(maxEdgeSize, edgeSize);
        edgeFile << edgeSize << std::endl;
    }
    edgeFile.close();
    std::cout << "average edge size: "
              << static_cast<double>(averageEdgeSize) / static_cast<double>(graph.getGraphEdgeSize() - 1) << std::endl;
    std::cout << "max edge size: " << maxEdgeSize << std::endl;


    const std::string degreePath = output_path + "_degree.txt";
    auto degreeFile = std::ofstream(degreePath);
    std::size_t averageDegree = 0;
    std::size_t maxDegree = 0;
    for (std::size_t node = 1; node < graph.getGraphNodeSize(); ++node) {
        auto degree = graph.getEdges(node);
        averageDegree += degree.second - degree.first;
        maxDegree = std::max(maxDegree, degree.second - degree.first);
        degreeFile << degree.second - degree.first << std::endl;
    }
    degreeFile.close();
    std::cout << "average degree: "
              << static_cast<double>(averageDegree) / static_cast<double>(graph.getGraphNodeSize() - 1)
              << std::endl;
    std::cout << "max degree: " << maxDegree << std::endl;

    std::cout << "save edge size to " << edgePath << std::endl;
    std::cout << "save degree size to " << degreePath << std::endl;
    std::cout << "save nbr size to " << nbrPath << std::endl;
}

void printGraphInfo(Hypergraph &graph) {
    std::cout << std::fixed << std::setprecision(2);

    std::cout << "node size: " << graph.getGraphNodeSize() << std::endl;
    std::cout << "edge size: " << graph.getGraphEdgeSize() << std::endl;
    std::size_t averageNbrSize = 0;
    std::size_t maxNbrSize = 0;
#pragma omp parallel for schedule(dynamic, 100) default(shared) reduction(+:averageNbrSize) num_threads(16) reduction(max:maxNbrSize)
    for (std::size_t node = 1; node < graph.getGraphNodeSize(); ++node) {
        auto nbrSize = graph.getNbrCount(node);
        averageNbrSize += nbrSize;
        maxNbrSize = std::max(maxNbrSize, nbrSize);
    }
    std::cout << "average nbr size: "
              << static_cast<double>(averageNbrSize) / static_cast<double>(graph.getGraphNodeSize() - 1) << std::endl;
    std::cout << "max nbr size: " << maxNbrSize << std::endl;

    std::size_t averageEdgeSize = 0;
    std::size_t maxEdgeSize = 0;

#pragma omp parallel for schedule(dynamic, 100) default(shared) reduction(+:averageEdgeSize) num_threads(16) reduction(max:maxEdgeSize)
    for (std::size_t edge = 1; edge < graph.getGraphEdgeSize(); ++edge) {
        auto edgeSize = graph.getNodes(edge).second - graph.getNodes(edge).first;
        averageEdgeSize += edgeSize;
        maxEdgeSize = std::max(maxEdgeSize, edgeSize);
    }
    std::cout << "average edge size: "
              << static_cast<double>(averageEdgeSize) / static_cast<double>(graph.getGraphEdgeSize() - 1) << std::endl;
    std::cout << "max edge size: " << maxEdgeSize << std::endl;

    std::size_t averageDegree = 0;
    std::size_t maxDegree = 0;
    for (std::size_t node = 1; node < graph.getGraphNodeSize(); ++node) {
        auto degree = graph.getEdges(node);
        averageDegree += degree.second - degree.first;
        maxDegree = std::max(maxDegree, degree.second - degree.first);
    }
    std::cout << "average degree: "
              << static_cast<double>(averageDegree) / static_cast<double>(graph.getGraphNodeSize() - 1)
              << std::endl;
    std::cout << "max degree: " << maxDegree << std::endl;
}

void coverNode(const std::string &simpliceseFile) {
    std::unordered_map<std::string, int> simplicese_to_index;
    std::vector<std::vector<int>> originalSimpliceseList;
    std::string line;

    std::ifstream file(simpliceseFile);
    if (!file.is_open()) {
        std::cerr << "Error: file not found" << std::endl;
        return;
    }

    int index = 1;
    while (std::getline(file, line)) {
        std::vector<int> coveredLine;
        size_t pos = 0;
        std::string token;
        while ((pos = line.find(',')) != std::string::npos) {
            token = line.substr(0, pos);
            if (simplicese_to_index.find(token) == simplicese_to_index.end()) {
                simplicese_to_index[token] = index++;
            }
            coveredLine.push_back(simplicese_to_index[token]);
            line.erase(0, pos + 1);
        }
        if (!line.empty() && simplicese_to_index.find(line) == simplicese_to_index.end()) {
            simplicese_to_index[line] = index++;
        }
        coveredLine.push_back(simplicese_to_index[line]);

        originalSimpliceseList.push_back(coveredLine);
    }
    file.close();
    std::cout << std::endl;

    std::ofstream out_file(simpliceseFile, std::ofstream::trunc);
    std::size_t line_count = 0;
    for (const auto &simplicese: originalSimpliceseList) {
        for (size_t i = 0; i < simplicese.size(); ++i) {
            out_file << simplicese[i];
            if (i < simplicese.size() - 1) out_file << ",";
        }
        if (line_count++ < originalSimpliceseList.size() - 1) out_file << "\n";
    }
    out_file.close();
    std::cout << std::endl;
}

void organize_nodes(const std::string &graph_path) {
    std::cout << "loading graph" << std::endl;
    std::ifstream hypFile(graph_path);
    if (!hypFile.is_open()) {
        throw std::runtime_error("Error to open file " + graph_path);
    }
    std::vector<std::size_t> edge_node_nvertx_offset;
    std::vector<std::size_t> edge_node_simplices;
    std::size_t prev = 0;
    std::size_t numNodes = 0;
    edge_node_nvertx_offset.push_back(prev);
    while (!hypFile.eof()) {
        std::string line;
        std::getline(hypFile, line);
        std::replace(line.begin(), line.end(), ',', ' ');
        std::stringstream ss(line);
        std::size_t temp;
        while (ss >> temp) {
            edge_node_simplices.push_back(temp);
            prev++;
            numNodes = std::max(numNodes, temp);
        }
        edge_node_nvertx_offset.push_back(prev);
    }
    std::cout << "max node: " << numNodes << std::endl;
    std::vector<bool> isVertex(numNodes + 1, false);
    std::vector<std::size_t> reduced_value_offset(numNodes + 1, 0);
    for (std::size_t v: edge_node_simplices) {
        isVertex[v] = true;
    }
    std::size_t reduced_value = 0;
    for (std::size_t i = 1; i < numNodes + 1; ++i) {
        if (!isVertex[i]) {
            ++reduced_value;
        }
        reduced_value_offset[i] = reduced_value;
    }

    for (auto &edge_node_simplice: edge_node_simplices) {
        edge_node_simplice -= reduced_value_offset[edge_node_simplice];
    }

    std::ofstream output(graph_path);
    std::cout << "Graph organize completed. writing to " << graph_path << std::endl;
    for (std::size_t i = 1; i < edge_node_nvertx_offset.size(); ++i) {
        std::unordered_set<std::size_t> edge;
        for (std::size_t j = edge_node_nvertx_offset[i - 1]; j < edge_node_nvertx_offset[i]; ++j) {
            edge.insert(edge_node_simplices[j]);
        }
        if (edge.size() < 2) {
            continue;
        }
//        sort edege
        auto edge_vector = std::vector<std::size_t>(edge.begin(), edge.end());
        std::sort(edge_vector.begin(), edge_vector.end());
        std::size_t count = 0;
        for (auto v: edge_vector) {
            output << v;
            if (count != edge.size() - 1) {
                output << ",";
            }
            count++;
        }
        output << "\n";
    }
    output.close();

//    coverNode(graph_path);

    std::cout << "Graph written to " << graph_path << std::endl;
}

void vary_nodes(const std::string &graph_path) {
    std::cout << "loading graph" << std::endl;
    std::ifstream hypFile(graph_path);
    if (!hypFile.is_open()) {
        throw std::runtime_error("Error to open file " + graph_path);
    }
    std::vector<std::size_t> edge_node_nvertx_offset;
    std::vector<std::size_t> edge_node_simplices;
    std::size_t prev = 0;
    std::size_t maxNodes = 0;
    edge_node_nvertx_offset.push_back(prev);
    while (!hypFile.eof()) {
        std::string line;
        std::getline(hypFile, line);
        std::replace(line.begin(), line.end(), ',', ' ');
        std::stringstream ss(line);
        std::size_t temp;
        while (ss >> temp) {
            edge_node_simplices.push_back(temp);
            prev++;
            maxNodes = std::max(maxNodes, temp);
        }
        edge_node_nvertx_offset.push_back(prev);
    }
    std::cout << "max node: " << maxNodes << std::endl;

    std::vector<std::size_t> prob = {2, 4, 6, 8}; // 0.2, 0.4, 0.6, 0.8
    std::vector<std::size_t> selected_nodes(maxNodes + 1);
    for (std::size_t i = 0; i <= maxNodes; ++i) {
        selected_nodes[i] = i;
    }
    std::random_device rd;
    std::mt19937 g(rd());
    std::shuffle(selected_nodes.begin(), selected_nodes.end(), g);
//    std::vector<std::unordered_set<std::size_t>> selected_nodes_set(prob.size());
//    for (std::size_t i = 0; i < prob.size(); ++i) {
//        for (std::size_t j = 0; j < static_cast<std::size_t>(static_cast<double>(maxNodes) *
//                                                             static_cast<double>(prob[i]) / 10); ++j) {
//            selected_nodes_set[i].insert(selected_nodes[j]);
//        }
//    }

    for (std::size_t p = 0; p < prob.size(); ++p) {
        std::unordered_set<std::size_t> selected_nodes_set;
        for (std::size_t j = 0; j < static_cast<std::size_t>(static_cast<double>(maxNodes) *
                                                             static_cast<double>(prob[p]) / 10); ++j) {
            selected_nodes_set.insert(selected_nodes[j]);
        }
        std::string result_path = graph_path + "_V_0." + std::to_string(prob[p]) + ".txt";
        std::ofstream output(result_path);
        std::cout << "writing to " << result_path << std::endl;
        for (std::size_t i = 1; i < edge_node_nvertx_offset.size(); ++i) {
            std::vector<std::size_t> line;
            for (std::size_t j = edge_node_nvertx_offset[i - 1]; j < edge_node_nvertx_offset[i]; ++j) {
                if (selected_nodes_set.find(edge_node_simplices[j]) != selected_nodes_set.end()) {
                    line.push_back(edge_node_simplices[j]);
                }
            }
            if (line.size() >= 2) {
                for (std::size_t j = 0; j < line.size(); ++j) {
                    output << line[j];
                    if (j != line.size() - 1) {
                        output << ",";
                    }
                }
                output << "\n";
            }
        }
        output.close();
    }
    std::cout << "Graph written to " << graph_path << std::endl;
}

void vary_edges(const std::string &graph_path) {
    std::cout << "loading graph" << std::endl;
    std::ifstream hypFile(graph_path);
    if (!hypFile.is_open()) {
        throw std::runtime_error("Error to open file " + graph_path);
    }
    std::vector<std::vector<std::size_t>> edges;
    std::size_t maxNodes = 0;
    while (!hypFile.eof()) {
        std::string line;
        std::getline(hypFile, line);
        std::replace(line.begin(), line.end(), ',', ' ');
        std::stringstream ss(line);
        std::size_t temp;
        std::vector<std::size_t> edge;
        while (ss >> temp) {
            edge.push_back(temp);
            maxNodes = std::max(maxNodes, temp);
        }
        edges.push_back(edge);
    }
    std::cout << "max node: " << maxNodes << std::endl;

    std::vector<std::size_t> prob = {2, 4, 6, 8}; // 0.2, 0.4, 0.6, 0.8
    std::random_device rd;
    std::mt19937 g(rd());
    std::shuffle(edges.begin(), edges.end(), g);
    for (std::size_t p = 0; p < prob.size(); ++p) {
        std::string result_path = graph_path + "_E_0." + std::to_string(prob[p]) + ".txt";
        std::ofstream output(result_path);
        std::cout << "writing to " << result_path << std::endl;
        for (std::size_t i = 0; i < static_cast<std::size_t>(static_cast<double>(edges.size()) *
                                                             static_cast<double>(prob[p]) / 10); ++i) {
            for (std::size_t j = 0; j < edges[i].size(); ++j) {
                output << edges[i][j];
                if (j != edges[i].size() - 1) {
                    output << ",";
                }
            }
            output << "\n";
        }
        output.close();
    }
}