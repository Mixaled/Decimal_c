#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "../s21_arbitrary_math.h"
#include "../s21_decimal.h"

#define ERR_PANIC -1
#define ERR_DIV_TOO_LARGE 1
#define ERR_DIV_TOO_SMALL 2
#define ERR_DIV_BY_ZERO 3

#define BUF_SIZE (S21_MANTISSA_BYTE_SIZE * 2)

// Private function
void _s21_divide_fractional(uint8_t mantissa_1[], uint8_t mantissa_2[],
                            uint8_t into[BUF_SIZE]) {
  // Just divide two integer mantissas.
  uint8_t tmp_buffer_1[BUF_SIZE] = {0};
  uint8_t tmp_buffer_2[BUF_SIZE] = {0};
  memset(tmp_buffer_1, 0, BUF_SIZE);
  memset(tmp_buffer_2, 0, BUF_SIZE);

  // In (a / b) `a` is shifted left (up) exactly by one word width.
  // Word = mantissa.
  // This is done, so we do not lose fractional part of division.
  memcpy(tmp_buffer_1 + S21_MANTISSA_BYTE_SIZE, mantissa_1,
         S21_MANTISSA_BYTE_SIZE);
  memcpy(tmp_buffer_2, mantissa_2, S21_MANTISSA_BYTE_SIZE);

  // Inside result (`into`) first word is fractional part, and second word is
  // whole part. NOTE: we are using big endian notation.
  int div_err =
      s21_arbitrary_div(tmp_buffer_1, tmp_buffer_2, BUF_SIZE, into, BUF_SIZE);
  s21_assert_no_error(
      div_err, "_s21_divide_fractional division success assertion failed");
}

s21_error s21_div(s21_decimal value_1, s21_decimal value_2,
                  s21_decimal *result) {
  s21_error error = 0;
  (*result) = s21_default_decimal(0, 0);

  if (s21_arbitrary_is_zero((uint8_t *)value_2.bits, S21_MANTISSA_BYTE_SIZE)) {
    error = ERR_DIV_BY_ZERO;
  } else {
    int exp1 = (int)s21_get_exponent(&value_1);
    int exp2 = (int)s21_get_exponent(&value_2);
    int target_sign = s21_get_sign_bit(&value_1) ^ s21_get_sign_bit(&value_2);
    int exponent = exp1 - exp2;

    uint8_t result_buffer[BUF_SIZE] = {0};
    _s21_divide_fractional((uint8_t *)value_1.bits, (uint8_t *)value_2.bits,
                           result_buffer);

    error = s21_fit_in_mantissa(result_buffer, BUF_SIZE, S21_MANTISSA_BYTE_SIZE,
                                &exponent);

    if (error) {
      if (target_sign == S21_SIGN_POSITIVE)
        error = ERR_DIV_TOO_LARGE;
      else if (target_sign == S21_SIGN_NEGATIVE)
        error = ERR_DIV_TOO_SMALL;
    } else {
      memcpy(result->bits, result_buffer + S21_MANTISSA_BYTE_SIZE,
             S21_MANTISSA_BYTE_SIZE);
      s21_set_exponent(result, exponent);
      s21_set_sign(result, target_sign);
    }
  }

  return error;
}

// a % b = a - (b * int(a/b))
s21_error s21_mod(s21_decimal value_1, s21_decimal value_2,
                  s21_decimal *result) {
  (*result) = s21_default_decimal(0, 0);
  s21_error error;

  error = s21_div(value_1, value_2, result);
  if (!error) {
    error = s21_truncate(*result, result);
    s21_normalize(result);
    if (!error) {
      error = s21_mul(value_2, *result, result);

      if (!error) {
        error = s21_sub(value_1, *result, result);
      }
    }
  }

  return error;
}
