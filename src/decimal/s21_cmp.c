#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

#include "../s21_arbitrary_math.h"
#include "../s21_decimal.h"

#define BUF_SIZE S21_MANTISSA_BYTE_SIZE

int s21_get_digit(uint32_t bindec[MAX_DIGITS], int digit_id) {
  int digit = 0;

  if (digit_id < 0 || digit_id >= MAX_DIGITS)
    digit = 0;
  else
    digit = bindec[digit_id];

  return digit;
}

struct s21_compare {
  bool is_less;
  bool is_bigger;
};

struct s21_compare s21_is_less_or_bigger(s21_decimal a, s21_decimal b) {
  uint32_t a_base_10[MAX_DIGITS] = {0}, b_base_10[MAX_DIGITS] = {0};

  s21_convert_to_digits(a, a_base_10);
  s21_convert_to_digits(b, b_base_10);

  int sign_a = s21_get_sign_bit(&a), sign_b = s21_get_sign_bit(&b),
      exp1 = s21_get_exponent(&a), exp2 = s21_get_exponent(&b),
      pos_a = MAX_DIGITS + exp1, pos_b = MAX_DIGITS + exp2;

  struct s21_compare result = {false, false};
  bool do_break = false;

  if (sign_a != sign_b) {
    bool is_zero =
        s21_arbitrary_is_zero((uint8_t*)a.bits, S21_MANTISSA_BYTE_SIZE) &&
        s21_arbitrary_is_zero((uint8_t*)b.bits, S21_MANTISSA_BYTE_SIZE);

    result.is_less = sign_a == S21_SIGN_NEGATIVE && !is_zero;
    result.is_bigger = sign_a == S21_SIGN_POSITIVE && !is_zero;
  } else {
    while ((pos_a >= 0 || pos_b >= 0) && !do_break) {
      uint32_t digit_a = s21_get_digit(a_base_10, pos_a),
               digit_b = s21_get_digit(b_base_10, pos_b);

      if (digit_a < digit_b) {
        result.is_less = true;
        result.is_bigger = false;
        do_break = true;
      } else if (digit_a > digit_b) {
        result.is_less = false;
        result.is_bigger = true;
        do_break = true;
      }

      pos_a--;
      pos_b--;
    }
  }
  if (sign_a == S21_SIGN_NEGATIVE && sign_b == S21_SIGN_NEGATIVE) {
    bool tmp = result.is_less;
    result.is_less = result.is_bigger;
    result.is_bigger = tmp;
  }
  return result;
}

s21_bool s21_is_less(s21_decimal a, s21_decimal b) {
  struct s21_compare result = s21_is_less_or_bigger(a, b);
  return result.is_less;
}

s21_bool s21_is_greater(s21_decimal a, s21_decimal b) {
  struct s21_compare result = s21_is_less_or_bigger(a, b);
  return result.is_bigger;
}

s21_bool s21_is_equal(s21_decimal a, s21_decimal b) {
  struct s21_compare result = s21_is_less_or_bigger(a, b);
  bool b_res = false;
  if (result.is_bigger == false && result.is_less == false) b_res = true;
  return b_res;
}

s21_bool s21_is_less_or_equal(s21_decimal a, s21_decimal b) {
  struct s21_compare result = s21_is_less_or_bigger(a, b);
  bool b_res = false;
  if ((result.is_bigger == false && result.is_less == false) ||
      result.is_less == true)
    b_res = true;
  return b_res;
}

s21_bool s21_is_greater_or_equal(s21_decimal a, s21_decimal b) {
  struct s21_compare result = s21_is_less_or_bigger(a, b);
  bool b_res = false;
  if ((result.is_bigger == false && result.is_less == false) ||
      result.is_bigger == true)
    b_res = true;
  return b_res;
}

s21_bool s21_is_not_equal(s21_decimal a, s21_decimal b) {
  struct s21_compare result = s21_is_less_or_bigger(a, b);
  return (result.is_less || result.is_bigger);
}
