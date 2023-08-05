#include <float.h>
#include <math.h>
#include <stdio.h>

#include "s21_decimal.h"

int relativelyEqual(float a, float b) {
  const float difference = fabs(a - b);

  // Scale to the largest value.
  a = fabs(a);
  b = fabs(b);
  const float scaledEpsilon = 10.0 * FLT_EPSILON * fmax(a, b);

  return difference <= scaledEpsilon;
}

int main() {
  s21_decimal a, b, result, correct_result;
  a.bits[0] = 123456;  // 123456
  a.bits[1] = 0;
  a.bits[2] = 0;
  a.bits[3] = 0;
  b.bits[0] = 1600699885;  // -789.10111213
  b.bits[1] = 18;
  b.bits[2] = 0;
  b.bits[3] = 2148007936;
  correct_result.bits[0] = 746063767;
  correct_result.bits[1] = 1218036038;
  correct_result.bits[2] = 848124919;
  correct_result.bits[3] = 2149187584;

  int error = s21_div(a, b, &result);

  s21_print(a);
  printf("\n");
  s21_print(b);
  printf("\n Result (%d):\n", error);
  s21_print(result);
  printf("\n Must be (0):\n");
  s21_print(correct_result);
  printf("\n");
  // s21_decimal diff;
  // s21_sub(result, must_be, &diff);
  // printf("Diff:\n");
  // s21_print(diff);
  printf("\n");
}