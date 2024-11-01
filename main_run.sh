datasets=(
"./datasets/coauth-MAG-History-full.hyp"
)

cmake -S . -B ./build -DCMAKE_BUILD_TYPE=Release
cmake --build ./build
for datasets in "${datasets[@]}"
do
      ./build/HypergraphKCore "${datasets}" org

#      You can sort the dataset to achieve better performance, this may effect the performance of filter strategy
      ./build/HypergraphKCore "${datasets}" sortEV desc asc
      echo "------------"
done


# for Figure 4,5,6,8
echo "----------------memory, init time, exec time----------------"
algorithms=(0 1 2 3 4 5)
it=5
for datasets in "${datasets[@]}"
do
    for algo in "${algorithms[@]}"
    do
        sorted_dataset="${datasets}_sorted_by_edge_desc_node_asc.txt"
        /usr/bin/time -v ./build/HypergraphKCore ${sorted_dataset} ${algo} ${it}
        echo "------------"
    done
done



# for Figure 7
echo "----------------get node change in each iteration----------------"
algorithms=(0 1 2)
it=5
for datasets in "${datasets[@]}"
do
    for algo in "${algorithms[@]}"
    do
        sorted_dataset="${datasets}_sorted_by_edge_desc_node_asc.txt"
        ./build/HypergraphKCore ${sorted_dataset} ${algo} ${it} "--info"
        echo "------------"
    done
done

# for Figure 10
echo "----------------Very Edge for Scalability----------------"
veryed=("_E_0.2.txt" "_E_0.4.txt" "_E_0.6.txt" "_E_0.8.txt")
for dataset in "${datasets[@]}"
do
      sorted_dataset="${dataset}_sorted_by_edge_desc_node_asc.txt"

      ./build/HypergraphKCore ${sorted_dataset} "varyE"

      echo "------------"
done

for dataset in "${datasets[@]}"
do
      sorted_dataset="${dataset}_sorted_by_edge_desc_node_asc.txt"

      for very in "${veryed[@]}"
      do
          veryed_dataset="${sorted_dataset}${very}"
          ./build/HypergraphKCore ${veryed_dataset} "org"
          ./build/HypergraphKCore ${veryed_dataset} "sortEV" "desc" "asc"
          mv ${veryed_dataset}_sorted_by_edge_desc_node_asc.txt ${veryed_dataset}
          echo "------------"
      done
done
#
for dataset in "${datasets[@]}"
do
      sorted_dataset="${dataset}_sorted_by_edge_desc_node_asc.txt"
      for very in "${veryed[@]}"
      do
          veryed_dataset="${sorted_dataset}${very}"
          ./build/HypergraphKCore ${veryed_dataset} 0
          echo "------------"
      done
done

# for Figure 11
echo "----------------Parallel----------------"
threads=(1 2 4 8 16 32 64)
it=1
for i in $(seq 1 $it)
do
  echo "Iteration $i"
  for dataset in "${datasets[@]}"
  do
      for thread in "${threads[@]}"
      do
          sorted_dataset="${dataset}_sorted_by_edge_desc_node_asc.txt"
          ./build/HypergraphKCore ${sorted_dataset} 10 ${thread}
          echo "------------"
      done
  done
done

echo "Done!"
