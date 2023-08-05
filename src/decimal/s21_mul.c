#include <stdbool.h>
#include <stdint.h>
#include <string.h>

#include "../s21_arbitrary_math.h"
#include "../s21_decimal.h"

#define ERR_MUL_TOO_LARGE 1
#define ERR_MUL_TOO_SMALL 2

#define BUF_SIZE (S21_MANTISSA_BYTE_SIZE * 2)

// Public function
s21_error s21_mul(s21_decimal value_1, s21_decimal value_2,
                  s21_decimal *result) {
  (*result) = s21_default_decimal(0, 0);
  uint8_t tmp_buffer[BUF_SIZE] = {0};
  s21_error error = 0;

  // Actual multiplication
  int target_sign = s21_get_sign_bit(&value_1) ^ s21_get_sign_bit(&value_2);
  int exponent = s21_get_exponent(&value_1) + s21_get_exponent(&value_2);

  int mul_err =
      s21_arbitrary_mul((uint8_t *)value_1.bits, (uint8_t *)value_2.bits,
                        S21_MANTISSA_BYTE_SIZE, tmp_buffer, BUF_SIZE);
  s21_assert_no_error(mul_err,
                      "s21_mul multiplication success assertion failed");

  // Number after multiplication has twice the bits of original.
  // We need to get rid of extra bits
  error = s21_fit_in_mantissa(tmp_buffer, BUF_SIZE, /*fractional_size*/ 0,
                              &exponent);

  // Write results out
  memcpy(result->bits, tmp_buffer, S21_MANTISSA_BYTE_SIZE);
  s21_set_exponent(result, exponent);
  s21_set_sign(result, target_sign);

  // Map error
  if (error) {
    if (target_sign == S21_SIGN_POSITIVE)
      error = ERR_MUL_TOO_LARGE;
    else if (target_sign == S21_SIGN_NEGATIVE)
      error = ERR_MUL_TOO_SMALL;
  }

  return error;
}
