#!/usr/bin/env python3

""" 
    Generate prime numbers starting from 1009, where each prime is greater than 
    double the previous prime. Stops if the next prime would exceed SIZE_MAX.
    Usage: python prime_generator.py <number_of_primes>

    This script will print the generated prime numbers as a prime_table c array definition.
"""

import sys
import math

# Dynamically determine SIZE_MAX based on the system's pointer size (in bits)
import ctypes
POINTER_SIZE = ctypes.sizeof(ctypes.c_void_p) * 8
SIZE_MAX = (2 ** POINTER_SIZE) - 1

print(f"/* Detected {POINTER_SIZE}-bit system, SIZE_MAX = {SIZE_MAX} */", file=sys.stderr)

def is_prime(n):
    if n <= 1:
        return False
    if n <= 3:
        return True
    if n % 2 == 0 or n % 3 == 0:
        return False
    i = 5
    while i * i <= n:
        if n % i == 0 or n % (i + 2) == 0:
            return False
        i += 6
    return True

def main():
    if len(sys.argv) != 2:
        print("Usage: python prime_generator.py <number_of_primes>")
        return

    num_primes = int(sys.argv[1])
    primes = []
    current = 1009

    while len(primes) < num_primes:
        if current > SIZE_MAX:
            print(f"/* Warning: Stopped at {len(primes)} primes. Next prime would exceed SIZE_MAX */", file=sys.stderr)
            break
            
        if is_prime(current):
            primes.append(current)

            # Next prime must be > 2 * current for implementing doubling strategy in the c hash map
            next_start = 2 * current + 1
            if next_start > SIZE_MAX:
                print(f"/* Note: Stopping. Next prime would exceed SIZE_MAX ({SIZE_MAX}) */", file=sys.stderr)
                break
            current = next_start
        else:
            current += 1

    print("static const size_t prime_table[] = {")
    for i, prime in enumerate(primes):
        if i < len(primes) - 1:
            print(f"    {prime},")
        else:
            print(f"    {prime}")
    print("};")
    print(f"static const size_t prime_table_size = {len(primes)};")
    
    # Print actual count achieved
    if len(primes) < num_primes:
        print(f"/* Generated {len(primes)} primes (requested {num_primes}) */", file=sys.stderr)


if __name__ == "__main__":
    main()
    