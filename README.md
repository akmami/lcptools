# `LCP` (Locally Consistent Parsing) Algorithm Implementation <br>
![GitHub release (latest SemVer)](https://img.shields.io/github/v/release/akmami/lcptools)
![GitHub last commit](https://img.shields.io/github/last-commit/akmami/lcptools)
![GitHub](https://img.shields.io/github/license/akmami/lcptools)

This repository contains an implementation of the Locally Consistent Parsing (LCP) algorithm, applied to strings using a specific binary alphabet encoding. The implementation is in C++ and is designed for efficient computation of LCP on large datasets.

## Features

- **Efficient LCP Computation:** Implemented in C++ for efficient and scalable computation on large datasets.
- **High Accuracy:** Achieves highly accurate comparisons by leveraging the unique LCP approach.
- **Designed for Genomics:** Specifically caters to the needs of genomic researchers and bioinformaticians.

## Installation

You can install lcptools either system-wide (requires sudo privileges) or in a user-specific directory (no sudo required).

### System-wide Installation

To install lcptools system-wide, you need sudo privileges. This will install the library in `/usr/local`.

1. **Install the repository:**
    ```sh
    git clone https://github.com/akmami/lcptools.git
    cd lcptools

    # Build the library
    make

    # Install the library
    sudo make install
    ```

2. **Uninstall the library (if needed):**
    ```sh
    sudo make uninstall
    ```

### User-specific Installation

To install lcptools in your home directory (or another custom directory), you don't need sudo privileges.

1. **Install the repository:**
    ```sh
    git clone https://github.com/akmami/lcptools.git
    cd lcptools

    # Build the library
    make

    # Install the library to a custom directory (e.g., `~/.local`):**
    make install PREFIX=$(HOME)/.local
    ```

2. **Uninstall the library from the custom directory (if needed):**
    ```sh
    make uninstall PREFIX=$(HOME)/.local
    ```

## Usage

To compile your program with your program, you need to specify the include and library paths based on your installation method.

### System-wide Installation

```sh
g++ -o your_program your_program.cpp -llcptools
```

### User-specific Installation

```sh
g++ -o your_program your_program.cpp -I$(HOME)/.local/include/lcptools -L$(HOME)/.local/lib -llcptools
```

Alternatively, you can include the header directly in your program:

```sh
#include "lcptools/string.cpp"
```

Note: Make sure that paths are correct.

## Character Encoding

The binary encoding of the alphabet is defined as follows. This default encoding is used unless a custom encoding is provided:

| Character | Binary Encoding |
| --------- | --------------- |
| A, a      | 00              |
| T, t      | 11              |
| G, g      | 10              |
| C, c      | 01              |

### Initialization

To initialize the encodings, use the following function call at the beginning of your program. A boolean parameter `verbose` can be provided, which, when set to `true`, prints a summary of the encoding:

```cpp
lcp::init_coefficients(verbose);
```

To display the encoding summary separately, use:

```cpp
lcp::encoding_summary();
```

For custom character encoding, initialize the encoding by passing a `std::map<char, int>` object to the initialization function:

```cpp
std::map<char, int> customEncoding = { {'A', 3}, {'T', 0}, {'C', 2}, {'G', 1} };
lcp::init_coefficients(customEncoding, verbose);
```

In the above code, `verbose` is an optional boolean parameter, defaulting to `false`.

## Usage Example

Below is an example demonstrating the usage of the LCP algorithm implementation:

```cpp
#include <iostream>
#include "string.cpp"

int main() {
    // Initialize alphabet coefficients
    lcp::init_coefficients();

    // Example string
    std::string str = "GGGACCTGGTGACCCCAGCCCACGACAGCCAAGCGCCAGCTGAGCTCAGGTGTGAGGAGATCACAGTCCT";

    // Create LCP string object
    lcp::string *lcp_str = new lcp::string(str);

    // Deepen the LCP analysis
    lcp_str->deepen();

    // Output LCP string
    std::cout << *lcp_str << std::endl;

    // Clean up to prevent memory leaks
    delete lcp_str;

    return 0;
}
```

## LCP Algorithm Description

The LCP algorithm operates as follows:

### Constructor:

Processes the input string and identifies cores that adhere to specific rules:

1. The subsequent characters should not be the same, and the middle character is local minima.

	Ex: *xyz* where *x!=y* and *y!=z*.

	**y<x** and **y<z**

2. The subsequent characters should not be the same, and the middle character local maxima, and its neighbors are not local minima. 

	Ex: *sxyzt* where *s!=x* and *x!=y* and *y!=z* and *z!=st*.

	**x<y** and **z<y** and **s<=x** and **z>=t**.

3. The characters, except the front and back, are the same.

	Ex: *xyyz*, *xyyyz*, *xyyyyz*, ...

	**x!=y** and **y!=z**

### Compress Function:

The compress function in the LCP algorithm is crucial for processing binary sequences. It starts by pinpointing the initial point of difference between two binary strings, beginning from the right-end. The function then assesses the difference based on the position and value of the divergent bit. This detail is transformed into a new binary sequence, which establishes the foundation of a newly generated 'core'. This core is a clear representation of the differences between the original sequences, integral to the algorithm's deepening process. Essentially, the compress function effectively consolidates and encapsulates the information, ensuring efficient further analysis within the LCP framework.

```
PROGRAM compress(bits1, bits2):
	
	position <- FIND FIRST DIFFERENCE BETWEEN bits1 AND bits2 STARTING FROM RIGHT
	difference <- GET BIT FROM THE bits2 AT POSITION (bits2.length - position)
	new_bits <- position * 2 + difference

	new_core = CREATE NEW CORE FROM new_bits

	return new_core
```

Ex: 11101**0**00 vs 00010**1**00 -> **100**0 as the position is **2 (100)** and the bit is **0**. Position index start from 0.

### Deepen Function:

The deepen function in the LCP algorithm primarily focuses on the compression of 'cores' alongside their left neighbors. The purpose of this repeated compression is to manage the length of the cores, preventing them from becoming large. After a few rounds of compression, the LCP algorithm is re-applied. This re-application aims to identify new cores within the compressed data. In this context, each compressed core is treated as a discrete value, represented in binary form. This representation facilitates efficient processing and analysis within the algorithm.

This function iteratively compresses and processes cores to find new cores in compliance with the rules stated above.

```
PROGRAM deepen(core_length=5, verbose=false):
	
	iteration_index <- 0

	FOR iteration_index < COMPRESSION_ITERATION_COUNT:
		iteration_index++
	 	temp_prev <- cores[0]
	 
		FOR index in range(1, cores.length):
			temp_curr <- cores[index]
			cores[index] <- compress(temp_prev, temp_curr)
			temp_prev <- temp_curr
		ENDFOR

		cores.pop_front() // it is not compressed since it has no left neighbour
	ENDFOR
	
	RUN LCP ALGORITHM FOR COMPRESSED CORES TO FIND NEW CORES
	CONCATENATE NEW CORES THAT COMPLY WITH EITHER ONE OF THE RULE DISCUSSED IN THE PREVIOUS SECTION

	level += 1
```

The deepen function in the LCP algorithm primarily focuses on the compression of 'cores' alongside their left neighbors. The purpose of this repeated compression is to manage the length of the cores, preventing them from becoming unwieldy with each iteration of deepening. After a few rounds of compression, the LCP algorithm is reapplied. This reapplication aims to identify new cores within the compressed data. In this context, each compressed core is treated as a discrete value, represented in binary form. This representation facilitates efficient processing and analysis within the algorithm.

## Default Variables

The default iteration count for compression in each deepening is set to 2.

The default value for `verbose` is set to `false`.


