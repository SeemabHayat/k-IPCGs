# Canonical Form Conversion (canon_convert)

To get canonical form and remove isomorphic duplicates
```
./canon_convert.exe graph10.txt 10
```
Arguments, in order:
- graph10.txt -> input file of graphs in nauty format (e.g. output of showg.exe)
- 10          -> number of vertices in the graphs
*********
Requirements
- dreadnaut.exe (from nauty) on PATH
- Headers alongside canon_convert.cpp: graph3.h, debug.h, canon_graph.h
*********
Build
```
g++ -std=c++11 -o canon_convert canon_convert.cpp
```
*********
Output

input_nauty_<N>Graphs.txt -> dreadnaut input generated from the graphs
canon_<N>All_Graph.txt    -> canonical form of each graph

Console output reports total graphs read, duplicates found, and final non-isomorphic count.
