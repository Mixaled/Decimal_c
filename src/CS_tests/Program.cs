using System;
using MyNamespace;
using MyFloor;
using MyCompare;
using MyArithmetic;
using MyConvert;
using System.Collections.Generic;
using System.IO;

namespace DecimalFunctionsExample
{

    class Program
    {
        static void Main(string[] args)
        {
            decimal[] values = DecimalArray.GetDecimalArray();
            decimal[] extra_values = DecimalArray.GetDecimalArrayExtra();
            int[] int_values = DecimalArray.GetIntArray();
            float[] float_values = DecimalArray.GetFloatArray();

            using (StreamWriter writer = new StreamWriter("test.c")) {
                Console.WriteLine("Generating tests...");
                writer.WriteLine("#include <check.h>");
                writer.WriteLine("#include <stdio.h>");
                writer.WriteLine("#include <math.h>");
                writer.WriteLine("#include \"s21_decimal.h\"\n");
                writer.WriteLine("#include \"rel_compare.h\"\n\n");
                
                List<string> all_suites = new List<string>();
                DecimalArithmetic.GenArithmetics(values, writer, all_suites);
                DecimalCompare.GenComparsions(values, writer, all_suites);
                DecimalOneArg.GenOneArgFunctions(extra_values, writer, all_suites);
                DecimalConvert.GenConvertTests(extra_values, float_values, int_values, writer, all_suites);

                CreateMain(all_suites.ToArray(), writer);
            }
            Console.WriteLine("Wrote");
        }
        
        public static void CreateMain(string[] suites, StreamWriter writer) {
            const string FORMAT = @"
#define SUITES {{ {0} }}

typedef Suite* (*SuiteFn)();
int main() {{
  printf(""s21_print example:"");
  s21_print(s21_default_decimal(-12, 3));
  printf(""\n"");
  s21_print(s21_default_decimal(101234, 0));
  printf(""\n"");

  int total_failed = 0;
  SuiteFn suites[] = SUITES;

  for (int i = 0; i < {1}; i++) {{
    SRunner* sr = srunner_create((suites[i])());
    srunner_run_all(sr, CK_NORMAL);
    total_failed += srunner_ntests_failed(sr);
    srunner_free(sr);
  }}

  printf(""========= FAILED: %d =========\n"", total_failed);
  return (total_failed == 0) ? 0 : 1;
}}";
            writer.Write(String.Format(FORMAT, string.Join(", ", suites), suites.Length));
        }

        // Suite* XXX_suite()
        public static string CreateSuite(string name, string[] cases, StreamWriter writer) {
            const string FORMAT = @"
#define ALL_CASES_{0} {{ {1} }}

Suite* {0}_suite() {{
    Suite* s = suite_create("" \033[45m -- {0} -- \033[0m "");
    TCase* tc = tcase_create(""Core"");
    const void* cases[] = ALL_CASES_{0};

    for (int i = 0; i < {2}; i++)
        tcase_add_test(tc, cases[i]);

    suite_add_tcase(s, tc);
    return s;
}}
";
            writer.Write(String.Format(FORMAT, name, string.Join(", ", cases), cases.Length.ToString()));
            return String.Format("{0}_suite", name);
        }
        
    }
}
