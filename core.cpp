/**
 * @file core.cpp
 * @brief Implementation of the `core` struct and its associated functions.
 *
 * This file contains the implementation of the `core` struct, which is used to
 * represent a sequence of encoded bits for string data. The stuct supports
 * operations such as compression, comparison, and writing/reading to files.
 *
 * Key operations include:
 * - Encoding strings into bit arrays using coefficient-based encoding.
 * - Constructing `core` objects from strings or other `core` objects.
 * - Compressing bit representations to optimize memory usage.
 * - Writing and reading `core` objects to and from files.
 * - Comparing `core` objects with overloaded operators.
 * - Efficiently handling block-wise bit manipulations.
 *
 * @note The `STATS` macro is used to conditionally compile sections of the code
 * that track additional metadata such as `start` and `end` indices for
 * performance analysis.
 */

#include "core.h"

namespace lcp {

	core::core(size_t bit_size, ublock *bit_rep, ulabel label, size_t start, size_t end) {
		this->bit_size = bit_size;
		this->bit_rep = bit_rep;
		this->label = label;

#ifdef STATS
		this->start = start;
		this->end = end;
#else
		(void)start;
		(void)end;
#endif
	};

	core::core(std::ifstream &in) {
#ifdef STATS
		in.read(reinterpret_cast<char *>(&start), sizeof(start));
		in.read(reinterpret_cast<char *>(&end), sizeof(end));
#endif
		in.read(reinterpret_cast<char *>(&bit_size), sizeof(bit_size));
		size_t block_number = (bit_size + UBLOCK_BIT_SIZE - 1) / UBLOCK_BIT_SIZE;
		this->bit_rep = new ublock[block_number];
		in.read(reinterpret_cast<char *>(bit_rep), block_number * sizeof(ublock));
		in.read(reinterpret_cast<char *>(&label), sizeof(label));
	};

	core::~core() {
		if (this->bit_rep != nullptr) {
			delete[] this->bit_rep;
		}
	};

	void core::compress(const struct core &other) {

		ubit_size index = std::min(this->bit_size, other.bit_size);
		ubit_size t_block = (this->bit_size - 1) / UBLOCK_BIT_SIZE,
				 o_block = (other.bit_size - 1) / UBLOCK_BIT_SIZE;

		while (index >= UBLOCK_BIT_SIZE && this->bit_rep[t_block] == other.bit_rep[o_block]) {
			t_block--;
			o_block--;
			index -= UBLOCK_BIT_SIZE;
		}

		t_block = this->bit_rep[t_block];
		o_block = other.bit_rep[o_block];

		while (index > 0 && t_block % 2 == o_block % 2) {
			t_block /= 2;
			o_block /= 2;
			index--;
		}

		if (this->bit_size > UBLOCK_BIT_SIZE) {
			delete[] this->bit_rep;
			this->bit_rep = new ublock[1];
		}

		this->bit_rep[0] = 0;

		// shift left by 1 bit and set last bit to difference
		this->bit_rep[0] = 2 * (std::min(this->bit_size, other.bit_size) - index) + (t_block % 2);
		this->bit_size = 0;

		if (this->bit_rep[0] > 0) {
			this->bit_size = (32 - __builtin_clz(this->bit_rep[0]));
		}

		this->bit_size = this->bit_size > 1 ? this->bit_size : 2;
	};

	void core::write(std::ofstream &out) const {
#ifdef STATS
		out.write(reinterpret_cast<const char *>(&start), sizeof(start));
		out.write(reinterpret_cast<const char *>(&end), sizeof(end));
#endif
		out.write(reinterpret_cast<const char *>(&bit_size), sizeof(bit_size));
		out.write(reinterpret_cast<const char *>(bit_rep), ((bit_size + UBLOCK_BIT_SIZE - 1) / UBLOCK_BIT_SIZE) * sizeof(ublock));
		out.write(reinterpret_cast<const char *>(&label), sizeof(label));
	};

	size_t core::memsize() const {
		return sizeof(*this) + sizeof(ublock) * ((this->bit_size + UBLOCK_BIT_SIZE - 1) / UBLOCK_BIT_SIZE);
	};

	struct core &core::operator=(const struct core &other) {
		if (*this == other) {
			return *this;
		}

		if (this->bit_rep) {
			delete[] this->bit_rep;
		}

		this->bit_rep = nullptr;

#ifdef STATS
		this->start = other.start;
		this->end = other.end;
#endif

		this->bit_size = other.bit_size;

		if (this->bit_size) {
			this->bit_rep = new ublock[(other.bit_size + UBLOCK_BIT_SIZE - 1) / UBLOCK_BIT_SIZE];
			std::copy(other.bit_rep, other.bit_rep + ((other.bit_size + UBLOCK_BIT_SIZE - 1) / UBLOCK_BIT_SIZE) * sizeof(ublock), this->bit_rep);
		}

		this->label = other.label;

		return *this;
	};

	// core operator overloads
	bool operator==(const struct core &lhs, const struct core &rhs) {

		if (lhs.bit_size != rhs.bit_size) {
			return false;
		}

		ubit_size index = 0;

		while (index < lhs.bit_size) {
			if (lhs.bit_rep[index / UBLOCK_BIT_SIZE] != rhs.bit_rep[index / UBLOCK_BIT_SIZE])
				return false;

			index += UBLOCK_BIT_SIZE;
		}

		return true;
	};

	bool operator!=(const struct core &lhs, const struct core &rhs) {

		if (lhs.bit_size != rhs.bit_size) {
			return true;
		}

		ubit_size index = 0;

		while (index < lhs.bit_size) {
			if (lhs.bit_rep[index / UBLOCK_BIT_SIZE] != rhs.bit_rep[index / UBLOCK_BIT_SIZE])
				return true;

			index += UBLOCK_BIT_SIZE;
		}

		return false;
	};

	bool operator>(const struct core &lhs, const struct core &rhs) {

		if (lhs.bit_size != rhs.bit_size) {
			return lhs.bit_size > rhs.bit_size;
		}

		ubit_size index = 0;

		while (index < lhs.bit_size) {
			if (lhs.bit_rep[index / UBLOCK_BIT_SIZE] == rhs.bit_rep[index / UBLOCK_BIT_SIZE]) {
				index += UBLOCK_BIT_SIZE;
				continue;
			}

			return lhs.bit_rep[index / UBLOCK_BIT_SIZE] > rhs.bit_rep[index / UBLOCK_BIT_SIZE];
		}

		return false;
	};

	bool operator<(const struct core &lhs, const struct core &rhs) {

		if (lhs.bit_size != rhs.bit_size) {
			return lhs.bit_size < rhs.bit_size;
		}

		ubit_size index = 0;

		while (index < lhs.bit_size) {
			if (lhs.bit_rep[index / UBLOCK_BIT_SIZE] == rhs.bit_rep[index / UBLOCK_BIT_SIZE]) {
				index += UBLOCK_BIT_SIZE;
				continue;
			}

			return lhs.bit_rep[index / UBLOCK_BIT_SIZE] < rhs.bit_rep[index / UBLOCK_BIT_SIZE];
		}

		return false;
	};

	bool operator>=(const struct core &lhs, const struct core &rhs) {

		if (lhs.bit_size != rhs.bit_size) {
			return lhs.bit_size >= rhs.bit_size;
		}

		ubit_size index = 0;

		while (index < lhs.bit_size) {
			if (lhs.bit_rep[index / UBLOCK_BIT_SIZE] != rhs.bit_rep[index / UBLOCK_BIT_SIZE]) {
				return lhs.bit_size >= rhs.bit_size;
			}

			index += UBLOCK_BIT_SIZE;
		}

		return true;
	};

	bool operator<=(const struct core &lhs, const struct core &rhs) {

		if (lhs.bit_size != rhs.bit_size) {
			return lhs.bit_size <= rhs.bit_size;
		}

		ubit_size index = 0;

		while (index < lhs.bit_size) {
			if (lhs.bit_rep[index / UBLOCK_BIT_SIZE] != rhs.bit_rep[index / UBLOCK_BIT_SIZE]) {
				return lhs.bit_size <= rhs.bit_size;
			}

			index += UBLOCK_BIT_SIZE;
		}

		return true;
	};

	std::ostream &operator<<(std::ostream &os, const struct core &element) {
		size_t block_number = (element.bit_size - 1) / UBLOCK_BIT_SIZE + 1;
		for (int index = element.bit_size - 1; 0 <= index; index--) {
			os << ((element.bit_rep[block_number - index / UBLOCK_BIT_SIZE - 1] >> (index % UBLOCK_BIT_SIZE)) & 1);
		}

		return os;
	};

	std::ostream &operator<<(std::ostream &os, const struct core *element) {
		size_t block_number = (element->bit_size - 1) / UBLOCK_BIT_SIZE + 1;
		for (int index = element->bit_size - 1; 0 <= index; index--) {
			os << ((element->bit_rep[block_number - index / UBLOCK_BIT_SIZE - 1] >> (index % UBLOCK_BIT_SIZE)) & 1);
		}

		return os;
	};

}; // namespace lcp