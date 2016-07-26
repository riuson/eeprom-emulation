using NUnit.Framework;
using System;
using System.Collections.Generic;
using System.Linq;
using testlib.Classes;
using testlib.Wrapper;

namespace testlib.Tests
{
    [TestFixture]
    internal class TestInfo : TestBase
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
                result = this.mMemory.Write(i, i);

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
            result = this.mMemory.GetKeysCount(out countStored);
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
        public void CanReadByIndex(
            [Range(0u, WordsOnPage + 40u, WordsOnPage / 7u)]
            uint count)
        {
            Eeprom.Result result;
            ushort[] array = this.GenerateArray(count);

            for (ushort i = 0; i < Convert.ToUInt16(count); i++)
            {
                result = this.mMemory.Write(i, array[i]);
            }

            ushort countStored;
            result = this.mMemory.GetKeysCount(out countStored);
            Assert.That(result, Is.EqualTo(Eeprom.Result.Success));

            Dictionary<ushort, ushort> readed = new Dictionary<ushort, ushort>();

            for (ushort i = 0; i < countStored; i++)
            {
                ushort key, value;
                result = this.mMemory.ReadByIndex(i, out key, out value);
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
        public void CanReadByIndex2()
        {
            Eeprom.Result result;
            Dictionary<ushort, ushort> list = new Dictionary<ushort, ushort>();
            list.Add(1, 1);
            list.Add(2, 2);
            list.Add(3, 3);
            list.Add(4, 4);
            list.Add(5, 5);
            list.Add(6, 6);
            list.Add(7, 7);
            list.Add(8, 8);

            foreach (var kvp in list)
            {
                result = this.mMemory.Write(kvp.Key, kvp.Value);
            }

            list[1] = Convert.ToUInt16(list[1] + 1);
            list[2] = Convert.ToUInt16(list[2] + 1);
            list[3] = Convert.ToUInt16(list[3] + 1);

            foreach (var kvp in list)
            {
                result = this.mMemory.Write(kvp.Key, kvp.Value);
            }

            ushort countStored;
            result = this.mMemory.GetKeysCount(out countStored);
            Assert.That(result, Is.EqualTo(Eeprom.Result.Success));
            Assert.That(countStored, Is.EqualTo(list.Count));

            Dictionary<ushort, ushort> readed = new Dictionary<ushort, ushort>();

            for (ushort i = 0; i < countStored; i++)
            {
                ushort key, value;
                result = this.mMemory.ReadByIndex(i, out key, out value);
                Assert.That(result, Is.EqualTo(Eeprom.Result.Success));
                readed.Add(key, value);
            }

            foreach (var readedItem in readed)
            {
                Assert.That(list.ContainsKey(readedItem.Key), Is.True);

                Assert.That(readedItem.Value, Is.EqualTo(list[readedItem.Key]));
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
                result = this.mMemory.Write(i, array[i]);
            }

            ushort key, value;
            result = this.mMemory.ReadByIndex((ushort)(count + 2), out key, out value);
            Assert.That(result, Is.EqualTo(Eeprom.Result.KeyNotFound));
        }
    }
}
