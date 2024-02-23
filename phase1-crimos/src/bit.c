/**
 * CPE/CSC 159 Operating System Pragmatics
 * California State University, Sacramento
 *
 * Bit Utilities
 */
#include "bit.h"

/**
 * Counts the number of bits that are set
 * @param value - the integer value to count bits in
 * @return number of bits that are set
 */
unsigned int bit_count(unsigned int value) {
    unsigned int count = 0, number = value;
    while(number > 0){
        number>>=1; // right shift value to check the nect bit
        count += number & 1; // add 1 if the least significant bit is et
    }
    return count;
}

/**
 * Checks if the given bit is set
 * @param value - the integer value to test
 * @param bit - which bit to check
 * @return 1 if set, 0 if not set
 */
unsigned int bit_test(unsigned int value, int bit) {
    return (( value >> bit ) & 1);
}

/**
 * Sets the specified bit in the given integer value
 * @param value - the integer value to modify
 * @param bit - which bit to set
 */
unsigned int bit_set(unsigned int value, int bit) {
    return (value | (1 << bit));
}

/**
 * Clears the specified bit in the given integer value
 * @param value - the integer value to modify
 * @param bit - which bit to clear
 */
unsigned int bit_clear(unsigned int value, int bit) {
    return (value & ~(1 << bit));
}

/**
 * Toggles the specified bit in the given integer value
 * @param value - the integer value to modify
 * @param bit - which bit to toggle
 */
unsigned int bit_toggle(unsigned int value, int bit) {
    return (value ^ (1 << bit));

}
