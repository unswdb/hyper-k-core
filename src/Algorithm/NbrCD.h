//
// Created by _ on 19/9/2023.
//

#ifndef HYPERGRAPHKCORE_NBRCD_H
#define HYPERGRAPHKCORE_NBRCD_H

#include "../Hypergraph.h"

////use for counting the number of neighbors
//struct Counter {
//    struct value_type {
//        template<typename T>
//        value_type(const T &) {}
//    };
//
//    void push_back(const value_type &) { ++count; }
//
//    size_t count = 0;
//};


void nbrCoreDecomposition(Hypergraph &graph);

void nbrCoreDecomposition_onlyLowerBound(Hypergraph &graph);

void nbrCoreDecomposition_onlyChanged(Hypergraph &graph);

void nbrCoreDecomposition_no_filter(Hypergraph &graph);

void nbrCoreDecomposition_withNbrs(Hypergraph &graph);

void baseNbrCoreDecomposition(Hypergraph &graph);

void baseNbrCoreDecomposition_withNbrs(Hypergraph &graph);

void fullNbrCoreDecomposition(Hypergraph &graph);

void fullNbrCoreDecomposition_withNbrs(Hypergraph &graph);

void ParNbrCoreDecomposition(Hypergraph &graph, int numThreads, int computeThreshold = 3000);

void find_best_threshold(Hypergraph &graph, int numThreads);

#endif //HYPERGRAPHKCORE_NBRCD_H
