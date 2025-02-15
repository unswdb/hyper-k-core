//
// Created by _ on 3/4/2024.
//
#include "../Hypergraph.h"
#include "graphTool.h"
#include <iostream>
#include <vector>
#include <fstream>
#include <algorithm>
#include <sstream>
#include <random>
#include <chrono>
#include <iomanip>

double generatePowerLawRandom(double alpha, double xmin, double xmax, double u) {
    double part1 = pow((pow(xmax, 1 - alpha) - pow(xmin, 1 - alpha)) * u + pow(xmin, 1 - alpha), 1 / (1 - alpha));
    return std::max(xmin, std::min(xmax, part1));
}

void writeHypergraphToFile(const std::string &graph_path, const std::vector<std::vector<std::size_t>> &edges) {
    std::ofstream output(graph_path);
    if (!output.is_open()) {
        std::cerr << "Failed to open file: " << graph_path << std::endl;
        return;
    }
    std::cout << "Writing graph to " << graph_path << std::endl;
    std::size_t num_edges = 0;
    for (const auto &edge: edges) {
        if (edge.empty() || edge.size() == 1) {
            continue;
        }
        for (std::size_t node: edge) {
            output << node << ",";
        }
        ++num_edges;
        output << "\n";
    }
    output.close();
    std::cout << "num_edges: " << num_edges << std::endl;
    std::cout << "Graph written to " << graph_path << std::endl;
}


void generateHypergraph_random(
        std::size_t numNodes,
        std::size_t numEdges,
        std::size_t maxEdgeSize,
        const std::string &graph_path) {


    std::cout << "Generating graph..." << std::endl;

    std::vector<std::vector<std::size_t>> edges(numEdges);
    std::random_device rd;
    std::mt19937 gen(rd());

    std::vector<std::size_t> edge_size;

    std::mt19937 node_deg_gen(rd());
    for (std::size_t i = 0; i < numEdges; ++i) {
        std::weibull_distribution<> edge_size_dis(1.2, 18);
        edge_size.push_back(static_cast<std::size_t>(edge_size_dis(node_deg_gen)));
    }

    std::uniform_real_distribution<> dis(0, 1);
    std::vector<std::size_t> not_full_edge;
    not_full_edge.reserve(numEdges);
    for (std::size_t i = 0; i < numEdges; ++i) {
        not_full_edge.push_back(i);
    }
    for (std::size_t node = 1; node <= numNodes; ++node) {
        if (node % (numNodes / 100) == 0) {
            std::cout << "Progres: " << node << "/" << numNodes << std::endl;
        }
        auto max_degree =
                static_cast<std::size_t>(generatePowerLawRandom(1.1, 1, static_cast<double>(maxEdgeSize), dis(gen)));
        if (max_degree <= 0) {
            max_degree = 1;
        }
        auto current_degree = static_cast<std::size_t>(0);
        std::vector<std::size_t> unselected_edges = std::vector<std::size_t>();
        for (std::size_t i = 0; i < numEdges; ++i) {
            if (edges[i].size() < edge_size[i]) {
                unselected_edges.push_back(i);
            }
        }
        while (current_degree <= max_degree && !unselected_edges.empty()) {
            std::uniform_int_distribution<> edge_dis(0, static_cast<int>(unselected_edges.size() - 1));
            auto edge_index = static_cast<std::size_t>(edge_dis(gen));
            edges[unselected_edges[edge_index]].push_back(node);
            current_degree++;
            unselected_edges.erase(unselected_edges.begin() + static_cast<long>(edge_index));
        }
    }

    writeHypergraphToFile(graph_path, edges);
}

void generateHypergraph_deg_PowerLaw(
        std::size_t numNodes,
        std::size_t numEdges,
        const std::string &graph_path) {


    std::cout << "generating graph with PowerLaw deg distribution, numNodes: " << numNodes << " numEdges: "
              << numEdges << std::endl;
    std::cout << " writing to " << graph_path << std::endl;

    std::vector<std::vector<std::size_t>> edges(numEdges);
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<> dis(0, 1);
    for (std::size_t node = 0; node < numNodes; ++node) {
        if (node % (numNodes / 100) == 0) {
            std::cout << "Progress: " << node << "/" << numNodes << std::endl;
        }
        auto max_degree = static_cast<std::size_t>(generatePowerLawRandom(1.8, 1, 5000, dis(gen)));
        if (max_degree <= 0) {
            max_degree = 1;
        }
        auto current_degree = static_cast<std::size_t>(0);
        std::unordered_set<std::size_t> chosen_edges;
        while (current_degree <= max_degree) {
//            std::uniform_int_distribution<> edge_dis(0, static_cast<int>(numEdges) - 1);
//            auto edge = static_cast<std::size_t>(edge_dis(gen));
            auto edge = static_cast<std::size_t>(generatePowerLawRandom(1.1, 1, static_cast<int>(numEdges) - 1,
                                                                        dis(gen)));
            // Ensure unique edges are selected for   each node
            if (chosen_edges.find(edge) != chosen_edges.end()) {
                continue; // Edge already chosen, pick another
            }

            chosen_edges.insert(edge);
            edges[edge].push_back(node);
            current_degree++;
        }
    }

    writeHypergraphToFile(graph_path, edges);
}

void generateHypergraph_deg_WeiBull(
        std::size_t numNodes,
        std::size_t numEdges,
        const std::string &graph_path) {


    std::cout << "generating graph with WeiBull deg distribution, numNodes: " << numNodes << " numEdges: "
              << numEdges << std::endl;
    std::cout << "writing to " << graph_path << std::endl;

    std::vector<std::vector<std::size_t>> edges(numEdges);
    std::random_device rd;
    std::mt19937 gen(rd());


    std::mt19937 node_deg_gen(rd());
    std::weibull_distribution<> node_degree_dis(1.2, 18);
    for (std::size_t node = 0; node < numNodes; ++node) {
        if (node % (numNodes / 100) == 0) {
            std::cout << "Progress: " << node << "/" << numNodes << std::endl;
        }
        auto max_degree = static_cast<std::size_t>(node_degree_dis(node_deg_gen));
        if (max_degree <= 0) {
            max_degree = 1;
        }
        auto current_degree = static_cast<std::size_t>(0);
        std::unordered_set<std::size_t> chosen_edges;
        while (current_degree <= max_degree) {
            std::uniform_int_distribution<> edge_dis(0, static_cast<int>(numEdges) - 1);
            auto edge = static_cast<std::size_t>(edge_dis(gen));

            // Ensure unique edges are selected for   each node
            if (chosen_edges.find(edge) != chosen_edges.end()) {
                continue; // Edge already chosen, pick another
            }

            chosen_edges.insert(edge);
            edges[edge].push_back(node);
            current_degree++;
        }
    }

    writeHypergraphToFile(graph_path, edges);
}

void generateHypergraph_nbr_PowerLaw(
        std::size_t numNodes,
        std::size_t numEdges,
        const std::string &graph_path) {
    std::cout << "generating graph with PowerLaw nbr distribution, numNodes: " << numNodes << " numEdges: "
              << numEdges << std::endl;
    std::cout << " writing to " << graph_path << std::endl;

    std::vector<std::vector<std::size_t>> edges(numEdges);
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<> dis(0, 1);
    std::vector<std::size_t> un_chosen_edges_raw;
    un_chosen_edges_raw.reserve(numEdges);
    for (std::size_t i = 0; i < numEdges; ++i) {
        un_chosen_edges_raw.push_back(i);
    }
    auto time_start = std::chrono::high_resolution_clock::now();
    std::size_t nodesPer01Percent = std::max(static_cast<std::size_t>(10), static_cast<std::size_t>(
            static_cast<double>(numNodes) * 0.00001));
    std::cout << "\033[?25l";
    for (std::size_t node = 1; node < numNodes; ++node) {

        if (node % nodesPer01Percent == 0) {
            double percentage = static_cast<double>(node) / static_cast<double>(numNodes) * 100;
            std::cout << "\r" << "Progress: " << std::fixed << std::setprecision(2) << percentage << "%";

            auto step_time = std::chrono::high_resolution_clock::now();
            auto duration = std::chrono::duration_cast<std::chrono::microseconds>(step_time - time_start).count();
            if (percentage > 0) {
                auto estimated_total_time_mic = static_cast<long long>((static_cast<double>(duration) /
                                                                        (percentage / 100)));
                auto estimated_time_remaining_s = static_cast<double>(estimated_total_time_mic - duration) / 1000000;
                std::cout << " | Time remaining: " << estimated_time_remaining_s << "s";
                std::cout << " | Time elapsed: " << static_cast<double>(duration) / 1000000 << "s ";
            }
        }

        auto max_nbr = static_cast<std::size_t>(generatePowerLawRandom(1.4, 1,
                                                                       pow(log2(static_cast<double>(node)), 2.5),
                                                                       dis(gen)));
        max_nbr = std::max(max_nbr, static_cast<std::size_t>(1));


        std::size_t max_nbr_edges = std::min(numEdges, max_nbr);
        std::vector<std::size_t> swap_indexes;
        for (size_t i = 0; i < max_nbr_edges; ++i) {
            auto swap_index = static_cast<std::size_t>(generatePowerLawRandom(1.0000001, 1, static_cast<double>(
                                                                                      un_chosen_edges_raw.size() - 1),
                                                                              dis(gen)));
            std::swap(un_chosen_edges_raw[i], un_chosen_edges_raw[swap_index]);
            swap_indexes.push_back(swap_index);
        }

        std::vector<std::size_t> sub_edge(un_chosen_edges_raw.begin(),
                                          un_chosen_edges_raw.begin() + static_cast<long>(max_nbr_edges));

        auto current_nbr = static_cast<std::size_t>(0);

        for (unsigned long edge_id: sub_edge) {
            edges[edge_id].push_back(node);
            current_nbr += edges[edge_id].size();
            if (current_nbr >= max_nbr) {
                break;
            }
        }

//        // Swap back
        for (size_t i = 0; i < max_nbr_edges; ++i) {
            std::swap(un_chosen_edges_raw[i], un_chosen_edges_raw[swap_indexes[i]]);
        }

    }
    std::cout << "\r" << "Progress: 100.000% |" << std::endl;
    std::cout << "\033[?25h";
    writeHypergraphToFile(graph_path, edges);
}