#include "../s21_arbitrary_math.h"

void s21_arbitrary_shift_left(uint8_t val[], int size, int shift) {
  if (shift < 0) {
    s21_arbitrary_shift_right(val, size, -shift);
  } else {
    for (; shift > 8; shift -= 8) s21_arbitrary_shift_left(val, size, 8);

    for (int i = size - 1; i > 0; i--) {
      uint16_t combined_words =
          ((uint16_t)val[i - 1]) | (((uint16_t)val[i]) << 8);
      val[i] = ((combined_words << shift) & 0xFF00) >> 8;
    }
    val[0] = val[0] << shift;
  }
}

void s21_arbitrary_shift_right(uint8_t val[], int size, int shift) {
  if (shift < 0) {
    s21_arbitrary_shift_left(val, size, -shift);
  } else if (shift > 0) {
    for (; shift > 8; shift -= 8) s21_arbitrary_shift_right(val, size, 8);

    for (int i = 0; i < size - 1; i++) {
      uint16_t combined_words =
          ((uint16_t)val[i]) | (((uint16_t)val[i + 1]) << 8);
      val[i] = (combined_words >> shift) & 0xFF;
    }

    val[size - 1] = val[size - 1] >> shift;
  }
}
