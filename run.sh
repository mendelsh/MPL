#!/usr/bin/env bash

# run.sh

# compile all C files in the current directory to a temp file (without info) run it and delete it (also delete on any exit of the program)

set -e

COMP_TIME_START=$(date +%s)

OPT_FLAGS="-O3 -ffast-math -Wall -Wextra"
CFILES=$(find . -type f -name "*.c")
OUTPUT="temp.out"

# Clean up on any exit (Ctrl+C, errors, normal exit)
trap "rm -f $OUTPUT" EXIT

gcc $OPT_FLAGS -o $OUTPUT $CFILES -lm

COMP_TIME_END=$(date +%s)

RUN_TIME_START=$(date +%s)

./$OUTPUT

RUN_TIME_END=$(date +%s)

echo
echo "Compilation complete in $((COMP_TIME_END - COMP_TIME_START)) seconds."
echo "Running complete in $((RUN_TIME_END - RUN_TIME_START)) seconds."