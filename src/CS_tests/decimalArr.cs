using System;
using System.Collections.Generic;
using System.Globalization;


// Random checks
namespace MyNamespace
{
    public class DecimalArray
    {
        public static decimal[] GetDecimalArray()
        {   
            List<decimal> decimals = new List<decimal>(new decimal[]{
                decimal.Zero, decimal.One, decimal.MaxValue,
                0.5m, 1234.5678901234123480713421356m,
                1.000000000000001m, 1.0m / decimal.MaxValue,
            });
            for (decimal i = 1m; i <= 3m; i++) {
                decimals.Add(decimal.MaxValue / i);
            }
            int length = decimals.Count;
            for (int i = 0; i < length; i++)
                decimals.Add(-decimals[i]);

            return Deduplicate(decimals.ToArray());
        }

        public static int[] GetIntArray()
        {   
            List<int> values = new List<int>(new int[]{
                0, 1, 2, 5, 10, 1000, 1234,
            });
            int length = values.Count;

            Random rand = new Random();
            for (int i = 0; i < 10; i++)
                values.Add(rand.Next());

            length = values.Count;
            for (int i = 0; i < length; i++)
                values.Add(-values[i]);

            return Deduplicate(values.ToArray());
        }

        public static float[] GetFloatArray()
        {   
            List<float> values = new List<float>(new float[]{
                0.0f, float.Epsilon, 0.5f, 1.0f, (float)Math.Sqrt(2.0), 2.0f, 10.0f, 5.0f, float.MaxValue,
                float.NaN, float.PositiveInfinity
            });
            int length = values.Count;

            Random rand = new Random();
            for (int i = 0; i < 50; i++)
                values.Add((float)rand.NextDouble());

            length = values.Count;
            for (int i = 0; i < length; i++)
                values.Add(-values[i]);

            return Deduplicate(values.ToArray());
        }

        public static T[] Deduplicate<T>(T[] values) where T: IComparable {
            Array.Sort(values);
            List<T> deduplicated = new List<T>();

            for (int i = 0; i < values.Length; i++)
                if (i == 0 || values[i].CompareTo(values[i - 1]) != 0)
                    deduplicated.Add(values[i]);

            return deduplicated.ToArray();
        }

        public static decimal[] GetDecimalArrayExtra() {
            return GetDecimalArray();
        }

        public static string DecimalToS21Initializer(decimal x) {
            int[] bits = Decimal.GetBits(x);
            return String.Format("{{{{ {0}, {1}, {2}, {3} }}}}", bits[0], bits[1], bits[2], bits[3]);
        }
    }
}
