
# Hypergraph Core Decomposition Source Code

The `HypergraphKCore` tool is designed for analyzing and manipulating hypergraphs through various operations including k-core decomposition, sorting, and generating hypergraphs with specific distributions.

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
2. Navigate to the root directory of the project.
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

## Example Usage

We provide a shell script `main_run.sh` to run the experiments in the paper. The simple dataset `coauth-MAG-History-full.hyp` is provided in the `/dataset` directory.

### License

This project is licensed under the MIT License - see the [LICENSE.txt](License.txt) file for details.
