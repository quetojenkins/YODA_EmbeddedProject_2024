import time
import subprocess
import pandas as pd
import matplotlib.pyplot as plt

def kernelHash(dictionary_path, hash):
    # run subprocess
    command = ["./vaders_fist", dictionary_path, "md5", hash]
    out = subprocess.run(command, stdout=subprocess.PIPE, stderr=subprocess.PIPE, text=True)
    output_lines = out.stdout.splitlines()

    # get password
    result = None
    for line in output_lines:
        if "Recovered Password:" in line:
            result = line.split("Recovered Password:")[1].strip()

    # get the timing data
    with open("results/times.txt", 'r') as file:
        lines = file.readlines()
        for line in lines:
            line = line.strip()
        total_time = float(lines[0])
        data_time = float(lines[1])

    return result, total_time, data_time

def seqHash(dictionary_path, hash):
    # run subprocess
    hash = hash.strip()
    command = ["./jarjars_fist", dictionary_path, hash]
    out = subprocess.run(command, stdout=subprocess.PIPE, stderr=subprocess.PIPE, text=True)
    output_lines = out.stdout.splitlines()

    # get password
    result = None
    for line in output_lines:
        if "Recovered Password:" in line:
            result = line.split("Recovered Password:")[1].strip()

    return result

def plotting(file_path):
    # Read the CSV file
    df = pd.read_csv(file_path)

    # Plot OpenCL execution time vs. C execution time
    plt.figure(figsize=(8, 6))
    plt.plot(df['Password Depth'], df['OpenCL Execution Time'], label='OpenCL Execution Time', linewidth = 2)
    plt.plot(df['Password Depth'], df['C Execution Time'], label='C Execution Time', linewidth = 2)
    plt.xlabel('Password Depth')
    plt.ylabel('Execution Time (s)')
    plt.title('OpenCL vs. C Execution Time')
    plt.legend()
    plt.grid(True)
    plt.savefig("results/exec_times.png")

    # Plot speedup
    plt.figure(figsize=(8, 6))
    plt.plot(df['Password Depth'], df['Speed Up'],color='green')
    plt.xlabel('Password Depth')
    plt.ylabel('Speed Up')
    plt.title('Speed Up (OpenCL vs. C)')
    plt.grid(True)
    plt.savefig("results/speedup.png")

if __name__ == "__main__":
    run = False
    new_test = input("New Test (y/n)?: ")
    if new_test == "y":
        new_test = input("Are you sure (y/n)?: ")
        if new_test == "y":
            run = True

    if run:
        # Define column names
        columns = ['Password Depth', 'Plain Text', 'Matching Plain Text', 'OpenCL Execution Time', 'C Execution Time', 'Speed Up']

        # Create an empty DataFrame with the defined columns
        df = pd.DataFrame(columns=columns)

        # read in all the plains
        with open("dictionaries/test_plains.txt", 'r') as file:
            plains = file.readlines()

        # read in all the hashes
        with open("dictionaries/test_hashed.txt", 'r') as file:
            hashes = file.readlines()

        # loop over every hash + plain
        for i in range(len(plains)):
            # run and time the kernel
            cl_start = time.time()
            kernel_result, total_time, data_time = kernelHash("dictionaries/rockyou.txt",hashes[i])
            cl_end = time.time()
            kernel_time = cl_end - cl_start

            # run and time the seq
            cl_start = time.time()
            seq_result = seqHash("dictionaries/rockyou.txt",hashes[i])
            cl_end = time.time()
            seq_time = cl_end - cl_start

            speedup = seq_time / kernel_time

            if seq_result.strip() == kernel_result.strip() and seq_result.strip() == plains[i].strip():
                matching = True
            else:
                matching = False

            data = [(i * 100000, plains[i].strip(), matching, kernel_time, seq_time, speedup)]
            print(data)
            df = pd.concat([df, pd.DataFrame(data, columns=columns)], ignore_index=True)
            df['Matching Plain Text'] = df['Matching Plain Text'].astype(bool)
        
        df.to_csv('results/c_vs_kernel.csv', index=False)
    
    plotting('results/c_vs_kernel.csv')
