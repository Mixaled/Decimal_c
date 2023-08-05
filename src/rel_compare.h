#ifndef SRC_S21_REL_COMPARE_H_
#define SRC_S21_REL_COMPARE_H_

#include <float.h>
#include <math.h>

#include "s21_decimal.h"

int relatively_equal(float a, float b) {
  int result = 0;
  if (isnan(a) && isnan(b)) {
    result = 1;
  } else if (isinf(a) && isinf(b)) {
    result = (a > 0.0 && b > 0.0) || (a < 0.0 && b < 0.0);
  } else {
    const float difference = fabs(a - b);

    // Scale to the largest value.
    a = fabs(a);
    b = fabs(b);
    const float scaledEpsilon = 10.0 * FLT_EPSILON * fmax(a, b);
    result = difference <= scaledEpsilon;
  }

  return result;
}

int relatively_equal_decimal(s21_decimal a, s21_decimal b) {
  s21_decimal difference;
  s21_sub(a, b, &difference);
  if (s21_get_sign_bit(&difference) == S21_SIGN_NEGATIVE)
    s21_negate(difference, &difference);

  // Scale to the largest value.
  if (s21_get_sign_bit(&a) == S21_SIGN_NEGATIVE) s21_negate(a, &a);
  if (s21_get_sign_bit(&b) == S21_SIGN_NEGATIVE) s21_negate(b, &b);

  s21_decimal scaledEpsilon;
  s21_from_float_to_decimal(0.000001, &scaledEpsilon);
  if (s21_is_greater(a, b))
    s21_mul(scaledEpsilon, a, &scaledEpsilon);
  else
    s21_mul(scaledEpsilon, b, &scaledEpsilon);

  return s21_is_less_or_equal(difference, scaledEpsilon);
}

int min(int a, int b) { return a < b ? a : b; }

int relatively_equal_decimal_2(s21_decimal a, s21_decimal b) {
  s21_decimal difference;
  s21_sub(a, b, &difference);
  if (s21_get_sign_bit(&difference) == S21_SIGN_NEGATIVE)
    s21_negate(difference, &difference);

  int epsilon_exp = min(s21_get_exponent(&a), s21_get_exponent(&b));
  s21_decimal epsilon = s21_default_decimal(100, min(epsilon_exp, 3));

  return s21_is_less_or_equal(difference, epsilon);
}

#endif