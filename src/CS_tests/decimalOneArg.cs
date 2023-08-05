using System;
using MyNamespace;
using DecimalFunctionsExample;
using System.Globalization;
using System.IO;
using System.Collections.Generic;

namespace MyFloor
{
    public class DecimalOneArg
    {
        public static void GenOneArgFunctions(decimal[] values, StreamWriter writer, List<string> all_suites) {
            var functions = new List<(string, Func<decimal, decimal>)>(new (string, Func<decimal, decimal>)[]{
                ("round",    a => decimal.Round(a)),
                ("floor",    a => decimal.Floor(a)),
                ("negate",   a => decimal.Negate(a)),
                ("truncate", a => decimal.Truncate(a)),
            });

            foreach (var fn in functions) {
                List<string> all_cases = new List<string>();

                GenOneArgTest(fn.Item1, fn.Item2, values, all_cases, writer);
                all_suites.Add(Program.CreateSuite(fn.Item1, all_cases.ToArray(), writer));
                Console.WriteLine("Created " + fn.Item1);
            }
        }

        public static void GenOneArgTest(
            string func_name, Func<decimal, decimal> function,
            decimal[] values, List<string> all_cases, StreamWriter writer
        ) {
            int count = 0;
            foreach (decimal a in values) {
                bool error = false;
                decimal result = 0m;
                try {
                    result = function(a);
                } catch (Exception) {
                    error = true;
                }

                string name = String.Format("test_{0}_{1}", func_name, count);
                all_cases.Add(name);
                //string err_msg = String.Format("\"Fail test {0}: {1} ({2}) = . Must be {4}\"", count, func_name, a, result, result);
                string format = @"
START_TEST({0})
{{
    s21_decimal a = {1};
    s21_decimal result;
    s21_decimal must_be = {2};
    int error = s21_{3}(a, &result);

    if (error != {4}) {{
        ck_abort_msg(""Error code differs - returned %d but must be %d - {3} ({6}) must be {8}"", error, {4});
    }} else if (error == 0 && s21_is_equal(must_be, result) == 0) {{
        ck_abort_msg(""Fail test {5}: {3} ({6}) must be {7}"");
    }}
}}
END_TEST
";
                string text = string.Format(
                    format, 
                    name,
                    DecimalArray.DecimalToS21Initializer(a),
                    DecimalArray.DecimalToS21Initializer(result),
                    func_name,
                    error ? 1 : 0,
                    count, a, result,
                    error ? "Err" : result.ToString()
                );
                writer.Write(text);

                count++;
            }
        }
    }
}