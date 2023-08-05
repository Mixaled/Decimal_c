#include <stdbool.h>
#include <stdint.h>
#include <string.h>

#include "../s21_arbitrary_math.h"
#include "../s21_decimal.h"

// TODO:
// Initialize everything
// Check for validity of every input

#define BUF_SIZE (S21_MANTISSA_BYTE_SIZE + S21_MAX_EXPONENT + 2)
int s21_add(s21_decimal value_1, s21_decimal value_2, s21_decimal *result) {
  (*result) = s21_default_decimal(0, 0);
  int error = 0;
  uint8_t ten[BUF_SIZE] = {10};
  uint8_t buffer_1[BUF_SIZE] = {0};
  uint8_t buffer_2[BUF_SIZE] = {0};
  uint8_t buffer_tmp[BUF_SIZE * 2] = {0};
  memcpy(&buffer_1[1], (uint8_t *)value_1.bits, S21_MANTISSA_BYTE_SIZE);
  memcpy(&buffer_2[1], (uint8_t *)value_2.bits, S21_MANTISSA_BYTE_SIZE);
  int exp_1 = s21_get_exponent(&value_1);
  int exp_2 = s21_get_exponent(&value_2);

  // ---- MAKE EXPONENTS EQUAL
  while (exp_1 < exp_2) {
    int mul_err =
        s21_arbitrary_mul(buffer_1, ten, BUF_SIZE, buffer_tmp, BUF_SIZE * 2);
    s21_assert_no_error(mul_err,
                        "s21_add multiplication success assertion 1 failed");

    memcpy(buffer_1, buffer_tmp, BUF_SIZE);
    exp_1++;
  }

  while (exp_2 < exp_1) {
    int mul_err =
        s21_arbitrary_mul(buffer_2, ten, BUF_SIZE, buffer_tmp, BUF_SIZE * 2);
    s21_assert_no_error(mul_err,
                        "s21_add multiplication success assertion 2 failed");

    memcpy(buffer_2, buffer_tmp, BUF_SIZE);
    exp_2++;
  }

  // ---- NEGATE IF NEEDED
  if (s21_get_sign_bit(&value_1) == S21_SIGN_NEGATIVE)
    s21_arbitrary_negate(buffer_1, BUF_SIZE);

  if (s21_get_sign_bit(&value_2) == S21_SIGN_NEGATIVE)
    s21_arbitrary_negate(buffer_2, BUF_SIZE);

  // ---- ADD
  s21_arbitrary_add(buffer_2, buffer_1, BUF_SIZE);

  int target_sign = buffer_1[BUF_SIZE - 1] >= 128;
  if (target_sign == S21_SIGN_NEGATIVE)
    s21_arbitrary_negate(buffer_1, BUF_SIZE);

  int target_exp = exp_1;
  uint8_t lower_bits = 0;

  // ----- CORRECT ROUNDING (very tedious)
  bool should_divide = false;
  bool should_round_up = false;
  do {
    memcpy(buffer_tmp, buffer_1, BUF_SIZE);
    should_round_up =
        buffer_1[0] > 128 ||  // Regular rounding   0.6 -> 1.0
        (buffer_1[0] == 128 &&
         lower_bits > 0) ||  // For edgecases, like 0.5000000001
        (buffer_1[0] == 128 && (buffer_1[1] & 1) == 1);  // For 0.5
    if (should_round_up) s21_arbitrary_add_val(1, &buffer_tmp[1], BUF_SIZE);

    should_divide = s21_has_excessive_bits(buffer_tmp, BUF_SIZE, 1) ||
                    target_exp > S21_MAX_EXPONENT;

    if (should_divide) {
      if (target_exp == 0) {
        error = target_sign ? 2 : 1;
      } else {
        lower_bits |= buffer_1[0];

        int div_err =
            s21_arbitrary_div(buffer_1, ten, BUF_SIZE, buffer_tmp, BUF_SIZE);
        s21_assert_no_error(div_err,
                            "s21_add division success assertion failed");

        memcpy(buffer_1, buffer_tmp, BUF_SIZE);
        target_exp--;
      }
    }
  } while (should_divide && error == 0);

  // ----- FINAL ROUNDING
  should_round_up = buffer_1[0] > 128 ||  // Regular rounding   0.6 -> 1.0
                    (buffer_1[0] == 128 &&
                     lower_bits > 0) ||  // For edgecases, like 0.5000000001
                    (buffer_1[0] == 128 && (buffer_1[1] & 1) == 1);  // For 0.5
  if (should_round_up) s21_arbitrary_add_val(1, &buffer_1[1], BUF_SIZE);

  // ----- SAVE RESULTS
  memcpy(result->bits, &buffer_1[1], S21_MANTISSA_BYTE_SIZE);
  s21_set_exponent(result, target_exp);
  s21_set_sign(result, target_sign);

  return error;
}

s21_error s21_sub(s21_decimal value_1, s21_decimal value_2,
                  s21_decimal *result) {
  (*result) = s21_default_decimal(0, 0);
  s21_negate(value_2, &value_2);
  return s21_add(value_1, value_2, result);
}

s21_error s21_negate(s21_decimal value, s21_decimal *result) {
  (*result) = value;
  s21_set_sign(result, !s21_get_sign_bit(&value));
  return 0;
}
