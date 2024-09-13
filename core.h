#ifndef   CORE_H
#define   CORE_H

#include <vector>
#include <fstream>
#include <iterator>
#include "constant.h"
#include "encoding.h"


/**
 * @brief Computes the number of blocks required to store a bit sequence of the given size.
 * 
 * @param size The size of the bit sequence.
 * @return The number of blocks required.
 */
inline size_t block_number(size_t size);



/**
 * @brief Calculates the starting index for bit manipulation in a block.
 * 
 * @param size The size of the bit sequence.
 * @return The starting index within the first block.
 */
inline size_t start_index(size_t size);


namespace lcp {
	class core {
    public:
		#ifdef STATS
		// Core related variables
		size_t start;
		size_t end;
		#endif

		// Represenation related variables
		size_t size;
		ublock* p;

		// Other variables
		uint label;

		/**
		 * @brief Constructs a `core` object by encoding a string into bit representation.
		 * 
		 * This constructor takes iterators to a string, and encodes it into a bit sequence using
		 * the `coefficients` or `reverse_complement_coefficients` array, depending on the `rev_comp` flag.
		 * It allocates the necessary memory for the bit sequence and encodes the string iteratively.
		 * 
		 * @param begin Iterator to the beginning of the string.
		 * @param end Iterator to the end of the string.
		 * @param begin_index The starting index of the sequence.
		 * @param end_index The ending index of the sequence.
		 * @param rev_comp Boolean indicating if the reverse complement encoding should be used.
		 */
		core(std::string::iterator begin, std::string::iterator end, size_t begin_index, size_t end_index, bool rev_comp = false);
		
		/**
		 * @brief Constructs a `core` object by combining multiple `core` objects.
		 * 
		 * This constructor takes a range of `core*` objects and concatenates their bit sequences into
		 * a single `core` object. It allocates the necessary memory and performs bitwise operations to merge
		 * the individual cores.
		 * 
		 * @param begin Iterator to the first `core*` object in the range.
		 * @param end Iterator to the last `core*` object in the range.
		 */
		core(std::vector<core*>::iterator begin, std::vector<core*>::iterator end);

		/**
		 * @brief Constructs a `core` object from raw bit data.
		 * 
		 * This constructor initializes a `core` object with a given pointer to a bit sequence, and
		 * sets its size, start, and end indices.
		 * 
		 * @param p Pointer to the bit sequence.
		 * @param size The size of the bit sequence.
		 * @param start The starting index of the sequence.
		 * @param end The ending index of the sequence.
		 */
		core(ublock* p, size_t size, uint label, size_t start, size_t end);

		/**
		 * @brief Constructs a `core` object by reading from an input file stream.
		 * 
		 * This constructor reads a `core` object's metadata and bit sequence from a file.
		 * It allocates memory and reads the bit sequence into the object.
		 * 
		 * @param in The input file stream.
		 */
		core(std::ifstream& in);

		/**
		 * @brief Destructor for the `core` object.
		 * 
		 * Frees the memory allocated for the bit sequence.
		 */
		~core();


		/**
		 * @brief Compresses the current `core` object by comparing it with another `core` object.
		 * 
		 * This function compresses the current bit sequence by identifying common patterns between
		 * the current object and another `core` object, and reduces the size of the sequence accordingly.
		 * 
		 * @param other The `core` object to compare against for compression.
		 */
		void compress(const core* other);

		/**
		 * @brief Writes the `core` object to an output file stream.
		 * 
		 * This function writes the metadata and bit sequence of the `core` object to a file.
		 * 
		 * @param out The output file stream.
		 */
		void write(std::ofstream& out) const;

		/**
		 * @brief Calculates the total memory size used by the `core` object.
		 * 
		 * This function computes the memory used by the `core` object, including the
		 * bit sequence and metadata.
		 * 
		 * @return The total memory size in bytes.
		 */
		size_t memsize() const;
	};

    // core operator overloads

	/**
	 * @brief Operator overload for equality comparison between two `core` objects.
	 * 
	 * @param lhs The left-hand side `core` object.
	 * @param rhs The right-hand side `core` object.
	 * @return True if the two objects are equal, false otherwise.
	 */
	bool operator == (const core& lhs, const core& rhs);

	/**
	 * @brief Operator overload for greater-than comparison between two `core` objects.
	 * 
	 * @param lhs The left-hand side `core` object.
	 * @param rhs The right-hand side `core` object.
	 * @return True if the left-hand object is greater, false otherwise.
	 */
	bool operator > (const core& lhs, const core& rhs);

	/**
	 * @brief Operator overload for smaller-than comparison between two `core` objects.
	 * 
	 * @param lhs The left-hand side `core` object.
	 * @param rhs The right-hand side `core` object.
	 * @return True if the left-hand object is smaller, false otherwise.
	 */
	bool operator < (const core& lhs, const core& rhs);

	/**
	 * @brief Operator overload for not-equal-to comparison between two `core` objects.
	 * 
	 * @param lhs The left-hand side `core` object.
	 * @param rhs The right-hand side `core` object.
	 * @return True if the two objects are not equal, false otherwise.
	 */
	bool operator != (const core& lhs, const core& rhs);

	/**
	 * @brief Operator overload for greater-than-or-equal-to comparison between two `core` objects.
	 * 
	 * @param lhs The left-hand side `core` object.
	 * @param rhs The right-hand side `core` object.
	 * @return True if the left-hand object is greater than or equal, false otherwise.
	 */
	bool operator >= (const core& lhs, const core& rhs);

	/**
	 * @brief Operator overload for smaller-than-or-equal-to comparison between two `core` objects.
	 * 
	 * @param lhs The left-hand side `core` object.
	 * @param rhs The right-hand side `core` object.
	 * @return True if the left-hand object is smaller than or equal, false otherwise.
	 */
	bool operator <= (const core& lhs, const core& rhs);

	/**
	 * @brief Overloads the stream insertion operator (<<) to output the bit representation of a `core` object.
	 *
	 * This function iterates over the bits in the `core` object and outputs them to the given output stream. 
	 * It prints each bit as either 0 or 1, starting from the most significant bit to the least significant bit.
	 *
	 * @param os The output stream to which the bits of the `core` object will be written.
	 * @param element The `core` object to be output.
	 * @return A reference to the output stream `os` after the `core` object has been written.
	 */
	std::ostream& operator<<(std::ostream& os, const core& element);

	/**
	 * @brief Overloads the stream insertion operator (<<) to output the bit representation of a `core` pointer.
	 *
	 * This function iterates over the bits in the `core` object pointed to by the given pointer and outputs 
	 * them to the given output stream. 
	 * It prints each bit as either 0 or 1, starting from the most significant bit to the least significant bit.
	 *
	 * @param os The output stream to which the bits of the `core` object will be written.
	 * @param element A pointer to the `core` object to be output.
	 * @return A reference to the output stream `os` after the `core` object has been written.
	 */
	std::ostream& operator<<(std::ostream& os, const core* element);
}; 

#endif