#include <ctype.h>
#include <math.h>
#include <stdbool.h>
#include <stdint.h>
#include <string.h>

#include "../s21_arbitrary_math.h"
#include "../s21_decimal.h"

int s21_from_int_to_decimal(int src, s21_decimal *dst) {
  unsigned int sign = 0;  // 0 == positive

  if (src < 0) {
    sign = 1;
    src = -src;
  }

  dst->bits[0] = src;
  dst->bits[1] = 0;
  dst->bits[2] = 0;
  dst->bits[3] = sign << 31;

  s21_normalize(dst);
  return 0;
}

#define BUF_SIZE (S21_MANTISSA_BYTE_SIZE + 1)
s21_error s21_from_decimal_to_int(s21_decimal src, int *dst) {
  (*dst) = 0;
  int exp = s21_get_exponent(&src);

  uint8_t ten[BUF_SIZE] = {10};
  uint8_t buffer_1[BUF_SIZE] = {0};
  uint8_t buffer_2[BUF_SIZE] = {0};

  memcpy(&buffer_1[1], src.bits, S21_MANTISSA_BYTE_SIZE);
  int error = 0;

  uint8_t lower_bits = 0;
  for (int i = 0; i < exp; i++) {
    lower_bits |= buffer_1[0];

    int div_err =
        s21_arbitrary_div(buffer_1, ten, BUF_SIZE, buffer_2, BUF_SIZE);
    s21_assert_no_error(
        div_err, "s21_from_decimal_to_int division success assertion failed");

    memcpy(buffer_1, buffer_2, BUF_SIZE);
  }

  bool is_negative = s21_get_sign_bit(&src) == S21_SIGN_NEGATIVE;
  memcpy(src.bits, &buffer_1[1], S21_MANTISSA_BYTE_SIZE);

  if (buffer_1[0] > 128 || (buffer_1[0] == 128 && lower_bits > 0))
    s21_arbitrary_add_val(1, (uint8_t *)src.bits, S21_MANTISSA_BYTE_SIZE);

  if (src.bits[0] >= 0 && src.bits[1] == 0 && src.bits[2] == 0) {
    if (is_negative)
      (*dst) = -src.bits[0];
    else
      (*dst) = src.bits[0];
  } else {
    error = 1;
  }
  return error;
}

#define FLOAT_VALUE_SIZE_BITS 23
int32_t s21_get_float_mantissa(float x) {
  union {
    float f;
    int i;
  } u;
  u.f = x;
  u.i &= 0x007fffff;
  return u.i;
}

int32_t s21_get_float_exponent(float x) {
  union {
    float f;
    int i;
  } u;
  u.f = x;
  return ((u.i >> 23) & 0xff) - 0x7f;
}

s21_error s21_from_float_to_decimal(float src, s21_decimal *dst) {
  s21_error error = 0;
  (*dst) = s21_default_decimal(0, 0);
  if (isnan(src) || isinf(src)) {
    error = 1;
  } else {
    int sign = 0;
    if (src < 0) {
      sign = 1;
      src = -src;
    }

    int32_t exponent = s21_get_float_exponent(src) - FLOAT_VALUE_SIZE_BITS;
    int32_t significant =
        s21_get_float_mantissa(src) | (1 << FLOAT_VALUE_SIZE_BITS);

    s21_decimal result = s21_default_decimal(significant, 0);
    s21_decimal two = s21_default_decimal(2, 0);

    for (; exponent > 0 && !error; exponent--) {
      error = s21_mul(result, two, &result);
    }

    for (; exponent < 0 && !error; exponent++) {
      error = s21_div(result, two, &result);
    }

    s21_set_sign(&result, sign);
    (*dst) = result;
  }
  return error ? 1 : 0;
}

s21_error s21_from_decimal_to_float(s21_decimal src, float *dst) {
  (*dst) = 0.0;
  float result = 0.0;

  for (int i = 0; i < 3; i++) {
    unsigned int data = src.bits[i];
    result += ((float)data) * (powf(2.0, i * sizeof(int) * 8));
  }

  result /= powf(10, s21_get_exponent(&src));
  if (s21_get_sign_bit(&src) == S21_SIGN_NEGATIVE) result = -result;

  (*dst) = result;
  return 0;
}
