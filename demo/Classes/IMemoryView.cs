using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace demo.Classes
{
    public interface IMemoryView
    {
        UInt32 BytesCount { get; }
        byte GetByte(UInt32 index);
    }
}
