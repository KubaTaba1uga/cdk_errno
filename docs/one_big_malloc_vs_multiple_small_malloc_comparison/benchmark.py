#!/usr/bin/env python3
import argparse
import re
import statistics
import subprocess

"""
Single-threaded Error-Allocation Benchmark
Compares performance of static vs dynamic error strategies, with and without backtrace capture.
Benchmarks:
  - test_single               (static, no backtrace)
  - test_multiple_method      (dynamic, no backtrace)
  - test_single_bt            (static, with backtrace)
  - test_multiple_method_bt   (dynamic, with backtrace)
"""


def run_and_time(cmd):
    result = subprocess.run(
        cmd, stdout=subprocess.PIPE, stderr=subprocess.PIPE, text=True
    )
    out = result.stdout + result.stderr
    for line in out.splitlines():
        if "Time elapsed" in line:
            m = re.search(r"([0-9]+\.?[0-9]*)\s*ms", line)
            if m:
                return float(m.group(1))
    raise RuntimeError(f"Could not parse time from output of {' '.join(cmd)}")


def benchmark_group(name, tests, single_bin, multiple_bin, runs):
    print(f"\n## {name}\n")
    headers = ["Scenario", "Single avg (ms)", "Multiple avg (ms)", "Δ time %"]

    max_label_len = max(len(headers[0]), *(len(t["label"]) for t in tests))
    col_w = {
        "single": max(len(headers[1]), 14),
        "multiple": max(len(headers[2]), 15),
        "delta": max(len(headers[3]), 9),
    }

    header_fmt = (
        f"| {{:<{max_label_len}}} |"
        f" {{:>{col_w['single']}}} |"
        f" {{:>{col_w['multiple']}}} |"
        f" {{:>{col_w['delta']}}} |"
    )
    print(header_fmt.format(*headers))
    print(
        f"| {'-' * max_label_len} |"
        f" {'-' * col_w['single']} |"
        f" {'-' * col_w['multiple']} |"
        f" {'-' * col_w['delta']} |"
    )

    row_fmt = (
        f"| {{:<{max_label_len}}} |"
        f" {{:>{col_w['single']}.2f}} |"
        f" {{:>{col_w['multiple']}.2f}} |"
        f" {{:>{col_w['delta']}.1f}}% |"
    )
    for t in tests:
        label = t["label"]
        args = [f"--max={t['max']}", f"--batch={t['batch']}"]

        s_times = [run_and_time([single_bin] + args) for _ in range(runs)]
        avg_s = statistics.mean(s_times)

        m_times = [run_and_time([multiple_bin] + args) for _ in range(runs)]
        avg_m = statistics.mean(m_times)

        delta = (avg_m - avg_s) / avg_s * 100
        print(row_fmt.format(label, avg_s, avg_m, delta))


if __name__ == "__main__":
    p = argparse.ArgumentParser(
        description="Error-Allocation Benchmark for c_minilib (static vs dynamic, with/without backtrace)"
    )
    p.add_argument("--runs", type=int, default=30, help="Repetitions per scenario")
    p.add_argument(
        "--single", default="./build/test_single_method", help="Static binary (no BT)"
    )
    p.add_argument(
        "--multiple",
        default="./build/test_multiple_method",
        help="Dynamic binary (no BT)",
    )
    p.add_argument(
        "--single_bt",
        default="./build/test_single_method_with_backtrace",
        help="Static binary (with BT)",
    )
    p.add_argument(
        "--multiple_bt",
        default="./build/test_multiple_method_with_backtrace",
        help="Dynamic binary (with BT)",
    )
    args = p.parse_args()

    tests = [
        {"label": "Cold-start / immediate free", "max": 10000, "batch": 1},
        {"label": "Tiny burst (live=4)", "max": 10000, "batch": 4},
        {"label": "Occasional cascade (live=8)", "max": 10000, "batch": 8},
    ]

    benchmark_group("No Backtrace", tests, args.single, args.multiple, args.runs)
    benchmark_group(
        "With Backtrace", tests, args.single_bt, args.multiple_bt, args.runs
    )
