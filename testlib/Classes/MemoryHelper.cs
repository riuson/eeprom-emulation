using System;
using System.Linq;
using System.Text.RegularExpressions;

namespace testlib.Classes
{
    internal static class MemoryHelper
    {
        public static string ToString(this byte[] array, int index, int length)
        {
            return String.Join(" ", Enumerable.Range(0, length).Select(x => array[x + index]).Select(b => b.ToString("X2")));
        }

        public static void Print(this byte[] array, int index, int length)
        {
            Log("Array 0x{0:X8} [0x{1:X8}]", index, length);

            int bytesOnRow = 16;
            int rowsOnPage = 16;

            byte[] columns = Enumerable.Range(0, bytesOnRow).Select(x => Convert.ToByte(x)).ToArray();
            Log("row  \\  col  {0}",
                columns.ToString(0, bytesOnRow));

            for (int i = 0, row = 0; i < length; i += bytesOnRow, row++)
            {
                int shift = index + i;
                int count = ((i + bytesOnRow) <= length) ? bytesOnRow : (length % bytesOnRow);
                string msg = String.Format(
                    "0x{0:X8}:  {1}",
                    shift,
                    array.ToString(shift, count));
                Log(msg);

                if (row % rowsOnPage == rowsOnPage - 1)
                {
                    Log(String.Empty);
                }
            }
        }

        private static void Log(string format, params object[] arg)
        {
            Console.WriteLine(format, arg);
        }
    }
}
