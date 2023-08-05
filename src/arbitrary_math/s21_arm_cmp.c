#include <stdbool.h>

#include "../s21_arbitrary_math.h"

// bool s21_arbitrary_is_eq(uint8_t val1[], uint8_t val2[], int size) {
//   bool is_equal = true;
//
//   for (int i = 0; i < size && is_equal; i++) is_equal = val1[i] == val2[i];
//
//   return is_equal;
// }

bool s21_arbitrary_is_eq(uint8_t val1[], uint8_t val2[], int size) {
  bool is_equal = true;
  for (int i = 0; i < size && is_equal; i++) is_equal = val1[i] == val2[i];
  return is_equal;
}

bool s21_arbitrary_is_greater(uint8_t val1[], uint8_t val2[], int size) {
  bool is_greater = false;

  for (int i = 0; i < size; i++)
    if (val1[i] > val2[i])
      is_greater = true;
    else if (val1[i] < val2[i])
      is_greater = false;

  return is_greater;
}

bool s21_arbitrary_is_less(uint8_t val1[], uint8_t val2[], int size) {
  bool is_less = false;

  for (int i = 0; i < size; i++)
    if (val1[i] < val2[i])
      is_less = true;
    else if (val1[i] > val2[i])
      is_less = false;

  return is_less;
}

bool s21_arbitrary_is_zero(uint8_t val[], int size) {
  bool is_zero = true;
  for (int i = 0; i < size && is_zero; i++) is_zero = val[i] == 0;
  return is_zero;
}
