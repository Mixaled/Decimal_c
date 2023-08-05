using System;
using MyNamespace;
using DecimalFunctionsExample;
using System.Globalization;
using System.IO;
using System.Collections.Generic;

namespace MyArithmetic
{
    public class DecimalArithmetic
    {
        public static void GenArithmetics(decimal[] values, StreamWriter writer, List<string> all_suites) {
            var functions = new List<(string, Func<decimal, decimal, decimal>)>(new (string, Func<decimal, decimal, decimal>)[]{
                ("add", (a, b) => a + b),
                ("sub", (a, b) => a - b),
                ("mul", (a, b) => a * b),
                ("div", (a, b) => a / b),
                ("mod", (a, b) => a % b)
            });

            foreach (var fn in functions) {
                List<string> all_cases = new List<string>();
                GenTestArithmetics(fn.Item1, fn.Item2, values, all_cases, writer);
                all_suites.Add(Program.CreateSuite(fn.Item1, all_cases.ToArray(), writer));
                Console.WriteLine("Created " + fn.Item1);
            }
        }

        public static void GenTestArithmetics(
            string fn_name, Func<decimal, decimal, decimal> function,
            decimal[] values, List<string> all_cases, StreamWriter writer)
        {
            int count = 0;
            foreach (decimal a in values) {
                foreach (decimal b in values) {
                    if (fn_name == "mod" && (Math.Abs(a) >= decimal.MaxValue / 4 || Math.Abs(b) >= decimal.MaxValue / 4))
                        continue;

                    int error = 0;
                    decimal result = 0m;
                    try {
                        result = function(a, b);
                    } catch (System.OverflowException) {
                        error = 1;
                    } catch (System.DivideByZeroException) {
                        error = 3;
                    }

                    string name = String.Format("test_{0}_{1}", fn_name, count);
                    all_cases.Add(name);

                    string err_msg = String.Format("\"Fail test {0}: {1} {2} {3}. Must be {4}\"", count, a, fn_name, b, result);
                    string eq_function = (fn_name == "div" || fn_name == "mod") ? "relatively_equal_decimal_2" : "s21_is_equal";
                    string text = @$"
START_TEST({name})
{{
    s21_decimal a = {DecimalArray.DecimalToS21Initializer(a)};
    s21_decimal b = {DecimalArray.DecimalToS21Initializer(b)}; 
    s21_decimal result;
    s21_decimal must_be = {DecimalArray.DecimalToS21Initializer(result)};

    int error = s21_{fn_name}(a, b, &result);
    
    if (error != {error} && !((error == 1 || error == 2) && ({error} == 1 || {error} == 2))) {{
        ck_abort_msg(""Error code differs - returned %d but must be {error} - {fn_name} ({a}, {b}) must be {result}"", error);
    }} else if (error == 0 && {eq_function}(must_be, result) == 0) {{
        ck_abort_msg(""Fail test {count}: {fn_name} ({a}, {b}) must be {result}"");
    }}
}}
END_TEST
";
                    writer.Write(text);

                    count++;
                }
            }
        }

    }
}
