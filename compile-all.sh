#!/usr/bin/env bash

# Compile all C files in the current directory with optimizations
# Usage: ./compile-all.sh -o output_file
# Output: Compilation complete in <time> seconds. Output file: output_file

set -e

time_start=$(date +%s)
output_file="main.out"
OPT_FLAGS="-O2 -Wall -Wextra -fopt-info-optimized"

while getopts ":o:" opt; do
    case $opt in
        o) output_file="$OPTARG" ;;
        \?) echo "Invalid option: -$OPTARG" >&2 ;;
    esac
done

CFILES=$(find . -type f -name "*.c")

gcc $OPT_FLAGS -o "$output_file" $CFILES -lm

time_end=$(date +%s)
echo
echo "Compilation complete in $((time_end - time_start)) seconds. Output file: $output_file"

