#ifndef SRC_S21_DECIMAL_H_
#define SRC_S21_DECIMAL_H_

#include <stdint.h>

#define S21_MANTISSA_BYTE_SIZE (3 * sizeof(int))
#define S21_MAX_EXPONENT 28

#define S21_SIGN_POSITIVE 0
#define S21_SIGN_NEGATIVE 1

#define MAX_DIGITS 32

typedef struct {
  int bits[4];
} s21_decimal;

// 0 - OK, other values are error codes.
typedef int s21_error;

// There is stdbool, but we need to ensure that this bool will be exactly int.
typedef int s21_bool;

s21_decimal s21_default_decimal(int val, int exp);

int8_t s21_get_sign_bit(s21_decimal *x);
int8_t s21_get_exponent(s21_decimal *x);

void s21_incr_exponent(s21_decimal *x, int8_t increment);
void s21_set_exponent(s21_decimal *x, int8_t exponent);
void s21_set_sign(s21_decimal *x, int8_t sign);

void s21_print(s21_decimal val);
void s21_convert_to_digits(s21_decimal val, uint32_t base_10[MAX_DIGITS]);

s21_error s21_fit_in_mantissa(uint8_t number[], int total_size,
                              int fractional_size, int *exponent);
s21_bool s21_has_excessive_bits(uint8_t number[], int total_size,
                                int fractional_size);

void s21_decrease_prec(s21_decimal *x);

// ======

void s21_normalize(s21_decimal *value);  // done

s21_error s21_add(s21_decimal value_1, s21_decimal value_2,
                  s21_decimal *result);  // done
s21_error s21_sub(s21_decimal value_1, s21_decimal value_2,
                  s21_decimal *result);  // done
s21_error s21_mul(s21_decimal value_1, s21_decimal value_2,
                  s21_decimal *result);  // done
s21_error s21_div(s21_decimal value_1, s21_decimal value_2,
                  s21_decimal *result);  // done
s21_error s21_mod(s21_decimal value_1, s21_decimal value_2,
                  s21_decimal *result);  // done

s21_bool s21_is_less(s21_decimal, s21_decimal);              // done
s21_bool s21_is_less_or_equal(s21_decimal, s21_decimal);     // done
s21_bool s21_is_greater(s21_decimal, s21_decimal);           // done
s21_bool s21_is_greater_or_equal(s21_decimal, s21_decimal);  // done
s21_bool s21_is_equal(s21_decimal, s21_decimal);             // done
s21_bool s21_is_not_equal(s21_decimal, s21_decimal);         // done

s21_error s21_from_int_to_decimal(int src, s21_decimal *dst);      // done
s21_error s21_from_float_to_decimal(float src, s21_decimal *dst);  // done
s21_error s21_from_decimal_to_int(s21_decimal src, int *dst);      // done
s21_error s21_from_decimal_to_float(s21_decimal src, float *dst);

s21_error s21_floor(s21_decimal value, s21_decimal *result);     // done
s21_error s21_round(s21_decimal value, s21_decimal *result);     // done
s21_error s21_truncate(s21_decimal value, s21_decimal *result);  // done
s21_error s21_negate(s21_decimal value, s21_decimal *result);    // done

#endif
