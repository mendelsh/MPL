import time


start = time.time()
fib_max = 33

def fib(n):    
    return n if n <= 1 else fib(n-1) + fib(n-2)


for i in range(fib_max+1):
    print(f"fib({i}) = {fib(i)}")

end = time.time()
print(end - start)
