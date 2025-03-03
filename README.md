# Hypergraph Core Decomposition Source Code

The `HypergraphKCore` repo provides tools for our k-core decomposition implementation for hypergraphs as in our **SIGMOD 2025** paper **Accelerating Core Decomposition in Billion-Scale Hypergraphs**, together with other useful tools for sorting and generating hypergraphs with specific distributions.

## Getting Started

These instructions will get you a copy of the project up and running on your local machine for development and testing purposes.

### Prerequisites

Before building the `HypergraphKCore` tool, ensure you have CMake installed on your system. You can download it from [https://cmake.org/download/](https://cmake.org/download/) or install it using a package manager for your operating system.

If you are using Max OS X, you need to install the `llvm` to allow the `omp` library to work properly. You can install it using Homebrew:

```bash
brew install llvm
```

#### Suggested IDE
We recommend using CLion as the IDE for this project. You can download it from [https://www.jetbrains.com/clion/download/](https://www.jetbrains.com/clion/download/).

### Building the Project

To build the project, follow these steps:

1. Clone the repository to your local machine.
```bash
git clone git@github.com:unswdb/hyper-k-core.git
```

2. Navigate to the root directory of the project.
```bash
cd hyper-k-core
```
3. Create a new directory for the build:

```bash
mkdir build
```

4. Run CMake to configure the project:

```bash
cmake -S . -B ./build -DCMAKE_BUILD_TYPE=Release
```

5. Compile the project:

```bash
cmake --build ./build
```

This will create the `HypergraphKCore` executable within the `build` directory.

## Usage

To use the `HypergraphKCore` tool, run the executable from the command line by specifying a dataset and the desired operation. The basic syntax is:

```bash
./build/HypergraphKCore <datasets> [options]
```

### Options

- `info` : Get graph information.
- `detail` : Get graph details and write to `<dataset>.{_nbrs,_edges,_degree}`.
- `org` : Organize nodes, renumbering them from 1 to N, remove duplicates nodes in the same hyperedge, remove empty hyperedges or single-node hyperedges, <span style="color: red;">**the output will overwrite the original dataset**</span>.
- `vary` : Vary nodes, generate 20%, 40%, 60%, 80%, and 100% of the original hypergraph, the output will be written to `<dataset>_{20,40,60,80,100}.hyp`.
- `sort <degree|nbr> <asc|desc>` : Sort nodes by degree or neighbors in ascending or descending order by renumbering nodes.
- `gen (P | W) (nbr | deg) <numNodes> <numEdges>` : Generate an organized hypergraph with a PowerLaw (P) or Weibull (W) distribution.
  - `P` : PowerLaw distribution.
  - `W` : Weibull distribution.
  - `nbr` : distribution neighbors.
  - `deg` : distribution degree.
  - `<numNodes>` : Number of nodes.
  - `<numEdges>` : Number of edges.
- `(0-5)` : Run single thread k-core decomposition with the specified algorithm.
  - 0 - HyperCD* <span style="color: green;">**_The Most efficient algorithm._**</span>
  - 1 - HyperCD+ with RUC
  - 2 - HyperCD+ with LB
  - 3 - HyperCD+
  - 4 - HyperCD
  - 5 - HyperCD++
- `10 <numThreads>` : Run multi-thread k-core decomposition with the specified number of threads.

### <span style="color: red;">Important:</span>
Before running any other operation, run `./build/HypergraphKCore <dataset> org` to organize the hypergraph.

### Examples

To get the graph information of `dataset.hyp`:

```bash
./build/HypergraphKCore datasets.hyp info
```

To run k-core decomposition with the HyperCD* algorithm:

```bash
./build/HypergraphKCore datasets.hyp 0
```

To generate a graph with a PowerLaw distribution of neighbor size, 1000 nodes, and 5000 edges:

```bash
./build/HypergraphKCore output.hyp gen P nbr 1000 5000
```

For further details on options and their usage, run the tool with the `--help` option:

```bash
./build/HypergraphKCore --help
```

### Dataset File Format

The dataset file expected by the `HypergraphKCore` tool should be structured where each line represents a hyperedge. Each line contains a list of node identifiers, separated by commas, indicating that these nodes are part of the same hyperedge. For example, a line like `1,2,3,4,5` signifies that 
 a hyperedge containing nodes 1, 2, 3, 4, and 5. This format allows the representation of complex connectivity patterns within the hypergraph, enabling the tool to accurately perform its analyses and manipulations based on the input dataset.

### Example Usage

We provide a shell script `main_run.sh` to run the experiments in the paper. The simple dataset `coauth-MAG-History-full.hyp` is provided in the `/dataset` directory.

## License

If you use this work, please cite:

```bibtex
@article{10.1145/3709656,
  author = {Zhang, Wenqian and Yang, Zhengyi and Wen, Dong and Li, Wentao and Zhang, Wenjie and Lin, Xuemin},
  title = {Accelerating Core Decomposition in Billion-Scale Hypergraphs},
  year = {2025},
  issue_date = {February 2025},
  publisher = {Association for Computing Machinery},
  address = {New York, NY, USA},
  volume = {3},
  number = {1},
  url = {https://doi.org/10.1145/3709656},
  doi = {10.1145/3709656},
  abstract = {Hypergraphs provide a versatile framework for modeling complex relationships beyond pairwise interactions, finding applications in various domains. k-core decomposition is a fundamental task in hypergraph analysis that decomposes hypergraphs into cohesive substructures. Existing studies capture the cohesion in hypergraphs based on the vertex neighborhood size. However, such decomposition poses unique challenges, including the efficiency of core value updates, redundant computation, and high memory consumption. We observe that the state-of-the-art algorithms do not fully address the above challenges and are unable to scale to large hypergraphs. In this paper, we propose an efficient approach for hypergraph k-core decomposition. Novel concepts and strategies are developed to compute the core value of each vertex and reduce redundant computation of vertices. Experimental results on real-world and synthetic hypergraphs demonstrate that our approach significantly outperforms the state-of-the-art algorithm by 7 times on average while reducing the average memory usage by 36 times. Moreover, while existing algorithms fail on tens of millions hyperedges, our approach efficiently handles billion-scale hypergraphs in only a single thread.},
  journal = {Proc. ACM Manag. Data},
  month = feb,
  articleno = {6},
  numpages = {27},
  keywords = {core decomposition, hypergraph, k-core}
}
```

This project is licensed under the MIT License - see the [LICENSE.txt](License.txt) file for details.

