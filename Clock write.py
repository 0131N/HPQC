import time


start_time = time.time()


with open("data/test_output.txt", "w") as f:
    for i in range(8000000):
        f.write(f"Line number {i}\n")

end_time = time.time()
print(f"Time taken to write: {end_time - start_time} seconds")
