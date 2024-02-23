/**
 * CPE/CSC 159 Operating System Pragmatics
 * California State University, Sacramento
 *
 * Bit Utilities
 */
#ifndef BIT_H
#define BIT_H

/**
 * Counts the number of bits that are set
 * @param value - the integer value to count bits in
 * @return number of bits that are set
 */
unsigned int bit_count(unsigned int value);

/**
 * Checks if the given bit is set
 * @param value - the integer value to test
 * @param bit - which bit to check
 * @return 1 if set, 0 if not set
 */
unsigned int bit_test(unsigned int value, int bit);

/**
 * Sets the specified bit in the given integer value
 * @param value - the integer value to modify
 * @param bit - which bit to set
 */
unsigned int bit_set(unsigned int value, int bit);

/**
 * Clears the specified bit in the given integer value
 * @param value - the integer value to modify
 * @param bit - which bit to clear
 */
unsigned int bit_clear(unsigned int value, int bit);

/**
 * Toggles the specified bit in the given integer value
 * @param value - the integer value to modify
 * @param bit - which bit to toggle
 */
unsigned int bit_toggle(unsigned int value, int bit);

#endif
