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

        private void ShowContent()
        {
            this.mMemoryArray.Print(0, WordsOnPage * (PagesCount + 1) * sizeof(UInt32));
        }

        [SetUp]
        public void Pre()
        {
            this.mMemoryArray = new byte[AllocatedSize];
            Lib.EepromResult result = Lib.EepromInitDebug(this.mMemoryArray, WordsOnPage, PagesCount);
            Assert.That(result, Is.EqualTo(Lib.EepromResult.Success));
        }

        [Test]
        public void CanWriteValues()
        {
            for (ushort i = 0; i < 50; i++)
            {
                Lib.EepromResult result = Lib.EepromWriteValue(i, i);

                Assert.That(result, Is.EqualTo(Lib.EepromResult.Success));
            }
        }

        [Test]
        public void Test2()
        {
        }

        [TearDown]
        public void Post()
        {
        }
    }
}
