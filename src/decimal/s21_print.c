#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "../s21_decimal.h"
#include "s21_bits_bindec_data.h"

#define BITS_IN_INT ((int)(sizeof(int) * 8))

void add_up_all_bits(s21_decimal *val, uint32_t base_10[MAX_DIGITS]) {
  const uint32_t BINDEC_DATA[][MAX_DIGITS] = BITS_BINDEC_DATA;

  // Add up all the bits in 10-base
  for (int bit_id = 0; bit_id < BITS_IN_INT * 3; bit_id++) {
    int int_id = bit_id / BITS_IN_INT;
    int shift = bit_id % BITS_IN_INT;

    int bit = (val->bits[int_id] >> shift) & 1;

    for (int digit = 0; digit < MAX_DIGITS && bit; digit++)
      base_10[digit] += BINDEC_DATA[bit_id][MAX_DIGITS - 1 - digit];
  }
}

void normalize_bindec_number(uint32_t base_10[MAX_DIGITS]) {
  // Normalize
  uint32_t carry = 0;
  for (int digit = 0; digit < MAX_DIGITS; digit++) {
    base_10[digit] += carry;
    carry = base_10[digit] / 10;
    base_10[digit] = base_10[digit] % 10;
  }
}

void print_bindec(uint32_t base_10[MAX_DIGITS], int point_pos) {
  int8_t has_number_started = 0;

  int first_digit = point_pos - 1;
  for (int digit = 0; digit < point_pos && first_digit == point_pos - 1;
       digit++)
    if (base_10[digit] != 0) first_digit = digit;

  // Print all character before point
  for (int digit = MAX_DIGITS - 1; digit >= 0; digit--) {
    if (digit < 0) {
      putc('0', stdout);
    } else {
      if (base_10[digit] != 0 || has_number_started || digit == point_pos) {
        has_number_started = 1;
        putc('0' + base_10[digit], stdout);
      }
    }

    if (digit == point_pos) putc('.', stdout);
  }
}

void s21_convert_to_digits(s21_decimal val, uint32_t base_10[MAX_DIGITS]) {
  memset(base_10, 0, MAX_DIGITS * sizeof(uint32_t));
  add_up_all_bits(&val, base_10);
  normalize_bindec_number(base_10);
}

void s21_print(s21_decimal val) {
  uint32_t base_10[MAX_DIGITS] = {0};
  s21_convert_to_digits(val, base_10);

  if (s21_get_sign_bit(&val)) putc('-', stdout);

  print_bindec(base_10, s21_get_exponent(&val));
}
