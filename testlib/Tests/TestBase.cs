using NUnit.Framework;
using System;
using System.Linq;
using testlib.Classes;
using testlib.Wrapper;

namespace testlib.Tests
{
    public class TestBase
    {
        protected byte[] mMemoryArray;
        protected const uint AllocatedSize = 1024 * 100;
        protected const uint WordsOnPage = 128;
        protected const uint PagesCount = 4;
        protected const uint ReservedWords = 1;
        protected Random mRnd;

        protected void ShowContent()
        {
            this.mMemoryArray.Print(0, WordsOnPage * (PagesCount + 1) * sizeof(UInt32));
        }

        protected ushort[] GenerateArray(uint count)
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

        [TearDown]
        public void Post()
        {
        }
    }
}
