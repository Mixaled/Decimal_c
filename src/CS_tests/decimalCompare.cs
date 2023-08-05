using System;
using MyNamespace;
using DecimalFunctionsExample;
using System.Globalization;
using System.Collections.Generic;
using System.IO;

namespace MyCompare
{
    public class DecimalCompare
    {
        public static void GenComparsions(decimal[] values, StreamWriter writer, List<string> all_suites) {
            var comparsions = new List<(string, Func<decimal, decimal, bool>)>(new (string, Func<decimal, decimal, bool>)[]{
                ("is_less",                 (a, b) => a < b),
                ("is_greater",              (a, b) => a > b),
                ("is_equal",                (a, b) => a == b),
                ("is_less_or_equal",        (a, b) => a <= b),
                ("is_greater_or_equal",     (a, b) => a >= b),
                ("is_not_equal",            (a, b) => a != b)
            });

            foreach (var cmp in comparsions) {
                List<string> all_cases = new List<string>();
                DecimalCompare.GenTestCmp(cmp.Item1, cmp.Item2, values, all_cases, writer);
                all_suites.Add(Program.CreateSuite(cmp.Item1, all_cases.ToArray(), writer));
                Console.WriteLine("Created " + cmp.Item1);
            }
        }

        public static void GenTestCmp(
            string cmp_name, Func<decimal, decimal, bool> comparsion,
            decimal[] values, List<string> all_cases, StreamWriter writer)
        {
            int count = 0;
            foreach (decimal a in values) {
                foreach (decimal b in values) {
                    bool result = comparsion(a, b);

                    string name = String.Format("test_{0}_{1}", cmp_name, count);
                    all_cases.Add(name);
                    string err_msg = String.Format("\"Fail test {0}: {1} {2} {3}. Must be {4}\"", count, a, cmp_name, b, result);
                    string format = @"
START_TEST({0})
{{
    s21_decimal a = {1};
    s21_decimal b = {2}; 
    
    if (s21_{5}(a, b) != {3})
        ck_abort_msg({4});
}}
END_TEST
";
                    string text = string.Format(
                        format, 
                        name, 
                        DecimalArray.DecimalToS21Initializer(a), 
                        DecimalArray.DecimalToS21Initializer(b), 
                        result ? 1 : 0, 
                        err_msg,
                        cmp_name
                    );
                    writer.Write(text);

                    count++;
                }
            }
        }
    }
}