using NUnit.Framework;
using System;
using System.Collections;
using System.Collections.Generic;
using System.Linq;
using testlib.Classes;
using testlib.Wrapper;

namespace testlib.Tests
{
    [TestFixture]
    internal class TestBitsOverwrite : TestBase
    {
        private bool CanOverwrite(UInt32 valueOld, UInt32 valueNew, out int problemBitIndex)
        {
            // Other way
            BitArray bitsOld = new BitArray(BitConverter.GetBytes(valueOld));
            BitArray bitsNew = new BitArray(BitConverter.GetBytes(valueNew));
            problemBitIndex = -1;

            if (bitsOld.Length == bitsNew.Length)
            {
                for (int i = 0; i < bitsOld.Length; i++)
                {
                    // Can't change stored bit only from 0 to 1
                    if ((bitsOld[i] == false) && (bitsNew[i] == true))
                    {
                        problemBitIndex = i;
                        return false;
                    }
                }

                return true;
            }

            return false;
        }

        private string ConvertValueToBitString(UInt32 value)
        {
            BitArray bits = new BitArray(BitConverter.GetBytes(value));
            return String.Join(" ", bits.Cast<bool>().Reverse().Select(x => x ? "1" : "0"));
        }

        [Test]
        [Repeat(1000)]
        public void CanDetectOverwritePossibility()
        {
            UInt32 valueOld = Convert.ToUInt32(this.mRnd.Next());
            UInt32 valueNew = Convert.ToUInt32(this.mRnd.Next());

            Eeprom.Result result = this.mMemory.LowLevelCanOverwrite(valueOld, valueNew);
            bool result1 = (result == Eeprom.Result.Success);
            int problemBitIndex;
            bool result2 = this.CanOverwrite(valueOld, valueNew, out problemBitIndex);

            if (result1 != result2)
            {
                TestContext.WriteLine("New: {0} {1:X8}", this.ConvertValueToBitString(valueNew), valueNew);
                TestContext.WriteLine("Old: {0} {1:X8}", this.ConvertValueToBitString(valueOld), valueOld);
                TestContext.WriteLine("Bit: {0}^", new string(' ', (31 - problemBitIndex) * 2));
                TestContext.WriteLine("Actual '{0}' <> expected '{1}'", result1, result2);
            }

            Assert.That(result1, Is.EqualTo(result2));
        }
    }
}
