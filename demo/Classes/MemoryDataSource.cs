using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using testlib.Wrapper;

namespace demo.Classes
{
    internal class MemoryDataSource : Eeprom, IMemoryView
    {
        public uint BytesCount
        {
            get
            {
                return Convert.ToUInt32(this.mDataBuffer.Length);
            }
        }

        public byte GetByte(uint index)
        {
            if (index >= this.BytesCount)
            {
                throw new IndexOutOfRangeException();
            }

            return this.mDataBuffer[index];
        }

        public ByteType GetByteType(uint index)
        {
            ByteType result = ByteType.None;

            if (this.GetByte(index) == 0xffu)
            {
                result |= ByteType.Clean;
            }

            return result;
        }
    }
}
