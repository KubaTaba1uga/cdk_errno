#!/usr/bin/env python3
import argparse


def main():
    parser = argparse.ArgumentParser(description="Change prefix for .h file.")

    parser.add_argument(
        "--inf",
        help="Input file",
        required=True,
    )
    parser.add_argument(
        "--out",
        help="Output file",
        required=True,
    )

    parser.add_argument(
        "--new",
        help="New prefix",
        required=True,
    )

    parser.add_argument(
        "--old",
        help="Old prefix",
        required=True,
    )

    args = parser.parse_args()

    with open(args.inf, "r") as infp:
        with open(args.out, "w") as outfp:
            while True:
                inline = infp.readline()
                if not inline:
                    break

                possible_ops = ["lower", "upper", "capitalize", "title"]
                for ops in possible_ops:
                    inline = inline.replace(
                        getattr(args.old, ops)(), getattr(args.new, ops)()
                    )

                outfp.write(inline)


if __name__ == "__main__":
    main()
