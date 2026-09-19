# Graph Preparation (showg / pickg)

To get graphs
```
./showg.exe graph10.g6 -e > graph10.txt
```
*********
To pick graphs special graphs

for example, disconnected
```
./pickg.exe -c0   graph10.g6 disconnected_graph10.g6
```
connected
```
./pickg.exe -c0  -v graph10.g6 connected_graph10.g6
```
*********
Output

graph10.txt              -> readable nauty-format graphs, converted from graph10.g6
disconnected_graph10.g6  -> disconnected graphs picked from graph10.g6
connected_graph10.g6     -> connected graphs picked from graph10.g6

These are the tools nauty provides for converting/filtering graph6 (.g6) files before
they're used by canon_convert.
