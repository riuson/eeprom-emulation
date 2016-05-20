using NUnit.Framework;
using testlib.Wrapper;

namespace testlib
{
    [TestFixture]
    public class Test
    {
        private const int MemorySize = 1024 * 1024;
        private byte[] mMemoryArray;

        [OneTimeSetUp]
        public void Pre()
        {
            this.mMemoryArray = new byte[MemorySize];
            Lib.EepromResult result = Lib.EepromInitDebug(this.mMemoryArray, 128, 8);
            Assert.That(result, Is.EqualTo(Lib.EepromResult.Success));
        }

        [Test]
        public void Test1()
        {
        }

        [OneTimeTearDown]
        public void Post()
        {

        }
    }
}
