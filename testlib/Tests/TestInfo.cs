using NUnit.Framework;
using System;
using System.Linq;
using testlib.Classes;
using testlib.Wrapper;

namespace testlib
{
    [TestFixture]
    public class TestInfo
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
        public void CanGetKeysCount(
            [Range(0u, WordsOnPage + 40u, WordsOnPage / 7u)]
            uint count)
        {
            Eeprom.Result result;
            ushort[] array = this.GenerateArray(count);

            for (ushort i = 0; i < Convert.ToUInt16(count); i++)
            {
                result = Eeprom.WriteValue(i, i);

                if (i < WordsOnPage - ReservedWords)
                {
                    Assert.That(result, Is.EqualTo(Eeprom.Result.Success));
                }
                else
                {
                    Assert.That(result, Is.EqualTo(Eeprom.Result.NoEmptyRecord));
                }
            }

            ushort countStored;
            result = Eeprom.GetKeysCount(out countStored);
            Assert.That(result, Is.EqualTo(Eeprom.Result.Success));

            if (count < WordsOnPage - ReservedWords)
            {
                Assert.That(countStored, Is.EqualTo(count));
            }
            else
            {
                Assert.That(countStored, Is.EqualTo(WordsOnPage - ReservedWords));
            }
        }

        [TearDown]
        public void Post()
        {
        }
    }
}
