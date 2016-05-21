using NUnit.Framework;
using System;
using System.Collections.Generic;
using System.Linq;
using testlib.Classes;
using testlib.Wrapper;

namespace testlib.Tests
{
    [TestFixture]
    public class TestInfo : TestBase
    {
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

        [Test]
        public void CanReadAll(
            [Range(0u, WordsOnPage + 40u, WordsOnPage / 7u)]
            uint count)
        {
            Eeprom.Result result;
            ushort[] array = this.GenerateArray(count);

            for (ushort i = 0; i < Convert.ToUInt16(count); i++)
            {
                result = Eeprom.WriteValue(i, array[i]);
            }

            ushort countStored;
            result = Eeprom.GetKeysCount(out countStored);
            Assert.That(result, Is.EqualTo(Eeprom.Result.Success));

            Dictionary<ushort, ushort> readed = new Dictionary<ushort, ushort>();

            for (ushort i = 0; i < countStored; i++)
            {
                ushort key, value;
                result = Eeprom.ReadByIndex(i, out key, out value);
                Assert.That(result, Is.EqualTo(Eeprom.Result.Success));
                readed.Add(key, value);
            }

            if (count < WordsOnPage - ReservedWords)
            {
                Assert.That(readed.Count, Is.EqualTo(count));
            }
            else
            {
                Assert.That(readed.Count, Is.EqualTo(WordsOnPage - ReservedWords));
            }

            for (ushort i = 0; i < count; i++)
            {
                if (readed.ContainsKey(i))
                {
                    Assert.That(readed[i], Is.EqualTo(array[i]));
                }
            }
        }

        [Test]
        public void CanNotReadOutOfIndex(
            [Values(0u, 5u, 10u)]
            uint count)
        {
            Eeprom.Result result;
            ushort[] array = this.GenerateArray(count);

            for (ushort i = 0; i < Convert.ToUInt16(count); i++)
            {
                result = Eeprom.WriteValue(i, array[i]);
            }

            ushort key, value;
            result = Eeprom.ReadByIndex((ushort)(count + 2), out key, out value);
            Assert.That(result, Is.EqualTo(Eeprom.Result.KeyNotFound));
        }
    }
}
