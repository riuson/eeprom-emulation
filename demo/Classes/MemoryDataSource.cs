using System;
using testlib.Wrapper;

namespace demo.Classes
{
    internal class MemoryDataSource : Eeprom, IMemoryView, IStoredView
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

            if ((index & 0xfffffffcu) == this.Config.ActivePageIndex * this.Config.WordsOnPage * sizeof(UInt32))
            {
                result |= ByteType.Start;
            }
            else
            {
                UInt32 offset = index & 0xfffffffcu;
                UInt16 value = Convert.ToUInt16((this.GetByte(offset + 1) << 8) | this.GetByte(offset));
                UInt16 key = Convert.ToUInt16((this.GetByte(offset + 3) << 8) | this.GetByte(offset + 2));

                if (key != 0xffffu)
                {
                    if (index - offset >= 2)
                    {
                        result |= ByteType.Key;
                    }
                    else
                    {
                        result |= ByteType.Value;
                    }
                }
            }

            return result;
        }

        public ushort KeysCount
        {
            get
            {
                UInt16 result;

                if (base.GetKeysCount(out result) == Result.Success)
                {
                    return result;
                }

                return 0;
            }
        }

        public bool GetRecord(UInt16 index, out UInt16 key, out UInt16 value)
        {
            return base.ReadByIndex(index, out key, out value) == Result.Success;
        }
    }
}
