import random

def create_random_read(read_length=15000):
    return ''.join(random.choice(['A', 'T', 'G', 'C']) for _ in range(read_length))

def simulate_reads_with_depth(input_string, read_length=1000, depth=10, length_variation=50, min_read_length=500):
    total_length = len(input_string)
    reads = []
    read_id = 1
    current_index = 0

    while current_index < total_length:
        # Introduce randomness in read length and step size
        read_length = random.randint(read_length - length_variation, read_length + length_variation)
        
        step_size = read_length // depth

        if current_index + read_length > total_length:
            current_index += step_size
            continue

        read = input_string[current_index:current_index + read_length]
        reads.append(("S_" + str(read_id), current_index, read))
        current_index += step_size
        read_id += 1

    return reads


input_string = create_random_read()
simulated_reads = simulate_reads_with_depth(input_string)

for read in simulated_reads:
    print("@" + read[0] + "_" + str(read[1]))
    print(read[2])
    print("+" + read[0] + "_" + str(read[1]))
    print("!" * len(read[2]))
