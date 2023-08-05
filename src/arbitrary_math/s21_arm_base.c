#include "../s21_arbitrary_math.h"

uint8_t s21_arbitrary_add(uint8_t val[], uint8_t into[], int size) {
  uint16_t carry = 0;

  for (int i = 0; i < size; i++) {
    uint16_t part_sum = ((uint16_t)val[i]) + ((uint16_t)into[i]) + carry;
    carry = part_sum >> 8;
    into[i] = part_sum & 0xFF;
  }

  return carry;
}
uint8_t s21_arbitrary_sub(uint8_t val[], uint8_t from[], int size) {
  uint16_t carry = 1;

  for (int i = 0; i < size; i++) {
    // a - b = a + (-b) = a + (~b + 1) = a + ~b + 1
    // ~b = b ^ 0x00FF
    // 1 -> carry
    uint16_t part_sum =
        (((uint16_t)val[i]) ^ 0x00FF) + ((uint16_t)from[i]) + carry;
    carry = part_sum >> 8;
    from[i] = part_sum & 0xFF;
  }

  return carry;
}

uint8_t s21_arbitrary_add_val(uint8_t val, uint8_t into[], int size) {
  uint16_t carry = val;

  for (int i = 0; i < size && carry != 0; i++) {
    uint16_t part_sum = ((uint16_t)into[i]) + carry;
    carry = part_sum >> 8;
    into[i] = part_sum & 0xFF;
  }

  return carry;
}

uint8_t s21_arbitrary_negate(uint8_t val[], int size) {
  s21_arbitrary_not(val, size);
  return s21_arbitrary_add_val(1, val, size);
}

uint8_t s21_arbitrary_get_bit(uint8_t val[], int size, int bit_id) {
  uint8_t ret_value;

  if (bit_id < 0 || (bit_id / 8) >= size) {
    ret_value = 0;
  } else {
    ret_value = (val[bit_id / 8] >> (bit_id % 8)) & 1;
  }

  return ret_value;
}

#include <stdio.h>

void s21_print_bytes(uint8_t array[], int length) {
  // printf("Bytes: ");
  // for (int i = 0; i < length; i++) printf("%4d", array[i]);
  // printf("\n");

  printf("Hex__: ");
  for (int i = 0; i < length; i++) printf("%4x", array[i]);
  printf("\n");
}

void s21_assert_no_error(int error, const char* msg) {
#if 1
  if (error != 0) {
    printf("%s\n", msg);
  }
#endif
  return;
}