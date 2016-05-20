using NUnit.Framework;
using System;
using System.Linq;
using testlib.Classes;
using testlib.Wrapper;

namespace testlib
{
    [TestFixture]
    public class TestReadWrite
    {
        private byte[] mMemoryArray;
        private const uint AllocatedSize = 1024 * 100;
        private const uint WordsOnPage = 128;
        private const uint PagesCount = 4;
        private const uint ReservedWords = 1;
        private Random mRnd;

        private void ShowContent()
        {
            this.mMemoryArray.Print(0, WordsOnPage * (PagesCount + 1) * sizeof(UInt32));
        }

        private ushort[] GenerateArray(uint count)
        {
            return Enumerable.Range(1, Convert.ToInt32(count)).Select(_ => Convert.ToUInt16(this.mRnd.Next(0, ushort.MaxValue))).ToArray();
        }

        [SetUp]
        public void Pre()
        {
            this.mRnd = new Random(DateTime.Now.Millisecond);

            this.mMemoryArray = new byte[AllocatedSize];
            Eeprom.Result result = Eeprom.InitDebug(this.mMemoryArray, WordsOnPage, PagesCount);
            Assert.That(result, Is.EqualTo(Eeprom.Result.Success));
        }

        [Test]
        public void CanWrite(
            [Range(0u, WordsOnPage + 20u, WordsOnPage / 7u)]
            uint count)
        {
            for (ushort i = 0; i < Convert.ToUInt16(count); i++)
            {
                Eeprom.Result result = Eeprom.WriteValue(i, i);

                if (i < WordsOnPage - ReservedWords)
                {
                    Assert.That(result, Is.EqualTo(Eeprom.Result.Success));
                }
                else
                {
                    Assert.That(result, Is.EqualTo(Eeprom.Result.NoEmptyRecord));
                }
            }
        }

        [Test]
        public void CanRead(
            [Range(0u, WordsOnPage + 20u, WordsOnPage / 7u)]
            uint count)
        {
            for (ushort i = 0; i < count; i++)
            {
                Eeprom.Result result = Eeprom.WriteValue(i, i);
            }

            for (ushort i = 0; i < count; i++)
            {
                ushort value;
                Eeprom.Result result = Eeprom.ReadValue(i, out value);

                if (i < WordsOnPage - ReservedWords)
                {
                    Assert.That(result, Is.EqualTo(Eeprom.Result.Success));
                    Assert.That(value, Is.EqualTo(i));
                }
                else
                {
                    Assert.That(result, Is.EqualTo(Eeprom.Result.KeyNotFound));
                }
            }
        }

        [Test]

        [Repeat(100)]
        public void CanReplace(
            [Range(1u, 4u)]
            uint count)
        {
            ushort[] values1 = this.GenerateArray(count);
            ushort[] values2 = this.GenerateArray(count);
            ushort[] values3 = this.GenerateArray(count);
            ushort[] values4 = this.GenerateArray(count);
            ushort[] values5 = this.GenerateArray(count);

            Action<ushort[], uint> check = (_array, _count) =>
                {
                    for (ushort i = 0; i < Convert.ToUInt16(_count); i++)
                    {
                        Eeprom.Result result = Eeprom.WriteValue(i, _array[i]);
                        Assert.That(result, Is.EqualTo(Eeprom.Result.Success));
                    }

                    for (ushort i = 0; i < Convert.ToUInt16(_count); i++)
                    {
                        ushort value;
                        Eeprom.Result result = Eeprom.ReadValue(i, out value);
                        Assert.That(result, Is.EqualTo(Eeprom.Result.Success));
                        Assert.That(value, Is.EqualTo(_array[i]));
                    }
                };

            check(values1, count);
            check(values2, count);
            check(values3, count);
            check(values4, count);
            check(values5, count);
        }

        [TearDown]
        public void Post()
        {
        }
    }
}
