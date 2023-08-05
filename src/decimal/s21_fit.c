#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "../s21_arbitrary_math.h"
#include "../s21_decimal.h"

#define ERR_PANIC -1

s21_error s21_fit_increase_exp(uint8_t number[], int total_size, int fract_size,
                               int *exponent, int mul_size, uint8_t *ten,
                               uint8_t *mul_buffer_1, uint8_t *mul_buffer_2);

s21_error s21_fit_decrease_exp(uint8_t number[], int total_size, int fract_size,
                               int *exponent, int mul_size, uint8_t *ten,
                               uint8_t *mul_buffer_1, uint8_t *mul_buffer_2);

s21_bool s21_has_excessive_bits(uint8_t number[], int total_size,
                                int fractional_size);

// Target: increase exponent and round value while it is possible to fit without
// overflow
s21_error s21_fit_in_mantissa(uint8_t number[], int total_size,
                              int fractional_size, int *exponent) {
  s21_error error = 0;

  int mul_size = total_size * 2 + 1;
  uint8_t *ten = (uint8_t *)calloc(mul_size, 1);
  ten[0] = 10;

  uint8_t *mul_buffer_1 = (uint8_t *)calloc(mul_size, 1);
  uint8_t *mul_buffer_2 = (uint8_t *)calloc(mul_size, 1);

  if (!ten || !mul_buffer_1 || !mul_buffer_2) {
    error = ERR_PANIC;
  } else {
    bool should_decrease =
        (*exponent) > S21_MAX_EXPONENT ||
        s21_has_excessive_bits(number, total_size, fractional_size);
    if (should_decrease) {
      // Has overflow initially
      error =
          s21_fit_decrease_exp(number, total_size, fractional_size, exponent,
                               mul_size, ten, mul_buffer_1, mul_buffer_2);
    } else {
      // Has no overflow initially
      error =
          s21_fit_increase_exp(number, total_size, fractional_size, exponent,
                               mul_size, ten, mul_buffer_1, mul_buffer_2);
    }
  }

  free(ten);
  free(mul_buffer_1);
  free(mul_buffer_2);

  return error;
}

// Private function
s21_error s21_fit_increase_exp(uint8_t number[], int total_size, int fract_size,
                               int *exponent, int mul_size, uint8_t *ten,
                               uint8_t *mul_buffer_1, uint8_t *mul_buffer_2) {
  s21_error error = 0;
  bool run = (*exponent) < S21_MAX_EXPONENT;
  bool should_round_up_res = false;

  // Will multiply mantissa by ten and increment exponent (which is
  // equivalent to division by ten) while it is possible
  while (run) {
    int mul_err =
        s21_arbitrary_mul(number, ten, total_size, mul_buffer_1, mul_size);
    s21_assert_no_error(
        mul_err,
        "s21_fit_increase_exp multiplication success assertion failed");

    memcpy(mul_buffer_2, mul_buffer_1, mul_size);

    uint8_t lower_bits = 0;
    for (int i = 0; i < (fract_size - 1); i++) lower_bits += mul_buffer_1[i];

    // Rounding check. Checks highest fractional bit
    bool should_round_up =
        fract_size > 0 &&
        (mul_buffer_1[fract_size - 1] > 128 ||
         (mul_buffer_1[fract_size - 1] == 128 && lower_bits != 0) ||
         (mul_buffer_1[fract_size - 1] == 128 &&
          (mul_buffer_1[fract_size] & 1) == 1));
    if (should_round_up) {
      int add_err = s21_arbitrary_add_val(1, mul_buffer_2 + fract_size,
                                          mul_size - fract_size);
      s21_assert_no_error(
          add_err, "s21_fit_increase_exp add_val 1 success assertion failed");
    }

    bool has_no_overflow = s21_arbitrary_is_zero(
        mul_buffer_2 + fract_size + (int)S21_MANTISSA_BYTE_SIZE,
        mul_size - (fract_size + (int)S21_MANTISSA_BYTE_SIZE));

    run = has_no_overflow && (*exponent < S21_MAX_EXPONENT);
    if (run) {
      (*exponent) += 1;
      should_round_up_res = should_round_up;
      memcpy(number, mul_buffer_1, total_size);
    }
  }
  if (should_round_up_res) {
    int add_err =
        s21_arbitrary_add_val(1, number + fract_size, total_size - fract_size);
    s21_assert_no_error(
        add_err, "s21_fit_increase_exp add_val 2 success assertion failed");
  }
  if ((*exponent) < 0) error = 1;

  return error;
}

void s21_fit_shorten_num(uint8_t *ten, uint8_t *mul_buffer_1,
                         uint8_t *mul_buffer_2, int *exponent, int mul_size,
                         int new_fract_size, s21_error *error, bool *run) {
  // Round number
  uint8_t lower_bits = 0;
  for (int i = 0; i < (new_fract_size - 1); i++) lower_bits += mul_buffer_2[i];

  // Rounding check. Checks highest fractional bit
  bool should_round_up =
      new_fract_size > 0 &&
      (mul_buffer_2[new_fract_size - 1] > 128 ||
       (mul_buffer_2[new_fract_size - 1] == 128 && lower_bits != 0) ||
       (mul_buffer_2[new_fract_size - 1] == 128 &&
        (mul_buffer_2[new_fract_size] & 1) == 1));
  if (should_round_up) {
    int add_err = s21_arbitrary_add_val(1, mul_buffer_2 + new_fract_size,
                                        mul_size - new_fract_size);
    s21_assert_no_error(add_err,
                        "s21_fit_shorten_num add_val success assertion failed");
  }

  bool has_overflow =
      s21_has_excessive_bits(mul_buffer_2, mul_size, new_fract_size) ||
      (*exponent) > S21_MAX_EXPONENT;

  if (has_overflow) {
    if ((*exponent) == 0) {
      // We need to decrement exponent, but it cannot be negative
      (*error) = 1;
      (*run) = false;
    } else {
      // Equal to multiplication by ten
      (*exponent)--;
      int div_err = s21_arbitrary_div(mul_buffer_1, (uint8_t *)ten, mul_size,
                                      mul_buffer_2, mul_size);
      s21_assert_no_error(
          div_err, "s21_fit_shorten_num division success assertion failed");
      memcpy(mul_buffer_1, mul_buffer_2, mul_size);
    }
  } else {
    (*run) = false;
  }
}

// Private function
s21_error s21_fit_decrease_exp(uint8_t number[], int total_size,
                               int fractional_size, int *exponent, int mul_size,
                               uint8_t *ten, uint8_t *mul_buffer_1,
                               uint8_t *mul_buffer_2) {
  s21_error error = 0;
  // 1 extra byte to round results
  int new_fract_size = fractional_size + 1;
  mul_buffer_1[0] = 0;
  memcpy(mul_buffer_1 + 1, number, total_size);

  bool run = true;
  while (run) {
    memcpy(mul_buffer_2, mul_buffer_1, mul_size);
    s21_fit_shorten_num(ten, mul_buffer_1, mul_buffer_2, exponent, mul_size,
                        new_fract_size, &error, &run);
  }

  // Round number
  uint8_t lower_bits = 0;
  for (int i = 0; i < (new_fract_size - 1); i++) lower_bits += mul_buffer_1[i];

  // Rounding check. Checks highest fractional bit
  bool should_round_up =
      new_fract_size > 0 &&
      (mul_buffer_1[new_fract_size - 1] > 128 ||
       (mul_buffer_1[new_fract_size - 1] == 128 && lower_bits != 0) ||
       (mul_buffer_1[new_fract_size - 1] == 128 &&
        (mul_buffer_1[new_fract_size] & 1) == 1));
  // Round number
  if (should_round_up) {
    int add_err = s21_arbitrary_add_val(1, mul_buffer_1 + new_fract_size,
                                        mul_size - new_fract_size);
    s21_assert_no_error(
        add_err, "s21_fit_decrease_exp add_val success assertion failed");
  }

  memcpy(number, mul_buffer_1 + 1, total_size);

  return error;
}

s21_bool s21_has_excessive_bits(uint8_t number[], int total_size,
                                int fractional_size) {
  int excessive_bytes_count =
      (total_size - fractional_size - (int)S21_MANTISSA_BYTE_SIZE);
  return (excessive_bytes_count > 0) &&
         (!s21_arbitrary_is_zero(number + total_size - excessive_bytes_count,
                                 excessive_bytes_count));
}
