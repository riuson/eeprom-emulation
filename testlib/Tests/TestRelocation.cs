using NUnit.Framework;
using System;
using System.Linq;
using testlib.Classes;
using testlib.Wrapper;

namespace testlib
{
    [TestFixture]
    public class TestRelocation
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
        public void DoNotChangeMemoryWhenValueExisted(
            [Range(0u, WordsOnPage + 40u, WordsOnPage / 7u)]
            uint count)
        {
            ushort[] array = this.GenerateArray(count);

            // 1
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

            byte[] copy1 = (byte[])this.mMemoryArray.Clone();

            // 2
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

            byte[] copy2 = (byte[])this.mMemoryArray.Clone();

            // 3
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

            byte[] copy3 = (byte[])this.mMemoryArray.Clone();

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

            byte[] copy1 = (byte[])this.mMemoryArray.Clone();

            // 2
            for (ushort i = 0; i < Convert.ToUInt16(count); i++)
            {
                Eeprom.Result result = Eeprom.WriteValue(i, i);

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

            byte[] copy2 = (byte[])this.mMemoryArray.Clone();

            Assert.That(copy1, Is.EqualTo(copy2));
        }

        [TearDown]
        public void Post()
        {
        }
    }
}
