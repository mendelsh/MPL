#!/usr/bin/env bash

# compile-all.sh

# Compile all C files in the current directory with optimizations
# Usage: ./compile-all.sh -o output_file
# Output: Compilation complete in <time> seconds. Output file: output_file

set -e

time_start=$(date +%s)
output_file="main.out"
OPT_FLAGS="-O3 -ffast-math -Wall -Wextra"

while getopts ":o:" opt; do
    case $opt in
        o) output_file="$OPTARG" ;;
        \?) echo "Invalid option: -$OPTARG" >&2 ;;
    esac
done

CFILES=$(find . -path "*/test" -prune -o -type f -name "*.c" -print)

# Redirect optimization info to file instead of stdout
gcc $OPT_FLAGS -fopt-info-optimized=optimization.log -o "$output_file" $CFILES -lm

time_end=$(date +%s)
echo
echo "Compilation complete in $((time_end - time_start)) seconds. Output file: $output_file"
echo "Optimization info saved to: optimization.log"