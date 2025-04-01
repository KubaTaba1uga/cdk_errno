#!/usr/bin/env python3
import re
import subprocess
import sys


def main():
    if len(sys.argv) != 3:
        print(f"Usage: {sys.argv[0]} <error_dump_file> <executable>")
        sys.exit(1)

    error_dump_file = sys.argv[1]
    executable = sys.argv[2]

    try:
        with open(error_dump_file, "r") as f:
            lines = f.readlines()
    except Exception as e:
        print(f"Error reading file: {e}", file=sys.stderr)
        sys.exit(1)

    # Only parse lines after the separator
    started = False
    addresses = list()

    for line in lines:
        if not started:
            print(line, end="")
            if line.strip() == "------------------------":
                started = True
            continue

        # Extract all addresses inside square brackets
        matches = re.findall(r"\[0x[0-9a-fA-F]+\]", line)
        for match in matches:
            addr = match.strip("[]").lower()
            addresses.append(addr)

    if not addresses:
        print("No valid addresses found.")
        sys.exit(0)

    for addr in addresses:
        try:
            output = subprocess.check_output(
                ["addr2line", "-f", "-e", executable, addr], text=True
            )
            print(f"Address {addr}:")
            print(output.strip())
        except subprocess.CalledProcessError as e:
            print(f"Error processing address {addr}: {e}", file=sys.stderr)


if __name__ == "__main__":
    main()
