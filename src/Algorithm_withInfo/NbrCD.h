//
// Created by _ on 19/9/2023.
//

//#ifndef HYPERGRAPHKCORE_NBRCD_H
//#define HYPERGRAPHKCORE_NBRCD_H

#include "../Hypergraph.h"

//use for counting the number of neighbors

namespace withInfo {
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
}
//#endif //HYPERGRAPHKCORE_NBRCD_H
