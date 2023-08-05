using System;
using MyNamespace;
using DecimalFunctionsExample;
using System.Globalization;
using System.Collections.Generic;
using System.IO;

namespace MyConvert
{
    public class DecimalConvert
    {
        public static void GenConvertTests(
            decimal[] decimal_values, float[] float_values, int[] int_values, 
            StreamWriter writer, List<string> all_suites
        ) {
            string name = "from_decimal_to_float";
            var cases = GenConvertTest<decimal, float>(
                name, decimal_values, writer,
                d => DecimalArray.DecimalToS21Initializer(d), f => f.ToString(),
                "s21_decimal", "float", "relatively_equal(result, must_be) == 0");
            all_suites.Add(Program.CreateSuite(name, cases.ToArray(), writer));
            Console.WriteLine("Created " + name);

            name = "from_decimal_to_int";
            cases = GenConvertTest<decimal, int>(
                name, decimal_values, writer,
                d => DecimalArray.DecimalToS21Initializer(d), i => i.ToString(),
                "s21_decimal", "int", "(result != must_be)");
            all_suites.Add(Program.CreateSuite(name, cases.ToArray(), writer));
            Console.WriteLine("Created " + name);

            name = "from_int_to_decimal";
            cases = GenConvertTest<int, decimal>(
                name, int_values, writer,
                i => i.ToString(), d => DecimalArray.DecimalToS21Initializer(d),
                "int", "s21_decimal", "s21_is_equal(result, must_be) == 0");
            all_suites.Add(Program.CreateSuite(name, cases.ToArray(), writer));
            Console.WriteLine("Created " + name);

            name = "from_float_to_decimal";
            cases = GenConvertTest<float, decimal>(
                name, float_values, writer,
                f => f.ToString(), d => DecimalArray.DecimalToS21Initializer(d),
                "float", "s21_decimal", "relatively_equal_decimal(result, must_be) == 0");
            all_suites.Add(Program.CreateSuite(name, cases.ToArray(), writer));
            Console.WriteLine("Created " + name);
        }
         public static List<string> GenConvertTest<A, B>(
            string func_name, A[] values, StreamWriter writer,
            Func<A, String> a_to_string, Func<B, String> b_to_string, 
            string a_type_name, string b_type_name, string equality_cmp
        ) {
            List<string> all_cases = new List<string>();
            int count = 0;
            foreach (A a in values) {
                int error = 0;
                B result = default(B);
                try {
                    result = (B)Convert.ChangeType(a, typeof(B));
                } catch (Exception) {
                    error = 1;
                }

                string name = String.Format("test_{0}_{1}", func_name, count);
                all_cases.Add(name);
                
                string text = @$"
START_TEST({name})
{{
    {a_type_name} a = {a_to_string(a)};
    {b_type_name} result;
    {b_type_name} must_be = {b_to_string(result)};
    int error = s21_{func_name}(a, &result);

    if (error != {error}) {{
        ck_abort_msg(""Error code differs - returned %d but must be {error} - {func_name} ({a}) must be {result}"", error);
    }} else if (error == 0 && {equality_cmp}) {{
        ck_abort_msg(""Fail test {count}: {func_name} ({a}) must be {result}"");
    }}
}}
END_TEST
";
                writer.Write(text);

                count++;
            }
            return all_cases;
        }
    }
}