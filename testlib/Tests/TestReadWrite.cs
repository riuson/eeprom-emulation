using NUnit.Framework;
using System;
using testlib.Wrapper;

namespace testlib.Tests
{
    [TestFixture]
    internal class TestReadWrite : TestBase
    {
        [Test]
        public void CanWrite(
            [Range(0u, WordsOnPage + 20u, WordsOnPage / 7u)]
            uint count)
        {
            for (ushort i = 0; i < Convert.ToUInt16(count); i++)
            {
                Eeprom.Result result = this.mMemory.Write(i, i);

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
                Eeprom.Result result = this.mMemory.Write(i, i);
            }

            for (ushort i = 0; i < count; i++)
            {
                ushort value;
                Eeprom.Result result = this.mMemory.Read(i, out value);

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
                        Eeprom.Result result = this.mMemory.Write(i, _array[i]);
                        Assert.That(result, Is.EqualTo(Eeprom.Result.Success));
                    }

                    for (ushort i = 0; i < Convert.ToUInt16(_count); i++)
                    {
                        ushort value;
                        Eeprom.Result result = this.mMemory.Read(i, out value);
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
    }
}
