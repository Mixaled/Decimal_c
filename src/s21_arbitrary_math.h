#ifndef SRC_S21_ARBITRARY_MATH_H_
#define SRC_S21_ARBITRARY_MATH_H_

#include <stdbool.h>
#include <stdint.h>

/*

This library provides functions to do basic integer number operations
with integers of ambigous/arbitrary size. Any number is represented
as array of bytes.

Numbers are passed as pointer to that array and length of array.

*/

// If carry value is     0 - everything is ok.
// If carry value is NOT 0 - there is some integer overflow happened.
// Except for negate - in case of negate, overflow is totally fine.

// Adds one byte to a number. Returns carry value.
uint8_t s21_arbitrary_add_val(uint8_t val, uint8_t into[], int size);

// Adds one number into another ('val -> into' or 'into += val').
uint8_t s21_arbitrary_add(uint8_t val[], uint8_t into[], int size);

// Substracts `val` from `from` - like if it was `from -= val`
uint8_t s21_arbitrary_sub(uint8_t val[], uint8_t from[], int size);

// Negates a number (`val = -val`)
uint8_t s21_arbitrary_negate(uint8_t val[], int size);

// Prints bytes
void s21_print_bytes(uint8_t array[], int length);

// ======

/*
Multiplies two numbers and stores result to destination (`dest = a * b`).
`dest` must be at least twice as big as `a` or `b`
Error codes:
  0 - OK
  1 - `dest` is too small
*/
int s21_arbitrary_mul(uint8_t a[], uint8_t b[], int size, uint8_t dest[],
                      int dest_size);

/*
Divides two numbers and stores result to destination (`dest = a / b`)
`dest` must be at least as big as `a` or `b`
Error codes:
  0 - OK
  1 - `dest` is too small
  2 - memory allocation error
  3 - memory allocation error
  4 - division by zero
*/
int s21_arbitrary_div(uint8_t a[], uint8_t b[], int size, uint8_t dest[],
                      int dest_size);

// Returns value of specified bit (0 or 1)
uint8_t s21_arbitrary_get_bit(uint8_t val[], int size, int bit_id);

// ======

// Shifts number to the left (`val << shift`)
void s21_arbitrary_shift_left(uint8_t val[], int size, int shift);

// Shifts number to the right (`val >> shift`)
void s21_arbitrary_shift_right(uint8_t val[], int size, int shift);

// ======

// Logic functions (& | ^ ~)
void s21_arbitrary_and(uint8_t val[], uint8_t into[], int size);
void s21_arbitrary_or(uint8_t val[], uint8_t into[], int size);
void s21_arbitrary_xor(uint8_t val[], uint8_t into[], int size);
void s21_arbitrary_not(uint8_t val[], int size);

// ======

// Checks if two numbers are equal
bool s21_arbitrary_is_eq(uint8_t val1[], uint8_t val2[], int size);

// Checks if `val1` > `val2`
bool s21_arbitrary_is_greater(uint8_t val1[], uint8_t val2[], int size);

// Checks if `val1` < `val2`
bool s21_arbitrary_is_less(uint8_t val1[], uint8_t val2[], int size);

// Checks if `val` is equal to 0
bool s21_arbitrary_is_zero(uint8_t val[], int size);

void s21_assert_no_error(int error, const char* msg);

#endif
