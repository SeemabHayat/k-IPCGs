# 2-IPCG Generation Project

This project has three tools, run in sequence. Each has its own README:

1. Graph Preparation (showg / pickg)  -> README_graph_prep.txt
2. Canonical Form Conversion (canon_convert)  -> README_canon_convert.txt
3. 2-Interval PCG Generator (new_heuristic_2_inter_PCG_gen_1.1 / 1.2)  -> README_pcg_generator.txt

Pipeline:
nauty website (.g6) -> showg/pickg -> canon_convert -> new_heuristic_2_inter_PCG_gen_1.1 / 1.2

See each tool's README for its exact commands, arguments, and output files.

## Project Structure

- `1_g6/`       - Graph preprocessing tools (showg, pickg, dreadnaut)
- `2_canon/`    - Canonical form conversion (canon_convert)
- `3_k-IPCG/`   - 2-Interval PCG generator (new_heuristic_2_inter_PCG_gen)
- `4_verifier/` - Verification scripts

## Experimental Results

Due to GitHub's file size limits, the large experimental result files
are hosted on Google Drive:
Experimental results of 2-IPCG Generator: https://drive.google.com/drive/folders/1-0-OFb2_XsJduOD2WWp23UMxNoDPMBzJ?usp=drive_link


## How to Run

- Compile C++ code with Visual Studio 14.0 (2015)
- Run Python scripts with: python batch_verifier_kIPCG_scriptfile.py
