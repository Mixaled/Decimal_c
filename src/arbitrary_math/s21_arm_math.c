#include <stdlib.h>
#include <string.h>

#include "../s21_arbitrary_math.h"

int s21_arbitrary_mul(uint8_t a[], uint8_t b[], int size, uint8_t dest[],
                      int dest_size) {
  int error = 0;

  if (dest_size < size * 2) {
    // Destination must be at least as big as the sum of sizes of both a and b.
    error = 1;
  } else {
    // dest = 0;
    memset(dest, 0, dest_size);

    for (int bit = size * 8 - 1; bit >= 0; bit--) {
      // dest = dest << 1;
      s21_arbitrary_shift_left(dest, dest_size, 1);

      if (s21_arbitrary_get_bit(b, size, bit)) {
        // dest += a;
        uint8_t carry = s21_arbitrary_add(a, dest, size);
        s21_arbitrary_add_val(carry, dest + size, dest_size - size);
      }
    }
  }

  return error;
}

void s21_div_unchecked(uint8_t a[], uint8_t b[], int size, uint8_t dest[],
                       int dest_size, uint8_t* shift_buffer,
                       uint8_t* mut_a_buffer);

int s21_arbitrary_div(uint8_t a[], uint8_t b[], int size, uint8_t dest[],
                      int dest_size) {
  int error = 0;

  if (s21_arbitrary_is_zero(b, size)) {
    error = 4;
  }
  if (dest_size < size) {
    // Destination must be at least as big as both a and b
    error = 1;
  } else {
    uint8_t* shift_buffer = (uint8_t*)calloc(size * 2, 1);

    if (shift_buffer) {
      uint8_t* mut_a_buffer = (uint8_t*)malloc(size * 2);

      if (mut_a_buffer) {
        s21_div_unchecked(a, b, size, dest, dest_size, shift_buffer,
                          mut_a_buffer);
        free(mut_a_buffer);
      } else {
        error = 3;
      }

      free(shift_buffer);
    } else {
      error = 2;
    }
  }

  return error;
}

// Actual code for division
void s21_div_unchecked(uint8_t a[], uint8_t b[], int size, uint8_t dest[],
                       int dest_size, uint8_t* shift_buffer,
                       uint8_t* mut_a_buffer) {
  int buffers_size = size * 2;

  memcpy(shift_buffer + size, b, size);  // shift_buffer = b << (size * 8)

  // mut_a_buffer = 00000...a;
  memcpy(mut_a_buffer, a, size);
  memset(mut_a_buffer + size, 0, buffers_size - size);

  memset(dest, 0, dest_size);  // dest = 0

  for (int bit = size * 8 - 1; bit >= 0; bit--) {
    // shift_buffer = shift_buffer >> 1;
    s21_arbitrary_shift_right(shift_buffer, buffers_size, 1);
    // dest = dest << 1;
    s21_arbitrary_shift_left(dest, dest_size, 1);

    // if (shift_buffer >= mut_a_buffer)
    if (!s21_arbitrary_is_less(mut_a_buffer, shift_buffer, buffers_size)) {
      // dest += 1;
      s21_arbitrary_add_val(1, dest, dest_size);
      // mut_a_buffer -= shift_buffer;
      s21_arbitrary_sub(shift_buffer, mut_a_buffer, buffers_size);
    }
  }
}
