using NUnit.Framework;
using System;
using System.Linq;
using testlib.Classes;
using testlib.Wrapper;

namespace testlib.Tests
{
    internal class TestBase
    {
        protected Eeprom mMemory;
        protected const uint WordsOnPage = 128;
        protected const uint PagesCount = 4;
        protected const uint ReservedWords = 1;
        protected Random mRnd;

        protected ushort[] GenerateArray(uint count)
        {
            return Enumerable.Range(1, Convert.ToInt32(count)).Select(_ => Convert.ToUInt16(this.mRnd.Next(0, ushort.MaxValue))).ToArray();
        }

        [SetUp]
        public void Pre()
        {
            this.mRnd = new Random(DateTime.Now.Millisecond);

            this.mMemory = new Eeprom();
            Eeprom.Result result = this.mMemory.Initialize(WordsOnPage, PagesCount);
            Assert.That(result, Is.EqualTo(Eeprom.Result.Success));
        }

        [TearDown]
        public void Post()
        {
        }
    }
}
