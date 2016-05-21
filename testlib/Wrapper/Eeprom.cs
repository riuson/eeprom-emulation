using System;
using System.Runtime.InteropServices;

namespace testlib.Wrapper
{
    internal static class Eeprom
    {
        [DllImport("wrapper.dll", SetLastError = true, CharSet = CharSet.Unicode, EntryPoint = "LIB_eeprom_init_debug", CallingConvention = CallingConvention.Cdecl)]
        public static extern Result InitDebug(byte[] buffer, UInt32 words_on_page, UInt32 pages_count);

        [DllImport("wrapper.dll", SetLastError = true, CharSet = CharSet.Unicode, EntryPoint = "LIB_eeprom_read_value", CallingConvention = CallingConvention.Cdecl)]
        public static extern Result ReadValue(UInt16 key, out UInt16 value);

        [DllImport("wrapper.dll", SetLastError = true, CharSet = CharSet.Unicode, EntryPoint = "LIB_eeprom_write_value", CallingConvention = CallingConvention.Cdecl)]
        public static extern Result WriteValue(UInt16 key, UInt16 value);

        [DllImport("wrapper.dll", SetLastError = true, CharSet = CharSet.Unicode, EntryPoint = "LIB_eeprom_keys_count", CallingConvention = CallingConvention.Cdecl)]
        public static extern Result GetKeysCount(out UInt16 count);

        [DllImport("wrapper.dll", SetLastError = true, CharSet = CharSet.Unicode, EntryPoint = "LIB_eeprom_read_by_index", CallingConvention = CallingConvention.Cdecl)]
        public static extern Result ReadByIndex(UInt16 index, out UInt16 key, out UInt16 value);

        public enum Result : int
        {
            Success = 0,
            ReadFailed = 1,
            NoEmptyPage = 2,
            KeyNotFound = 3,
            NoEmptyRecord = 4,
            InvalidParameters = 5,
            NeedErase = 6,
            UncatchedFail = 0xff
        };
    }
}
