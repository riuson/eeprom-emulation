using NUnit.Framework;
using System;
using System.Linq;
using testlib.Classes;
using testlib.Wrapper;

namespace testlib.Tests
{
    [TestFixture]
    internal class TestRelocation : TestBase
    {
        [Test]
        public void DoNotChangeMemoryWhenValueExisted(
            [Range(0u, WordsOnPage + 20u)]
            uint count)
        {
            ushort[] array = this.GenerateArray(count);

            // 1
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

            byte[] copy1 = (byte[])this.mMemory.GetBufferCopy();

            // 2
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

            byte[] copy2 = (byte[])this.mMemory.GetBufferCopy();

            // 3
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

            byte[] copy3 = (byte[])this.mMemory.GetBufferCopy();

            Assert.That(copy1, Is.EqualTo(copy2));
            Assert.That(copy2, Is.EqualTo(copy3));
        }

        [Test]
        public void DoNotMovePageWithAllRecordAreUniqueAndFilled()
        {
            uint count = WordsOnPage;
            ushort[] array = this.GenerateArray(count);

            // 1
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

            byte[] copy1 = (byte[])this.mMemory.GetBufferCopy();

            // 2
            for (ushort i = 0; i < Convert.ToUInt16(count); i++)
            {
                Eeprom.Result result = this.mMemory.Write(i, i);

                if (i < WordsOnPage - ReservedWords)
                {
                    if (result != Eeprom.Result.Success)
                    {
                        TestContext.WriteLine("{0}", i);
                    }
                    Assert.That(result, Is.EqualTo(Eeprom.Result.Success));
                }
                else
                {
                    Assert.That(result, Is.EqualTo(Eeprom.Result.NoEmptyRecord));
                }
            }

            byte[] copy2 = (byte[])this.mMemory.GetBufferCopy();

            Assert.That(copy1, Is.EqualTo(copy2));
        }

        [Test]
        [Repeat(100)]
        public void CanReadAllAfterRelocation()
        {
            uint count = (WordsOnPage - ReservedWords) / 2 + 1;
            ushort[] array = this.GenerateArray(count);
            ushort[] array2 = this.GenerateArray(count);
            ushort[] array3 = this.GenerateArray(count);

            // 1
            for (ushort i = 0; i < Convert.ToUInt16(count); i++)
            {
                Eeprom.Result result = this.mMemory.Write(i, array[i]);
                Assert.That(result, Is.EqualTo(Eeprom.Result.Success));

                {
                    ushort readed;
                    result = this.mMemory.Read(i, out readed);
                    Assert.That(result, Is.EqualTo(Eeprom.Result.Success));
                    Assert.That(readed, Is.EqualTo(array[i]));
                }

                result = this.mMemory.Write(i, array2[i]);
                Assert.That(result, Is.EqualTo(Eeprom.Result.Success));

                {
                    ushort readed;
                    result = this.mMemory.Read(i, out readed);
                    Assert.That(result, Is.EqualTo(Eeprom.Result.Success));
                    Assert.That(readed, Is.EqualTo(array2[i]));
                }

                result = this.mMemory.Write(i, array3[i]);
                Assert.That(result, Is.EqualTo(Eeprom.Result.Success));

                {
                    ushort readed;
                    result = this.mMemory.Read(i, out readed);
                    Assert.That(result, Is.EqualTo(Eeprom.Result.Success));
                    Assert.That(readed, Is.EqualTo(array3[i]));
                }
            }
        }
    }
}
