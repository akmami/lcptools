"""
file name: simulation.py
author: Akmuhammet Ashyralyyev
description: 
    This script is designed to simulate DNA sequencing reads. It includes functionalities 
    for generating random DNA sequences, simulating DNA reads with specified depth and 
    length variations, and writing these simulated reads to different file formats (FASTA, 
    FASTQ, GFA). The script utilizes random number generation to simulate the variability 
    inherent in sequencing processes.

dependencies:
    - random: For generating random numbers and choices.
    - gzip: For writing output to gzip-compressed files.

run: python3 simulator.py
"""
import random
import gzip


def custom_sort(t):
    """
    Custom sort function for tuples based on the length and content of the first element.

    Parameters:
    - t (tuple): A tuple where the first element is a string.

    Returns:
    - tuple: A tuple (length of first element, first element) used for sorting.
    """
    return (len(t[0]), t[0])


def create_random_read(read_length=1500000):
    """
    Generates a random DNA sequence of specified length.

    Parameters:
    - read_length (int, optional): Length of the DNA sequence to be generated. Defaults to 1,500,000.

    Returns:
    - str: A string representing a randomly generated DNA sequence.
    """
    return ''.join(random.choice(['A', 'T', 'G', 'C']) for _ in range(read_length))


def simulate_reads_with_depth(input_string, read_length=8000, depth=30, length_variation=50, min_read_length=4000):
    """
    Simulates DNA reads from a given input string with specified parameters.

    Parameters:
    - input_string (str): DNA string from which reads are simulated.
    - read_length (int, optional): Target length of each read. Defaults to 8,000.
    - depth (int, optional): Sequencing depth. Defaults to 30.
    - length_variation (int, optional): Allowed variation in read length. Defaults to 50.
    - min_read_length (int, optional): Minimum allowable read length. Defaults to 4,000.

    Returns:
    - list of tuples: Each tuple contains a read ID, the start index of the read in the input string, 
      and the read itself.
    """
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

    random.shuffle(reads)

    return reads


input_string = create_random_read()
simulated_reads = simulate_reads_with_depth(input_string)

with gzip.open('../data/simulated.fasta.gz', 'wt') as f:
    for read in simulated_reads:
        f.write(">" + read[0] + "_" + str(read[1]) + "\n")
        f.write(read[2] + "\n")


with gzip.open('../data/simulated.fastq.gz', 'wt') as f:
    for read in simulated_reads:
        f.write("@" + read[0] + "_" + str(read[1]) + "\n")
        f.write(read[2] + "\n")
        f.write("+" + read[0] + "_" + str(read[1]) + "\n")
        f.write("!" * len(read[2]) + "\n")


with open('../data/simulated.gfa', 'w') as f:
    
    reads = sorted(simulated_reads, key=custom_sort)
    
    for read in reads:
        f.write("S\t" + read[0] + "_" + str(read[1]) + "\t.\n")

    for i in range (1, len(reads)):
        f.write("L\t" + reads[i-1][0] + "_" + str(reads[i-1][1]) + "\t+\t" + reads[i][0] + "_" + str(reads[i][1]) + "\t-\t0M\n")

