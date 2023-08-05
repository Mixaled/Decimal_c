GCC= gcc -Wall -Wextra
GCOV_FLAGS= -fprofile-arcs -ftest-coverage

UNAME:=$(shell uname -s)
ifeq ($(UNAME),Linux)
	LINK_FLAGS=-lcheck -lsubunit -lrt -lm -lpthread
	LEAK=CK_FORK=no valgrind --leak-check=full --track-origins=yes --show-leak-kinds=all --log-file="leak.log"
	SYSFLAG=-D LINUX
endif
ifeq ($(UNAME),Darwin)
	LINK_FLAGS=-lcheck -lm
	LEAK=CK_FORK=no leaks -quiet -atExit --
	SYSFLAG=-D APPLE
endif


DEC_C_SOURCES=decimal/s21_print.c decimal/s21_misc.c decimal/s21_convert.c decimal/s21_basic_math.c decimal/s21_mul.c decimal/s21_div.c decimal/s21_cmp.c decimal/s21_fit.c
DEC_H_SOURCES=s21_decimal.h decimal/s21_bits_bindec_data.h

ARM_C_SOURCES=arbitrary_math/s21_arm_base.c arbitrary_math/s21_arm_logic.c arbitrary_math/s21_arm_cmp.c arbitrary_math/s21_arm_shift.c arbitrary_math/s21_arm_math.c
ARM_H_SOURCES=s21_arbitrary_math.h

ALL_SOURCES=${DEC_C_SOURCES} ${DEC_H_SOURCES} ${ARM_C_SOURCES} ${ARM_H_SOURCES}

all: style s21_decimal.a test

s21_decimal.a: ${DEC_C_SOURCES} ${DEC_H_SOURCES} s21_arbitrary_math.a
	rm -f *.o
	$(GCC) $(SYSFLAG) -c ${DEC_C_SOURCES}
	ar x s21_arbitrary_math.a
	ar r s21_decimal.a *.o
	ranlib s21_decimal.a
	rm -f *.o

s21_arbitrary_math.a: ${ARM_C_SOURCES} ${ARM_H_SOURCES}
	rm -f *.o
	$(GCC) $(SYSFLAG) -c ${ARM_C_SOURCES}
	ar r s21_arbitrary_math.a *.o
	ranlib s21_arbitrary_math.a
	rm -f *.o

clean:
	rm -rf *.a
	rm -rf *.o
	rm -rf *.out
	rm -f *.gcda
	rm -f *.gcno
	rm -f test.log
	rm -f test.info
	rm -rf report/
	rm -f .clang-format
	rm -rf __.SYMDEF\ SORTED
	rm -f new_test.c
	rm -rf CS_tests/bin
	rm -rf CS_tests/obj
	rm -f test_gcov
	rm -f tmp_keep/*.o
	rm -f .DS_Store
	rm -f ../.DS_Store

tmp_keep/test.o: test.c rel_compare.h
	$(GCC) $(SYSFLAG) -c test.c -o tmp_keep/test.o

test.out: tmp_keep/test.o s21_decimal.a
	$(GCC) $(SYSFLAG) tmp_keep/test.o s21_decimal.a  -L. $(LINK_FLAGS) -o test.out

test: test.out
	./test.out

test_gcov: tmp_keep/test.o ${DEC_C_SOURCES} ${DEC_H_SOURCES} s21_arbitrary_math.a
	$(GCC) $(GCOV_FLAGS) $(SYSFLAG) ${DEC_C_SOURCES} s21_arbitrary_math.a tmp_keep/test.o -L. $(LINK_FLAGS) -o test_gcov

regen_test: CS_tests/* .clang-format
	dotnet run --project CS_tests
	clang-format -i test.c

.clang-format:
	clang-format -style=Google -dump-config > .clang-format

style:
	clang-format -n -style=Google *.c *.h decimal/* arbitrary_math/*

style_force: .clang-format
	clang-format -i *.c *.h decimal/* arbitrary_math/*

gcov_report: test_gcov
	./test_gcov
	lcov -t "test" -o test.info -c -d .
	genhtml -o report test.info

playground.out: playground.c ${ALL_SOURCES} s21_decimal.a
	$(GCC) $(SYSFLAG) playground.c s21_decimal.a  -L. $(LINK_FLAGS) -o playground.out

playground: playground.out
	./playground.out

leaks: test.out
	$(LEAK) ./test.out

cppcheck:
	cppcheck *.c *.h decimal/*.c decimal/*.h arbitrary_math/*.c arbitrary_math/*.h