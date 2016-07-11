using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace demo.Classes
{
    public interface IStoredView
    {
        UInt16 KeysCount { get; }
        bool GetRecord(UInt16 index, out UInt16 key, out UInt16 value);
    }
}
