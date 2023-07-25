# LCP
The string class contains an algorithm that runs LCP on a given string and alphabet encoding.


## Character encoding

The default encoding of the alphabet is accordingly.

| Character | Binary |
| --------- | ------ |
| A | 00 |
| T | 11 |
| G | 10 |
| C | 01 |
| a | 00 |
| t | 11 |
| g | 10 |
| c | 01 |

You have to initialize the encodings once at the beginning. You can provide an optional *verbose* bool variable, which will print the summary of the encoding. 

```
 lcp::init_coefficients(verbose);
```

You can specifically call the function that prints the summary of the encoding as

```
 lcp::encoding_summary();
```

If you want to have your encoding values for the characters, you can initialize by passing the map<char, int> object to the initialization function.

```
 std::map<char, int> mp = { {'A', 3}, {'T', 0}, {'C', 2}, {'G', 1} };
 lcp::init_coefficients(mp, verbose);
```

where *verbose* is an optional bool parameter that is set to false as default.

## Usage

The program given below shows the usage of the LCP string.

```
#include <iostream>
#include "string.cpp"

int main() {

 // initializing coefficients of the alphabet
 lcp::init_coefficients();

 std::string str = "GGGACCTGGTGACCCCAGCCCACGACAGCCAAGCGCCAGCTGAGCTCAGGTGTGAGGAGATCACAGTCCT";

 lcp::string *lcp_str = new lcp::string(str);

 // increasing level of the lcp
 lcp_str->deepen();

 std::cout << lcp_str << std::endl;

 // prevent memory leaks
 delete lcp_str;

 return 0;
};
```

## LCP Algorithm

The constructor takes the string and finds all the cores which comply with one of the rules:

1. The subsequent characters should not be the same, and the middle character is either local minima or local maxima, and its neighbors are not local minima. 

Ex: *xyzst* where *x!=y* and *y!=z* and *z!=s* and *s!=t*.
**z<=x,y,s,t** or **x<y<z && z>s>t**

2. The characters, except the front and back, are the same. 

Ex: xyyz, xyyyz, xyyyyz, ... where x!=y and y!=z

### deepen

This function has the following function.

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


The compression is done for the cores with their left neighbours. After compressing cores a couple of times, which is done to prevent the increase of the length of the cores for each deepening, the same LCP algorithm is called, which finds new cores from compressed cores. Each compressed core is treated as a value as it is represented in bits.

### compress
This function has the following function.

```
PROGRAM compress(bits1, bits2):
	
	position <- FIND FIRST DIFFERENCE BETWEEN bits1 AND bits2 STARTING FROM RIGHT
	difference <- GET BIT FROM THE bits2 AT POSITION (bits2.length - position)
	new_bits <- position * 2 + difference

	new_core = CREATE NEW CORE FROM new_bits

	return new_core
```

Ex: 11101**0**00 vs 00010**1**00 -> **100**0 as the position is **2 (100)** and the bit is **0**. Position index start from 0.

## Default Variables

The default core length is set to 5.

The default iteration count for compression in each deepening is set to 2.

The default value for verbose is set to false.


