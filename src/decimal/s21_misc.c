#include <stdbool.h>
#include <stdint.h>
#include <string.h>

#include "../s21_arbitrary_math.h"
#include "../s21_decimal.h"

s21_decimal s21_default_decimal(int src, int exp) {
  int sign = 0;  // 0 == positive

  if (src < 0) {
    sign = 1;
    src = -src;
  }

  s21_decimal val = {
      .bits = {src, 0, 0, 0},
  };
  s21_set_exponent(&val, exp);
  s21_set_sign(&val, sign);

  return val;
}

int8_t s21_get_sign_bit(s21_decimal *x) { return ((x->bits[3]) >> 31) & 1; }

int8_t s21_get_exponent(s21_decimal *x) { return ((x->bits[3]) >> 16) & 0xFF; }

void s21_set_exponent(s21_decimal *x, int8_t exponent) {
  x->bits[3] = (x->bits[3] & 0xFF00FFFF) | (((int)exponent) << 16);
}

void s21_set_sign(s21_decimal *x, int8_t sign) {
  x->bits[3] = (x->bits[3] & 0x7FFFFFFF) | (((int)sign) << 31);
}

void s21_incr_exponent(s21_decimal *x, int8_t increment) {
  s21_set_exponent(x, s21_get_exponent(x) + increment);
}

void s21_decrease_prec(s21_decimal *x) {
  if (s21_get_exponent(x) > 0) {
    uint8_t ten[S21_MANTISSA_BYTE_SIZE] = {10};
    uint8_t buffer[S21_MANTISSA_BYTE_SIZE] = {0};

    s21_incr_exponent(x, -1);
    int div_err =
        s21_arbitrary_div((uint8_t *)x->bits, ten, S21_MANTISSA_BYTE_SIZE,
                          buffer, S21_MANTISSA_BYTE_SIZE);
    s21_assert_no_error(div_err,
                        "s21_decrease_prec division success assertion failed");
    memcpy((uint8_t *)x->bits, buffer, S21_MANTISSA_BYTE_SIZE);
  }
}

void s21_normalize(s21_decimal *value) {
  uint8_t ten[S21_MANTISSA_BYTE_SIZE] = {10};
  uint8_t buffer[S21_MANTISSA_BYTE_SIZE * 2] = {0};
  int buffer_size = S21_MANTISSA_BYTE_SIZE * 2;
  bool do_break = false;

  while (s21_get_exponent(value) < S21_MAX_EXPONENT && !do_break) {
    int mul_err =
        s21_arbitrary_mul((uint8_t *)value->bits, ten, S21_MANTISSA_BYTE_SIZE,
                          buffer, buffer_size);  // buffer = value * 10
    s21_assert_no_error(
        mul_err, "s21_normalize multiplication success assertion failed");

    if (buffer[S21_MANTISSA_BYTE_SIZE] == 0) {  // No overflow
      s21_incr_exponent(value, 1);              //  /= 10
      memcpy(value->bits, buffer, S21_MANTISSA_BYTE_SIZE);
    } else {  // Overflow
      do_break = true;
    }
  }
}

s21_error s21_floor(s21_decimal value, s21_decimal *result) {
  (*result) = s21_default_decimal(0, 0);
  s21_error error = 0;

  if (s21_get_exponent(&value) == 0) {
    (*result) = value;
  } else {
    s21_decimal tmp = s21_default_decimal(0, 0);
    s21_truncate(value, &tmp);
    s21_sub(value, tmp, &tmp);
    bool is_int = s21_is_equal(tmp, s21_default_decimal(0, 0));

    s21_truncate(value, result);
    if (s21_get_sign_bit(result) == S21_SIGN_NEGATIVE && !is_int) {
      s21_add(*result, s21_default_decimal(-1, 0), result);
    }
  }

  return error;
}

#define BUF_SIZE (S21_MANTISSA_BYTE_SIZE + 1)
s21_error s21_round(s21_decimal value, s21_decimal *result) {
  (*result) = s21_default_decimal(0, 0);

  int exp = s21_get_exponent(&value);
  uint8_t lower_bits = 0;
  uint8_t buffer_1[BUF_SIZE] = {0};
  uint8_t buffer_2[BUF_SIZE] = {0};
  uint8_t ten[BUF_SIZE] = {10};
  memcpy(&buffer_1[1], (uint8_t *)value.bits, S21_MANTISSA_BYTE_SIZE);

  while (exp > 0) {
    lower_bits |= buffer_1[0];
    int div_err =
        s21_arbitrary_div(buffer_1, ten, BUF_SIZE, buffer_2, BUF_SIZE);
    s21_assert_no_error(div_err, "s21_round division success assertion failed");

    memcpy(buffer_1, buffer_2, BUF_SIZE);
    exp--;
  }

  // Rounding check
  bool should_round_up = buffer_1[0] > 128 ||
                         (buffer_1[0] == 128 && (lower_bits > 0)) ||
                         (buffer_1[0] == 128 && (buffer_1[1] & 1) == 1);

  int error = 0;
  // Round number
  if (should_round_up)
    error = s21_arbitrary_add_val(1, &buffer_1[1], BUF_SIZE - 1);

  memcpy(result->bits, &buffer_1[1], S21_MANTISSA_BYTE_SIZE);
  s21_set_sign(result, s21_get_sign_bit(&value));
  s21_set_exponent(result, exp);
  return error;
}

s21_error s21_truncate(s21_decimal value, s21_decimal *result) {
  (*result) = value;

  while (s21_get_exponent(result) > 0) s21_decrease_prec(result);

  return 0;
}
