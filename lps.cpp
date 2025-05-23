#include "lps.h"

void reverse(std::string &str) {
	size_t left = 0;
	size_t right = str.size() - 1;

	while (left < right) {
		char temp = str[left];
		str[left] = str[right];
		str[right] = temp;

		left++;
		right--;
	}
};

void reverse(const char *str, int len, char **rev) {
    if (len > 0) {
        *rev = (char*) malloc(len*sizeof(char));
        size_t left = 0;
        size_t right = len - 1;

        while (left < right) {
            (*rev)[left] = str[right];
            (*rev)[right] = str[left];

            left++;
            right--;
        }
        if (left == right) {
            (*rev)[left] = str[left];
        }
    }
}; 

namespace lcp {

	lps::lps(std::string &str, const int lcp_level, const size_t sequence_split_length, const size_t overlap_margin_length) {

		this->level = 1;

		// create initial cores based on first sequence split
		this->cores = new std::vector<struct core>;
		this->cores->reserve(sequence_split_length / CONSTANT_FACTOR);

		std::string::iterator it = str.begin();

		parse(it, std::min(it + sequence_split_length, str.end()), this->cores, 0, char_gt, char_lt, char_eq, char_index, char_size, char_rep, char_data, false);

		this->deepen(lcp_level);

		// reserve the size based on the estimated core size for whole string
		this->cores->reserve(this->cores->size() * (str.size() / sequence_split_length + 1));

		// move to next split
		it += sequence_split_length;

		while (it < str.end()) {

			std::string::iterator begin = it - overlap_margin_length;
			std::string::iterator end = std::min(it + sequence_split_length, str.end());
			std::string str(begin, end);
			lcp::lps *temp = new lcp::lps(str);
			temp->deepen(lcp_level);

			// merge new processed segment with main cores
			size_t max_overlap_size = 4;
			size_t overlap_index = max_overlap_size;
			size_t max_overlap_index = 50 < temp->cores->size() ? 50 : temp->cores->size();
			bool found = false; // index for overlap

			while (overlap_index <= max_overlap_index) {

				size_t match_count = 0;
				std::vector<struct core>::iterator it1, it2;

				it1 = this->cores->end() - 1;
				it2 = temp->cores->begin() + overlap_index - 1;

				while (match_count < max_overlap_size && *((it1 - match_count)) == *((it2 - match_count))) {
					match_count++;
				}

				// found overlap with size overlap_size
				if (match_count == max_overlap_size) {
					found = true;
					break;
				}

				// try larger overlap
				overlap_index++;
			}

			if (!found) {
				overlap_index = 0;
			}

			// append new cores to the original core vector
			this->cores->insert(this->cores->end(), temp->cores->begin() + overlap_index, temp->cores->end());

			// remove non-overlapping cores so it wont be deleted
			temp->cores->erase(temp->cores->begin() + overlap_index, temp->cores->end());
			delete temp;

			// move next segment
			it += sequence_split_length;
		}
	};

	lps::lps(std::string::iterator begin, std::string::iterator end) {

		this->level = 1;

		this->cores = new std::vector<struct core>;
		this->cores->reserve((end - begin) / CONSTANT_FACTOR);

		parse(begin, end, this->cores, 0, char_gt, char_lt, char_eq, char_index, char_size, char_rep, char_data, false);
	};

    lps::lps(const char *str, int len, bool use_map, bool rev_comp) {

		this->level = 1;

		this->cores = new std::vector<struct core>;
		this->cores->reserve(len / CONSTANT_FACTOR);

		if (rev_comp) {
            char *rev = nullptr;
			reverse(str, len, &rev);
			parse(rev, rev+len, this->cores, 0, char_ptr_rc_gt, char_ptr_rc_lt, char_ptr_rc_eq, char_ptr_index, char_ptr_size, char_ptr_rev_rep, char_ptr_data, use_map);
            if (rev) {
                free(rev);
            }
		} else {
			parse(str, str+len, this->cores, 0, char_ptr_gt, char_ptr_lt, char_ptr_eq, char_ptr_index, char_ptr_size, char_ptr_rep, char_ptr_data, use_map);
		}
	};

	lps::lps(std::string &str, bool use_map, bool rev_comp) {

		this->level = 1;

		this->cores = new std::vector<struct core>;
		this->cores->reserve(str.size() / CONSTANT_FACTOR);

		if (rev_comp) {
			reverse(str);
			parse(str.begin(), str.end(), this->cores, 0, char_rc_gt, char_rc_lt, char_rc_eq, char_index, char_size, char_rev_rep, char_data, use_map);
		} else {
			parse(str.begin(), str.end(), this->cores, 0, char_gt, char_lt, char_eq, char_index, char_size, char_rep, char_data, use_map);
		}
	};

	lps::lps(std::ifstream &in) {
		in.read(reinterpret_cast<char *>(&level), sizeof(level));
		size_t size;
		in.read(reinterpret_cast<char *>(&size), sizeof(size));

		this->cores = nullptr;

		// read each core object
		if (0 < size) {
			// resize the vector to the appropriate size
			this->cores = new std::vector<struct core>;
			this->cores->reserve(size);

			// read each core object
			for (size_t i = 0; i < size; i++) {
				this->cores->emplace_back(in);
			}
		}
	};

	lps::~lps() {
		if (this->cores != nullptr)
			delete this->cores;
		this->cores = nullptr;
	};

	bool lps::dct() {

		// at least 2 cores are needed for compression
		if (this->cores == nullptr || this->cores->size() < DCT_ITERATION_COUNT + 2) {
			return false;
		}

		for (size_t dct_index = 0; dct_index < DCT_ITERATION_COUNT; dct_index++) {

			std::vector<struct core>::iterator it_curr = this->cores->end() - 1, it_left = this->cores->end() - 2;

			for (; this->cores->begin() + dct_index <= it_left; it_curr--, it_left--) {
				(it_curr)->compress(*it_left);
			}
		}

		return true;
	};

	bool lps::deepen(bool use_map) {

		// Compress cores
		if (!dct()) {
			if (this->cores != nullptr)
				delete this->cores;
			this->cores = nullptr;
			return false;
		}

		// Find new cores
		std::vector<struct core> *temp_cores = new std::vector<struct core>;
		temp_cores->reserve(this->cores->size() / CONSTANT_FACTOR);

		parse(this->cores->begin() + DCT_ITERATION_COUNT, this->cores->end(), temp_cores, DCT_ITERATION_COUNT, core_gt, core_lt, core_eq, core_index, core_size, core_rep, core_data, use_map);

		// Remove old cores
		delete this->cores;

		this->cores = temp_cores;
		temp_cores = nullptr;

		this->level++;

		return true;
	};

	bool lps::deepen(int lcp_level, bool use_map) {

		if (lcp_level <= this->level)
			return false;

		while (this->level < lcp_level && this->deepen(use_map))
			;

		return true;
	};

	void lps::write(std::ofstream &out) const {
		out.write(reinterpret_cast<const char *>(&level), sizeof(level));
		size_t size = this->cores == nullptr ? 0 : this->cores->size();
		out.write(reinterpret_cast<const char *>(&size), sizeof(size));

		// write each core object
		if (this->cores != nullptr) {
			for (std::vector<struct core>::iterator it = this->cores->begin(); it != this->cores->end(); it++) {
				(it)->write(out);
			}
		}
	};

	double lps::memsize() const {
		double total = sizeof(*this);

		if (this->cores != nullptr) {
			for (std::vector<struct core>::iterator it = this->cores->begin(); it != this->cores->end(); it++) {
				total += (it)->memsize();
			}
		}

		return total;
	};

	bool lps::get_labels(std::vector<ulabel> &labels) const {

		if (this->cores != nullptr) {
			labels.reserve(labels.size() + this->cores->size());

			for (std::vector<struct core>::iterator it = this->cores->begin(); it < this->cores->end(); it++) {
				labels.push_back((it)->label);
			}
		}

		return true;
	};

	size_t lps::size() const {
		return (this->cores == nullptr ? 0 : this->cores->size());
	};

	std::ostream &operator<<(std::ostream &os, const lps &element) {
		os << "Level: " << element.level << std::endl;
		if (element.cores != nullptr) {
			for (std::vector<struct core>::iterator it = element.cores->begin(); it != element.cores->end(); it++) {
				os << (*it) << " ";
			}
		}
		return os;
	};

	std::ostream &operator<<(std::ostream &os, const lps *element) {
		os << "Level: " << element->level << std::endl;
		if (element->cores != nullptr) {
			for (std::vector<struct core>::iterator it = element->cores->begin(); it != element->cores->end(); it++) {
				os << (*it) << " ";
			}
		}
		return os;
	};

	bool operator==(const lcp::lps &lhs, const lcp::lps &rhs) {
		if (lhs.cores->size() != rhs.cores->size()) {
			return false;
		}

		for (std::vector<struct core>::const_iterator lit = lhs.cores->begin(), rit = rhs.cores->begin(); lit < lhs.cores->end(); lit++, rit++) {
			if (*(lit) != *(rit)) {

				return false;
			}
		}

		return true;
	};

	bool operator!=(const lcp::lps &lhs, const lcp::lps &rhs) {
		if (lhs.cores->size() != rhs.cores->size()) {
			return true;
		}

		for (std::vector<struct core>::const_iterator lit = lhs.cores->begin(), rit = rhs.cores->begin(); lit < lhs.cores->end(); lit++, rit++) {
			if (*(lit) != *(rit)) {
				return true;
			}
		}

		return false;
	};
}; // namespace lcp
