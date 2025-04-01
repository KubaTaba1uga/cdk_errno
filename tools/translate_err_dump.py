#!/usr/bin/env python3
import re
import subprocess
import sys


def main():
    if len(sys.argv) < 3:
        print("Usage: {} <error_dump_file> <executable>".format(sys.argv[0]))
        sys.exit(1)

    error_dump_file = sys.argv[1]
    executable = sys.argv[2]

    try:
        with open(error_dump_file, "r") as f:
            lines = f.readlines()
    except Exception as e:
        print(f"Error opening file {error_dump_file}: {e}")
        sys.exit(1)

    for line in lines:
        line = line.strip()
        # Extract any addresses inside square brackets like [0x...]
        matches = re.findall(r"\[0x([0-9a-fA-F]+)\]", line)
        if matches:
            # Use the last match (assumed to be the actual backtrace address)
            addr = "0x" + matches[-1]
            try:
                output = subprocess.check_output(
                    ["addr2line", "-f", "-C", "-e", executable, addr],
                    universal_newlines=True,
                )
                print(f"Address {addr}:")
                print(output.strip())
            except subprocess.CalledProcessError as e:
                print(f"Error processing address {addr}: {e}")
        else:
            print("No valid address found in line:", line)


if __name__ == "__main__":
    main()
