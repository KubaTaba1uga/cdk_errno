#!/usr/bin/env python3
import argparse
import re
import statistics
import subprocess

"""
Single-threaded Error-Allocation Benchmark
Measures and compares the runtime overhead (ms) of two allocation strategies:
  - Static buffer: single malloc per error
  - Dynamic fields: malloc per string field
Outputs a padded Markdown table with average times and percentage delta.
"""


def run_and_time(cmd):
    """
    Runs the given command (list) and parses the elapsed time in ms from its output.
    Returns the elapsed time as float.
    """
    result = subprocess.run(
        cmd, stdout=subprocess.PIPE, stderr=subprocess.PIPE, text=True
    )
    out = result.stdout + result.stderr
    # Look for a line containing "Time elapsed"
    for line in out.splitlines():
        if "Time elapsed" in line:
            # Extract float
            m = re.search(r"([0-9]+\.?[0-9]*)\s*ms", line)
            if m:
                return float(m.group(1))
    raise RuntimeError(f"Could not parse time from output of {' '.join(cmd)}")


def benchmark(tests, static_bin, dynamic_bin, runs):
    # Table headers
    headers = [
        "Scenario",
        "Static avg (ms)",
        "Dynamic avg (ms)",
        "Δ time %",
    ]
    # Determine padding widths
    max_label_len = max(len(headers[0]), *(len(t["label"]) for t in tests))
    static_col_w = max(len(headers[1]), 14)
    dynamic_col_w = max(len(headers[2]), 14)
    delta_col_w = max(len(headers[3]), 9)

    # Print header row with padded columns
    header_fmt = (
        f"| {{:<{max_label_len}}} |"
        f" {{:>{static_col_w}}} |"
        f" {{:>{dynamic_col_w}}} |"
        f" {{:>{delta_col_w}}} |"
    )
    print(header_fmt.format(*headers))

    # Print separator row
    sep = (
        f"| {'-' * max_label_len} |"
        f" {'-' * static_col_w} |"
        f" {'-' * dynamic_col_w} |"
        f" {'-' * delta_col_w} |"
    )
    print(sep)

    # Print each test row
    row_fmt = (
        f"| {{:<{max_label_len}}} |"
        f" {{:>{static_col_w}.2f}} |"
        f" {{:>{dynamic_col_w}.2f}} |"
        f" {{:>{delta_col_w}.1f}}% |"
    )
    for t in tests:
        label = t["label"]
        args = [f"--max={t['max']}", f"--batch={t['batch']}"]

        # Static measurements
        s_times = [run_and_time([static_bin] + args) for _ in range(runs)]
        avg_s = statistics.mean(s_times)

        # Dynamic measurements
        d_times = [run_and_time([dynamic_bin] + args) for _ in range(runs)]
        avg_d = statistics.mean(d_times)

        # Percent delta
        delta = (avg_d - avg_s) / avg_s * 100

        # Print padded row
        print(row_fmt.format(label, avg_s, avg_d, delta))


if __name__ == "__main__":
    p = argparse.ArgumentParser(
        description="Error-Allocation Benchmark (single-threaded, padded table)"
    )
    p.add_argument(
        "--static", default="./build/test_static", help="Path to static-buffer binary"
    )
    p.add_argument(
        "--dynamic",
        default="./build/test_dynamic",
        help="Path to dynamic-fields binary",
    )
    p.add_argument("--runs", type=int, default=100, help="Repetitions per scenario")
    args = p.parse_args()

    # Single-thread burst tests
    tests = [
        {"label": "Cold-start / immediate free", "max": 10000, "batch": 1},
        {"label": "Tiny burst (live=4)", "max": 10000, "batch": 4},
        {"label": "Occasional cascade (live=8)", "max": 10000, "batch": 8},
    ]

    benchmark(tests, args.static, args.dynamic, args.runs)
