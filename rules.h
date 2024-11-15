#ifndef RULES_H
#define RULES_H

#include <string>
#include <vector>
#include "core.h"


namespace lcp {

    /**
     * Compares two characters from a string using a custom alphabet mapping.
     *
     * @param a An iterator pointing to the first character.
     * @param b An iterator pointing to the second character.
     * @return true if the character pointed to by a is greater than the character
     *         pointed to by b based on the modularity of the alphabet mapping;
     *         false otherwise.
     */
    inline bool char_gt( const std::string::iterator a, const std::string::iterator b ) {
        return ( alphabet[static_cast<unsigned char>(*a)] ) > ( alphabet[static_cast<unsigned char>(*b)] );
    };
    
    /**
     * Compares two characters from a string using a custom alphabet mapping.
     *
     * @param a An iterator pointing to the first character.
     * @param b An iterator pointing to the second character.
     * @return true if the character pointed to by a is less than the character
     *         pointed to by b based on the modularity of the alphabet mapping;
     *         false otherwise.
     */
    inline bool char_lt( const std::string::iterator a, const std::string::iterator b ) {
        return ( alphabet[static_cast<unsigned char>(*a)] ) < ( alphabet[static_cast<unsigned char>(*b)] );
    };

    /**
     * Compares two characters from a string using a custom alphabet mapping.
     *
     * @param a An iterator pointing to the first character.
     * @param b An iterator pointing to the second character.
     * @return true if the character pointed to by a is equal to the character
     *         pointed to by b based on the modularity of the alphabet mapping;
     *         false otherwise.
     */
    inline bool char_eq( const std::string::iterator a, const std::string::iterator b ) {
        return ( alphabet[static_cast<unsigned char>(*a)] ) == ( alphabet[static_cast<unsigned char>(*b)] );
    };

    /**
     * Compares two core objects pointed to by iterators to determine their order.
     *
     * @param a An iterator pointing to the first core object.
     * @param b An iterator pointing to the second core object.
     * @return true if the core object pointed to by a is greater than the core
     *         object pointed to by b; false otherwise.
     */
    inline bool core_gt( const std::vector<struct core>::iterator a, const std::vector<struct core>::iterator b ) {
        return (*a) > (*b);
    };

    /**
     * Compares two core objects pointed to by iterators to determine their order.
     *
     * @param a An iterator pointing to the first core object.
     * @param b An iterator pointing to the second core object.
     * @return true if the core object pointed to by a is less than the core
     *         object pointed to by b; false otherwise.
     */
    inline bool core_lt( const std::vector<struct core>::iterator a, const std::vector<struct core>::iterator b ) {
        return (*a) < (*b);
    };

    /**
     * Compares two characters from a string using a custom alphabet mapping.
     *
     * @param a An iterator pointing to the first character.
     * @param b An iterator pointing to the second character.
     * @return true if the character pointed to by a is equal to the character
     *         pointed to by b based on the modularity of the alphabet mapping;
     *         false otherwise.
     */
    inline bool core_eq( const std::vector<struct core>::iterator a, const std::vector<struct core>::iterator b ) {
        return (*a) == (*b);
    };
    
    /**
     * Determines if the element at the given iterator position is a local minimum.
     *
     * @tparam Iterator The type of the iterator.
     * @tparam Compare A comparison function, either greater-than or less-than.
     * @param it The iterator pointing to the current element.
     * @param gt The greater-than comparison function.
     * @param lt The less-than comparison function.
     * @return true if the element at the iterator position is a local minimum based
     *         on the comparison functions; false otherwise.
     */
    template <typename Iterator, typename Compare>
    bool isLMIN( const Iterator it, Compare gt, Compare lt ) {
        return  gt(it, it + 1) && 
                lt(it + 1, it + 2);
    };

    /**
     * Determines if the element at the given iterator position is a local maximum.
     *
     * @tparam Iterator The type of the iterator.
     * @tparam Compare A comparison function, either greater-than or less-than.
     * @param it The iterator pointing to the current element.
     * @param end The end iterator for the range being checked.
     * @param gt The greater-than comparison function.
     * @param lt The less-than comparison function.
     * @return true if the element at the iterator position is a local maximum based
     *         on the comparison functions; false otherwise.
     */
    template <typename Iterator, typename Compare>
    bool isLMAX( const Iterator it, const Iterator end, Compare gt, Compare lt ) {
        return  (it + 3) < end && 
                lt(it, it + 1) && 
                gt(it + 1, it + 2) && 
                !gt(it - 1, it) && 
                !lt(it + 2, it + 3);
    };

    /**
     * Counts the number of consecutive elements in the middle of a sequence that are equal.
     *
     * @tparam Iterator The type of the iterator.
     * @tparam Compare The equality comparison function.
     * @param it The iterator pointing to the start of the range.
     * @param end The end iterator for the range being checked.
     * @param eq The equality comparison function.
     * @return The count of consecutive elements in the middle that are equal; returns 0 if 
     *         the sequence runs to the end.
     */
    template <typename Iterator, typename Compare>
    size_t countMiddle( const Iterator it, const Iterator end, Compare eq ) {

        size_t middle_count = 1;
        Iterator temp = it + 2;

        while ( temp < end && eq( (temp - 1), temp ) ) {
            temp++;
            middle_count++;
        }

        if ( temp == end ) {
            return 0;
        }

        return middle_count;
    };

    /**
     * Checks if two sequences do not overlap, indicating that one sequence starts
     * after the other sequence ends.
     *
     * @tparam Iterator The type of the iterator.
     * @param cur_begin The iterator pointing to the start of the current sequence.
     * @param prev_end The iterator pointing to the end of the previous sequence.
     * @return true if the sequences do not overlap (i.e., `cur_begin` starts after
     *         `prev_end` ends); false otherwise.
     */
    template <typename Iterator>
    bool isSSEQ( const Iterator cur_begin, const Iterator prev_end ) {
        return prev_end < cur_begin;
    };

};

#endif