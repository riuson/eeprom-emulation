using NUnit.Framework;
using System;
using testlib.Classes;
using testlib.Wrapper;

namespace testlib
{
    [TestFixture]
    public class Test
    {
        private byte[] mMemoryArray;
        private const int AllocatedSize = 1024 * 1024;
        private const int WordsOnPage = 128;
        private const int PagesCount = 4;
        private const int ReservedWords = 1;

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
            int count)
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
            int count)
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

        [TearDown]
        public void Post()
        {
        }
    }
}
