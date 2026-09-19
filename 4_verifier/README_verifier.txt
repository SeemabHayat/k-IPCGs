# Batch Independent Verifier (verify_pcg.py)

Author: Seemab Hayat

Independently checks PCG_Gen output files (produced by
new_heuristic_2_inter_PCG_gen_1_2) to confirm each result is a genuine 2-IPCG:
the witness tree is a valid binary tree, the two intervals don't truly
overlap, and the tree distances reproduce the target graph's edges exactly.

To run
```
python verify_pcg.py
```
By default it scans every .txt file in the same folder the script is run
from (set INPUT_FOLDER at the top of the script to point elsewhere).
Files already ending in _results.txt are skipped, so re-running the
script won't try to verify its own output.
*********
Requirements
- Python 3
- No external packages (uses only os, glob, re, collections)
*********
What it checks, per graph entry

1. Tree validity   -> every internal vertex has degree 3, every leaf has degree 1
2. Interval check  -> the two intervals must be disjoint, or only touch at a
                      single shared endpoint (touching intervals still count
                      as a valid PCG, since no integer distance can land
                      exactly on the shared boundary)
3. Edge check      -> for every pair of leaves, the tree distance is
                      recomputed and checked against the intervals; the
                      resulting edge set must exactly match the target
                      graph's edges (no missing or extra edges)

A graph passes only if all three checks pass.
*********