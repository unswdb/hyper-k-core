#include <iostream>
#include <fstream>
#include "Hypergraph.h"
#include <thread>
#include <omp.h>
#include <iomanip>
#include "Algorithm/NbrCD.h"
#include "Algorithm_withInfo/NbrCD.h"
#include "graphTool/graphTool.h"


std::vector<void (*)(Hypergraph &)> fun_list = {
        nbrCoreDecomposition,
        nbrCoreDecomposition_onlyChanged,
        nbrCoreDecomposition_onlyLowerBound,
        nbrCoreDecomposition_no_filter,
        baseNbrCoreDecomposition_withNbrs,
        fullNbrCoreDecomposition,
        nbrCoreDecomposition_withNbrs,
};

std::vector<void (*)(Hypergraph &)> info_fun_list = {
        withInfo::nbrCoreDecomposition,
        withInfo::nbrCoreDecomposition_onlyChanged,
        withInfo::nbrCoreDecomposition_onlyLowerBound,
        withInfo::nbrCoreDecomposition_no_filter,
        withInfo::baseNbrCoreDecomposition_withNbrs,
        withInfo::fullNbrCoreDecomposition,
        withInfo::nbrCoreDecomposition_withNbrs,
};

std::vector<std::string> alg_name = {
        "HyperCD*",
        "HyperCD+ only with RUC",
        "HyperCD+ only with LowerBound",
        "HyperCD+ only with no filter",
        "HyperCD",
        "HyperCD++",
        "HyperCD with RUC",
};

void showHelpMessage() {
    std::vector<std::string> rainbow_colors = {
            "\033[31m", // Red
            "\033[33m", // Yellow
            "\033[32m", // Green
            "\033[36m", // Cyan
            "\033[34m", // Blue
            "\033[35m", // Magenta
            "\033[37m"  // White
    };

    std::cout << "Usage: ./HypergraphKCore <datasets> <command> [options]\n\n";
    std::cout << "Commands:\n";
    std::cout << rainbow_colors[0] << "  info            Print graph info\n";
    std::cout << rainbow_colors[1]
              << "  detail          Get graph details,  write result to <datasets>.{_nbrs,_edges,_degree}.\n";
    std::cout << rainbow_colors[2] << "  org             Organize nodes\n";
    std::cout << rainbow_colors[3] << "  varyV           Vary nodes, random remove 20%, 40%, 60%, 80% nodes\n";
    std::cout << rainbow_colors[4] << "  varyE           Vary edges, random remove 20%, 40%, 60%, 80% hyperedges\n";
    std::cout << rainbow_colors[5] << "  sortN           Reindex nodes by order, loading datasets from <datasets>\n";
    std::cout << "                  Options:\n";
    std::cout << "                    degree | nbr    - Sort criteria\n";
    std::cout << "                    asc | desc      - Order type\n";
    std::cout << "                  Example: ./HypergraphKCore <datasets> sortN degree asc\n";
    std::cout << rainbow_colors[6] << "  sortE           Sort edges, loading datasets from <datasets>\n";
    std::cout << "                  Options:\n";
    std::cout << "                    asc | desc      - Order type\n";
    std::cout << "                  Example: ./HypergraphKCore <datasets> sortE asc\n";
    std::cout << rainbow_colors[0] << "  sortEV | sortVE Sort by edge size and node, loading datasets from <datasets>\n";
    std::cout << "                  Options:\n";
    std::cout << "                    <edge order> <node order>\n";
    std::cout << "                  Example: ./HypergraphKCore <datasets> sortEV asc desc\n";
    std::cout << rainbow_colors[1] << "  gen             Generate hypergraph with given distribution\n";
    std::cout << "                  Options:\n";
    std::cout << "                    P | W           - Distribution type (PowerLaw or WeiBull)\n";
    std::cout << "                    nbr | deg       - Generation method\n";
    std::cout << "                    <numNodes> <numEdges>\n";
    std::cout << "                  Example: ./HypergraphKCore <datasets> gen P nbr 1000 5000\n";
    std::cout << rainbow_colors[2] << "  genR            Generate hypergraph with random distribution\n";
    std::cout << "                  Options:\n";
    std::cout << "                    <numNodes> <numEdges> <maxEdgeSize>\n";
    std::cout << "                  Example: ./HypergraphKCore <datasets> genR 1000 5000 10\n";
    std::cout << rainbow_colors[3] << "  --help          Print this help message\n";


    std::string reset = "\033[0m";
    std::cout << "\n\033[1;34mAlgorithms:\033[0m\n";
    for (std::size_t i = 0; i < alg_name.size(); ++i) {
        std::cout << rainbow_colors[i % rainbow_colors.size()] << "  " << i << "                             - "
                  << alg_name[i] << reset << std::endl;
    }

}


int main(int argc, char *argv[]) {

    if (argc < 2 || strcmp(argv[1], "--help") == 0) {
        showHelpMessage();
        return 0;
    }

    Hypergraph hg;
    if (argc == 3 && strcmp(argv[2], "info") == 0) {
        std::cout << "get graph info, loading datasets from" << argv[1] << std::endl;
        hg.loadFromFiles(argv[1]);
        printGraphInfo(hg);
        return 0;
    }

    if (argc == 3 && strcmp(argv[2], "detail") == 0) {
        std::cout << "get graph detail, loading datasets from" << argv[1] << std::endl;
        hg.loadFromFiles(argv[1]);
        getGraphDetiles(hg, argv[1]);
        return 0;
    }

    if (argc == 3 && strcmp(argv[2], "org") == 0) {
        std::cout << "Organize nodes, loading datasets from" << argv[1] << std::endl;
        organize_nodes(argv[1]);
        return 0;
    }

    if (argc == 3 && (strcmp(argv[2], "varyV") == 0)) {
        std::cout << "vary nodes, loading datasets from" << argv[1] << std::endl;
        vary_nodes(argv[1]);
        return 0;
    }

    if (argc == 3 && (strcmp(argv[2], "varyE") == 0)) {
        std::cout << "vary edge, loading datasets from" << argv[1] << std::endl;
        vary_edges(argv[1]);
        return 0;
    }

    if (argc == 5 && strcmp(argv[2], "sortN") == 0) {
        orderType order = cover_to_orderType(argv[4]);
        std::cout << "sort nodes, loading datasets from" << argv[1] << std::endl;

        if (strcmp(argv[3], "degree") == 0) {
            sortNodes(argv[1], DEGREE, order);
        } else if (strcmp(argv[3], "nbr") == 0) {
            sortNodes(argv[1], NBR, order);
        } else {
            std::cout << "Usage: ./HypergraphKCore datasets.hyp sortN (degree | nbr) (asc | desc)" << std::endl;
        }
        return 0;
    }

    if (argc == 4 && strcmp(argv[2], "sortE") == 0) {
        orderType order = cover_to_orderType(argv[3]);
        std::cout << "sort nodes, loading datasets from" << argv[1] << std::endl;
        sortEdges(argv[1], order);
        return 0;
    }

    if (argc == 5 && (strcmp(argv[2], "sortEV") == 0 || strcmp(argv[2], "sortVE") == 0)) {
        orderType edge = cover_to_orderType(argv[3]);
        orderType node = cover_to_orderType(argv[4]);

        std::cout << "sort by edge size with " << orderType_to_string(edge) << " and node with "
                  << orderType_to_string(node) << std::endl;

        sortEdgeNodes(argv[1], node, edge);
        return 0;
    }

    if (strcmp(argv[2], "gen") == 0) {
        if (argc < 7) {
            std::cout << "Usage: ./HypergraphKCore <datasets> gen (P | W) (nbr | deg) <numNodes> <numEdges>"
                      << std::endl;
            return 0;
        }
        if (strcmp(argv[3], "P") != 0 && strcmp(argv[3], "W") != 0 && strcmp(argv[4], "nbr") != 0 &&
            strcmp(argv[4], "deg") != 0) {
            std::cout << "Usage: ./HypergraphKCore <datasets> gen (P | W) (nbr | deg) <numNodes> <numEdges>"
                      << std::endl;
            return 0;
        }
        bool isPowerLaw = strcmp(argv[3], "P") == 0;
        bool isNbr = strcmp(argv[4], "nbr") == 0;
        if (isPowerLaw && isNbr) {
            generateHypergraph_nbr_PowerLaw(static_cast<std::size_t>(std::stoi(argv[5])),
                                            static_cast<std::size_t>(std::stoi(argv[6])),
                                            argv[1]);
        } else if (isPowerLaw && !isNbr) {
            generateHypergraph_deg_PowerLaw(static_cast<std::size_t>(std::stoi(argv[5])),
                                            static_cast<std::size_t>(std::stoi(argv[6])),
                                            argv[1]);
        } else if (!isPowerLaw && isNbr) {
            std::cout << "generating graph with WeiBull nbr distribution, numNodes: " << argv[5] << " numEdges: "
                      << argv[6] << std::endl;
//            std::cout << "writing to " << argv[1] << std::endl;
//            generateHypergraph_nbr_WeiBull(static_cast<std::size_t>(std::stoi(argv[5])),
//                                           static_cast<std::size_t>(std::stoi(argv[6])),
//                                           argv[1]);
        } else {
            generateHypergraph_deg_WeiBull(static_cast<std::size_t>(std::stoi(argv[5])),
                                           static_cast<std::size_t>(std::stoi(argv[6])),
                                           argv[1]);
        }

        return 0;
    }


    if (strcmp(argv[2], "genR") == 0) {

        if (argc < 6) {
            std::cout << "Usage: ./HypergraphKCore <datasets> genR <numNodes> <numEdges> <maxEdgeSize>"
                      << std::endl;
            return 0;
        }
        std::cout << "generating graph with random distribution, numNodes: " << argv[3] << " numEdges: " << argv[4]
                  << " maxEdgeSize: " << argv[5];
        generateHypergraph_random(static_cast<std::size_t>(std::stoi(argv[3])),
                                  static_cast<std::size_t>(std::stoi(argv[4])),
                                  static_cast<std::size_t>(std::stoi(argv[5])),
                                  argv[1]);
        return 0;
    }

    std::string dataName;

    if (argc < 3) {
        std::cout << "Usage: ./HypergraphKCore <datasets> (0-" << fun_list.size() << ")" << std::endl;
        return 0;
    } else {
        try {
            int alg = std::stoi(argv[2]);
            if (alg == 10) {
                if (argc < 4) {
                    std::cout << "Usage: ./HypergraphKCore <datasets> 10 <numThreads>" << std::endl;
                    return 0;
                } else {
                    std::stoi(argv[3]);
                }
            } else if (alg < 0 || static_cast<std::size_t>(alg) >= fun_list.size()) {
                std::cout << "Invalid algorithm index, should be in range 0-" << fun_list.size()-1 << std::endl;
                return 0;
            }
        } catch (std::exception &e) {
            std::cout << "Invalid Command: ";
            for (int i = 0; i < argc; ++i) {
                std::cout << argv[i] << " ";
            }
            return 0;
        }
    }


    std::size_t alg = static_cast<std::size_t>(std::stoi(argv[2]));
    std::cout << "loading datasets from " << argv[1] << std::endl;
    hg.loadFromFiles(argv[1]);

    bool withInfo = strcmp(argv[argc - 1], "--info") == 0;

    if (alg < fun_list.size()) {
        int iter_time = 1;
        if (withInfo){
            iter_time = 1;
        }
        else {
            iter_time = argc > 3 ? std::stoi(argv[3]) : 1;
        }
        for (int i = 0; i < iter_time; ++i) {
            std::cout << "loading datasets from " << argv[1] << std::endl;
            if (withInfo) {
                info_fun_list[alg](hg);
            } else {
                fun_list[alg](hg);
            }
            if (!withInfo){
                hg.resetCore();
            }
        }
    } else {
        int iter_time = 1;
        if (!withInfo){
            iter_time = argc > 5 ? std::stoi(argv[5]) : 1;
        }
        int computeThreshold = 3000;
        if (argc > 4) {
            computeThreshold = std::stoi(argv[4]);
        }
        for (int i = 0; i < iter_time; ++i) {
            if (withInfo) {
                withInfo::ParNbrCoreDecomposition(hg, std::stoi(argv[3]), computeThreshold);
            } else {
                ParNbrCoreDecomposition(hg, std::stoi(argv[3]), computeThreshold);
            }
            if (!withInfo){
                hg.resetCore();
            }
        }
    }

//    print
//  edges_edge_simplices[nodes_edge_nvertx_offset[v] : nodes_edge_nvertx_offset[v+1]] is all edge id that contains node v
//    std::vector<std::size_t> nodes_edge_nvertx_offset;
//    std::vector<std::size_t> node_edge_simplices;

//    edge_simplices[edge_nvertx_offset[v] : edge_nvertx_offset[v+1]] is all node id that is connected to edge v
//    std::vector<std::size_t> edge_node_nvertx_offset;
//    std::vector<std::size_t> edge_node_simplices;
//    for (std::size_t i : hg.nodes_edge_nvertx_offset){
//        std::cout << i << ", ";
//    }
//    std::cout << std::endl;
//    for (std::size_t i : hg.node_edge_simplices){
//        std::cout << i << ", ";
//    }
//    std::cout << std::endl;
//    for (std::size_t i : hg.edge_node_nvertx_offset){
//        std::cout << i << ", ";
//    }
//    std::cout << std::endl;
//    for (std::size_t i : hg.edge_node_simplices){
//        std::cout << i << ", ";
//    }
//    std::cout << std::endl;


    if (withInfo){
        dataName = argv[1];
        std::string file = dataName + "_output_" + std::to_string(alg) + ".txt";

        std::ofstream outPut(file);
        std::cout << "writing to " << file << std::endl;
        for (std::size_t i = 1; i < hg.getGraphNodeSize(); ++i) {
            outPut << i << "," << hg.nodes_core.at(i) << std::endl;
        }
        outPut.close();
    }
    std::cout << "done" << std::endl;
    return 0;
}