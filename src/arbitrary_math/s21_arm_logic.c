#include "../s21_arbitrary_math.h"

void s21_arbitrary_not(uint8_t val[], int size) {
  for (int i = 0; i < size; i++) val[i] = ~val[i];
}

void s21_arbitrary_and(uint8_t val[], uint8_t into[], int size) {
  for (int i = 0; i < size; i++) into[i] = val[i] & into[i];
}

void s21_arbitrary_or(uint8_t val[], uint8_t into[], int size) {
  for (int i = 0; i < size; i++) into[i] = val[i] | into[i];
}

void s21_arbitrary_xor(uint8_t val[], uint8_t into[], int size) {
  for (int i = 0; i < size; i++) into[i] = val[i] ^ into[i];
}
