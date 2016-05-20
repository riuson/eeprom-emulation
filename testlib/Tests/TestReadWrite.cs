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
        private const uint AllocatedSize = 1024 * 1024;
        private const uint WordsOnPage = 128;
        private const uint PagesCount = 4;
        private const uint ReservedWords = 1;

        private void ShowContent()
        {
            this.mMemoryArray.Print(0, WordsOnPage * (PagesCount + 1) * sizeof(UInt32));
        }

        [SetUp]
        public void Pre()
        {
            this.mMemoryArray = new byte[AllocatedSize];
            Eeprom.Result result = Eeprom.InitDebug(this.mMemoryArray, WordsOnPage, PagesCount);
            Assert.That(result, Is.EqualTo(Eeprom.Result.Success));
        }

        [Test]
        public void CanWriteValues()
        {
            ushort count = 100;

            for (ushort i = 0; i < count; i++)
            {
                Eeprom.Result result = Eeprom.WriteValue(i, i);
                Assert.That(result, Is.EqualTo(Eeprom.Result.Success));
            }
        }

        [Test]
        public void CanWriteOnlyFittableCount(
            [Range(0, WordsOnPage + 20, WordsOnPage / 7)]
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
        public void CanReadValues()
        {
            ushort count = 100;

            for (ushort i = 0; i < count; i++)
            {
                Eeprom.Result result = Eeprom.WriteValue(i, i);
            }

            for (ushort i = 0; i < count; i++)
            {
                ushort value;
                Eeprom.Result result = Eeprom.ReadValue(i, out value);
                Assert.That(result, Is.EqualTo(Eeprom.Result.Success));
                Assert.That(value, Is.EqualTo(i));
            }
        }

        [Test]
        public void CanReadOnlyFittableCount(
            [Range(0, WordsOnPage + 20, WordsOnPage / 7)]
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
        public void CanReplace(
            [Range(0, WordsOnPage - ReservedWords, WordsOnPage / 7)]
            uint count)
        {
            Random rnd = new Random(DateTime.Now.Millisecond);
            ushort[] values1 = Enumerable.Range(1, Convert.ToInt32(count)).Select(_ => Convert.ToUInt16(rnd.Next(0, ushort.MaxValue))).ToArray();
            ushort[] values2 = Enumerable.Range(1, Convert.ToInt32(count)).Select(_ => Convert.ToUInt16(rnd.Next(0, ushort.MaxValue))).ToArray();
            ushort[] values3 = Enumerable.Range(1, Convert.ToInt32(count)).Select(_ => Convert.ToUInt16(rnd.Next(0, ushort.MaxValue))).ToArray();

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
        }

        [TearDown]
        public void Post()
        {
        }
    }
}
