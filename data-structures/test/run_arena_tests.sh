#!/usr/bin/env bash
# Script to compile, run, and log arena_chain_t tests with ASan and Valgrind

# Timestamped log file
LOG="arena_test_$(date +%Y%m%d_%H%M%S).log"
echo "=== Arena Chain Test Log - $(date) ===" > "$LOG"

# -----------------------------
# Compile ASan/UBSan binary
# -----------------------------
echo -e "\n=== Compiling with ASan/UBSan ===\n" &>> "$LOG"
gcc -fsanitize=address -fsanitize=undefined -g -o test_asan arena_test_safety.c &>> "$LOG"

if [ $? -ne 0 ]; then
    echo "Compilation failed. Check $LOG for details."
    exit 1
fi

# -----------------------------
# Run ASan binary normally
# -----------------------------
echo -e "\n=== Running tests normally (ASan) ===\n" &>> "$LOG"
./test_asan &>> "$LOG"

# -----------------------------
# Compile plain binary for Valgrind
# -----------------------------
echo -e "\n=== Compiling plain binary for Valgrind ===\n" &>> "$LOG"
gcc -g -o test_valgrind arena_test_safety.c &>> "$LOG"

if [ $? -ne 0 ]; then
    echo "Compilation for Valgrind failed. Check $LOG for details."
    exit 1
fi

# -----------------------------
# Run plain binary under Valgrind
# -----------------------------
echo -e "\n=== Running tests under Valgrind ===\n" &>> "$LOG"
valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes ./test_valgrind &>> "$LOG"

# -----------------------------
# Finish
# -----------------------------
echo -e "\n=== Test log completed: $LOG ==="
echo "All output saved to $LOG"
