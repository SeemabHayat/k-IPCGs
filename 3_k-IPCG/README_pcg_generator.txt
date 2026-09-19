# 2-Interval PCG Generator (new_heuristic_2_inter_PCG_gen_1_2)

To generate 2-interval PCGs (using trees, candidate graphs, and their canonical forms)
```
./new_heuristic_2_inter_PCG_gen_1_2.exe treeFile.txt graphFile.txt canon_repFile.txt 10 numTrees timeLimit
```
Arguments, in order:
- treeFile.txt      -> file containing all trees to test
- graphFile.txt     -> candidate graphs (e.g. 10-vertex graphs after removing supergraphs of NPCGs / non-biconnected graphs)
- canon_repFile.txt -> canonical representations of graphFile.txt (from canon_convert.exe)
- 10                -> number of vertices in the graphs
- numTrees          -> total number of trees in treeFile.txt
- timeLimit         -> time limit (seconds) to search for PCGs before stopping
*********
Requirements
- dreadnaut.exe (from nauty) on PATH
- Headers alongside the .cpp file: graph3.h, debug.h, canon_graph.h
*********
Build
```
g++ -std=c++11 -o new_heuristic_2_inter_PCG_gen_1_2 new_heuristic_2_inter_PCG_gen_1_2.cpp
```
*********
Output

A folder named <numVertices>PCG_Gen_Time<timeLimit> is created, containing:
- <N>PCG_Gen_time<T>.txt        -> PCGs found, with witness tree, edge weights, and pairwise distances
- <N>Further_invest_time<T>.txt -> candidate graphs not yet found as PCGs (need further investigation)
- <N>num_PCG_round<T>.txt       -> number of PCGs found per round

Console output reports round counter, total PCGs generated, graphs left to investigate, and total PCGs found.
