"""
Batch Independent Verifier for k-IPCG Output Files
Author: Seemab Hayat
Date: 31-08-2026
"""

import os
import glob
import re
from collections import defaultdict, deque

# Folder to scan for input .txt files. "." means the same folder the script is run from.
INPUT_FOLDER = "."

# Files to skip when scanning (so the script doesn't try to re-verify its own output).
OUTPUT_SUFFIX = "_results.txt"


def build_adjacency(tree_edges):
    adj = defaultdict(list)
    for u, v, w in tree_edges:
        adj[u].append((v, w))
        adj[v].append((u, w))
    return adj


def leaf_distance(adj, source, target):
    visited = {source}
    queue = deque([(source, 0)])
    while queue:
        node, dist = queue.popleft()
        if node == target:
            return dist
        for neighbor, w in adj[node]:
            if neighbor not in visited:
                visited.add(neighbor)
                queue.append((neighbor, dist + w))
    raise ValueError(f"No path found between {source} and {target}")


def in_union_of_intervals(value, intervals):
    return any(lo <= value <= hi for (lo, hi) in intervals)


def check_intervals_disjoint(intervals):
    """Checks pairwise relationship between intervals.

    Returns (status, pair) where status is one of:
      - "disjoint": all intervals are cleanly separated (no shared points)
      - "touching": at least one pair shares exactly one boundary point
                    (e.g. [47.5, 48.5] and [48.5, 49.5]) -- this means the
                    two intervals effectively merge into one continuous
                    range, since no real (integer) distance can land on
                    the shared point.
      - "overlap":  a real overlap with shared width was found -- this is
                    a genuine violation.
    'pair' is the (idx_a, idx_b) of the relevant interval pair, or None.
    """
    touching_pair = None
    for a in range(len(intervals)):
        for b in range(a + 1, len(intervals)):
            lo1, hi1 = intervals[a]
            lo2, hi2 = intervals[b]
            if lo1 < hi2 and lo2 < hi1:  # strict interval-overlap test
                return "overlap", (a, b)
            if hi1 == lo2 or hi2 == lo1:  # touching at a single endpoint
                touching_pair = (a, b)
    if touching_pair is not None:
        return "touching", touching_pair
    return "disjoint", None


def check_binary_tree(leaves, tree_edges):
    """
    Checks that the tree is a valid binary tree as defined in the paper:
    every internal (non-leaf) vertex has degree exactly 3, and every
    leaf vertex has degree exactly 1.
    Returns (True, None) if valid, otherwise (False, list_of_problem_vertices).
    """
    degree = defaultdict(int)
    for u, v, _w in tree_edges:
        degree[u] += 1
        degree[v] += 1

    leaf_set = set(leaves)
    problems = []

    for vertex, deg in degree.items():
        if vertex in leaf_set:
            if deg != 1:
                problems.append((vertex, "leaf", deg))
        else:
            if deg != 3:
                problems.append((vertex, "internal", deg))

    return (len(problems) == 0), problems


def verify_one(leaves, tree_edges, intervals, target_edges):
    # --- Structural check: is the witness a valid binary tree? ---
    tree_ok, tree_problems = check_binary_tree(leaves, tree_edges)

    # --- Interval relationship check ---
    interval_status, interval_pair = check_intervals_disjoint(intervals)
    intervals_ok = (interval_status in ("disjoint", "touching"))

    # --- Distance / exact-match check (as before) ---
    adj = build_adjacency(tree_edges)
    reconstructed_edges = set()
    for i, u in enumerate(leaves):
        for v in leaves[i + 1:]:
            dist = leaf_distance(adj, u, v)
            if in_union_of_intervals(dist, intervals):
                reconstructed_edges.add(frozenset((u, v)))
    target_set = set(frozenset(e) for e in target_edges)
    missing = target_set - reconstructed_edges
    extra = reconstructed_edges - target_set
    edges_ok = (missing == set() and extra == set())

    overall_ok = tree_ok and intervals_ok and edges_ok

    details = {
        "tree_ok": tree_ok,
        "tree_problems": tree_problems,
        "intervals_ok": intervals_ok,
        "interval_status": interval_status,
        "interval_pair": interval_pair,
        "edges_ok": edges_ok,
        "missing": missing,
        "extra": extra,
    }
    return overall_ok, details


def parse_edge_list_line(line):
    nums = [int(x) for x in line.split()]
    return [(nums[i], nums[i + 1]) for i in range(0, len(nums), 2)]


def parse_output_file(filepath):
    with open(filepath, "r") as f:
        content = f.read()

    blocks = re.split(r"\n(?=Graph \d+, Order \d+)", content)
    entries = []

    for block in blocks:
        if not block.strip().startswith("Graph"):
            continue

        header_match = re.search(r"Graph (\d+), Order (\d+)", block)
        if not header_match:
            continue

        graph_id = int(header_match.group(1))
        order = int(header_match.group(2))

        lines = block.strip().splitlines()

        target_edges = []
        if len(lines) >= 3:
            target_edges = parse_edge_list_line(lines[2])

        tree_header_match = re.search(r"Tree \d+, Order (\d+)\n(\d+ \d+)\n(.+)", block)
        tree_struct_edges = []
        if tree_header_match:
            tree_edge_line = tree_header_match.group(3)
            tree_struct_edges = parse_edge_list_line(tree_edge_line)

        interval_match = re.search(
            r"Intervals:\s*\[([\d.]+),\s*([\d.]+)\]\s*;\s*\[([\d.]+),\s*([\d.]+)\]",
            block
        )
        intervals = []
        if interval_match:
            a, b, c, d = map(float, interval_match.groups())
            intervals = [(a, b), (c, d)]

        weight_map = {}
        for m in re.finditer(r"\((\d+),\s*(\d+)\)\s*:\s*(\d+)", block):
            u = int(m.group(1))
            v = int(m.group(2))
            w = int(m.group(3))
            weight_map[frozenset((u, v))] = w

        tree_edges = []
        for (u, v) in tree_struct_edges:
            w = weight_map.get(frozenset((u, v)))
            if w is not None:
                tree_edges.append((u, v, w))

        leaves = list(range(order))

        entries.append({
            "graph_id": graph_id,
            "order": order,
            "target_edges": target_edges,
            "tree_edges": tree_edges,
            "intervals": intervals,
            "leaves": leaves,
        })

    return entries


def verify_file(filepath):
    """
    Runs the full verification process on a single input file.
    Returns (lines, passed, failed, skipped, failed_ids) -- never raises;
    any error while parsing/verifying this file is caught and logged so
    that batch processing can continue with the next file.
    """
    lines = []

    def log(msg=""):
        lines.append(msg)

    passed = 0
    failed = 0
    skipped = 0
    failed_ids = []

    try:
        entries = parse_output_file(filepath)
    except Exception as e:
        log(f"ERROR: could not parse file '{filepath}' - {e}")
        return lines, passed, failed, skipped, failed_ids

    log(f"Parsed {len(entries)} graph entries from {filepath}\n")

    for entry in entries:
        graph_id = entry["graph_id"]
        order = entry["order"]

        if not entry["tree_edges"] or not entry["intervals"]:
            log(f"Graph {graph_id} (order {order}): SKIPPED (could not parse tree/intervals)")
            skipped += 1
            continue

        try:
            ok, details = verify_one(
                entry["leaves"], entry["tree_edges"], entry["intervals"], entry["target_edges"]
            )
        except Exception as e:
            log(f"Graph {graph_id} (order {order}): ERROR during verification - {e}")
            failed += 1
            failed_ids.append(graph_id)
            continue

        if ok:
            passed += 1
            if details["interval_status"] == "touching":
                log(f"Graph {graph_id} (order {order}): PASS "
                    f"[tree: OK, edges: exact match] -- "
                    f"All weights lie in same interval so PCG and hence 2-IPCG")
            else:
                log(f"Graph {graph_id} (order {order}): PASS "
                    f"[tree: OK, intervals: disjoint, edges: exact match]")
        else:
            failed += 1
            failed_ids.append(graph_id)
            msgs = []
            if not details["tree_ok"]:
                msgs.append(f"INVALID TREE (bad-degree vertices: {details['tree_problems']})")
            if not details["intervals_ok"]:
                msgs.append(f"INTERVALS OVERLAP (pair {details['interval_pair']})")
            if not details["edges_ok"]:
                msgs.append(f"EDGE MISMATCH (missing={sorted(tuple(e) for e in details['missing'])}, "
                             f"extra={sorted(tuple(e) for e in details['extra'])})")
            log(f"Graph {graph_id} (order {order}): FAIL -> " + "; ".join(msgs))

    log("\n" + "=" * 60)
    log(f"SUMMARY: {passed} passed, {failed} failed, {skipped} skipped, out of {passed + failed} checked")
    if failed_ids:
        log(f"Failed graph IDs: {failed_ids}")

    return lines, passed, failed, skipped, failed_ids


def main():
    # Find every .txt file in the folder, except ones we previously created
    # as results output (so re-running the script doesn't try to verify
    # its own results files).
    all_txt_files = sorted(glob.glob(os.path.join(INPUT_FOLDER, "*.txt")))
    input_files = [f for f in all_txt_files if not f.endswith(OUTPUT_SUFFIX)]

    if not input_files:
        print(f"No input .txt files found in folder: {INPUT_FOLDER}")
        return

    print(f"Found {len(input_files)} input file(s) to verify in '{INPUT_FOLDER}':")
    for f in input_files:
        print(f"  - {f}")
    print()

    grand_passed = 0
    grand_failed = 0
    grand_skipped = 0

    for filepath in input_files:
        print(f"\n{'#' * 60}")
        print(f"# Processing: {filepath}")
        print(f"{'#' * 60}")

        # verify_file() never raises -- any per-file error is caught inside
        # it and logged, so one bad file never stops the batch.
        lines, passed, failed, skipped, failed_ids = verify_file(filepath)

        for line in lines:
            print(line)

        grand_passed += passed
        grand_failed += failed
        grand_skipped += skipped

        # Build a separate output filename for this input file, e.g.
        # "8PCG_Gen_time10.txt" -> "8PCG_Gen_time10_results.txt"
        base_name = os.path.splitext(os.path.basename(filepath))[0]
        out_name = base_name + OUTPUT_SUFFIX
        out_path = os.path.join(os.path.dirname(filepath) or ".", out_name)

        try:
            with open(out_path, "w") as f:
                f.write("\n".join(lines) + "\n")
            print(f"\n[Results written to {out_path}]")
        except Exception as e:
            print(f"\n[ERROR: could not write results file '{out_path}' - {e}]")

    print(f"\n\n{'=' * 60}")
    print("GRAND TOTAL ACROSS ALL FILES")
    print(f"{'=' * 60}")
    print(f"Files processed: {len(input_files)}")
    print(f"Passed: {grand_passed}, Failed: {grand_failed}, Skipped: {grand_skipped}")


if __name__ == "__main__":
    main()
