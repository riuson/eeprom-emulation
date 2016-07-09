using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace demo.Classes
{
    [Flags]
    public enum ByteType : int
    {
        None = 0,
        Clean = 1,
        Start = 2,
        Key = 4,
        Value = 8
    }

    public interface IMemoryView
    {
        UInt32 BytesCount { get; }
        byte GetByte(UInt32 index);
        ByteType GetByteType(UInt32 index);
    }
}
