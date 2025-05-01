import re
import statistics
import subprocess


def run_and_parse(cmd):
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


def benchmark(tests, static_bin, dynamic_bin, runs=100):
    """
    For each test scenario in tests, runs static and dynamic binaries 'runs' times,
    computes average elapsed time, and prints a Markdown table.
    """
    header = ["Scenario", "Static avg (ms)", "Dynamic avg (ms)", "Δ %"]
    print("| " + " | ".join(header) + " |")
    print("|" + "---|" * (len(header) - 1) + "---|")

    for test in tests:
        label = test["label"]
        max_err = test["max"]
        batch = test["batch"]

        static_cmd = [static_bin, f"--max={max_err}", f"--batch={batch}"]
        dynamic_cmd = [dynamic_bin, f"--max={max_err}", f"--batch={batch}"]

        print(f"# Running {label} (static) {runs} times...")
        static_times = [run_and_parse(static_cmd) for _ in range(runs)]
        avg_static = statistics.mean(static_times)

        print(f"# Running {label} (dynamic) {runs} times...")
        dynamic_times = [run_and_parse(dynamic_cmd) for _ in range(runs)]
        avg_dynamic = statistics.mean(dynamic_times)

        delta = (avg_dynamic - avg_static) / avg_static * 100

        print(f"| {label} | {avg_static:.2f} | {avg_dynamic:.2f} | {delta:.1f}% |")


if __name__ == "__main__":
    # Path to your compiled binaries
    STATIC_BIN = "./builddir/test_static"
    DYNAMIC_BIN = "./builddir/test_dynamic"

    # Define your test scenarios
    tests = [
        {"label": "1 / 1 thr", "max": 10000, "batch": 1},
        {"label": "4 / 1 thr", "max": 10000, "batch": 4},
        {"label": "8 / 1 thr", "max": 10000, "batch": 8},
        {"label": "1 / 2 thr", "max": 20000, "batch": 1},
        {"label": "1 / 4 thr", "max": 40000, "batch": 1},
        {"label": "1 / 8 thr", "max": 80000, "batch": 1},
    ]

    # Number of repetitions per measurement
    RUNS = 100

    benchmark(tests, STATIC_BIN, DYNAMIC_BIN, RUNS)
