using System;
using System.Runtime.InteropServices;
using testlib.Classes;

namespace testlib.Wrapper
{
    internal class Eeprom
    {
        #region Types
        [StructLayout(LayoutKind.Sequential)]
        public struct Configuration
        {
            public UInt32 WordsOnPage;
            public UInt32 PagesCount;
            public UInt32 TotalSize;
            public UInt32 ActivePageAddress;

            public Configuration(UInt32 wordOnPage, UInt32 pagesCount)
            {
                this.WordsOnPage = wordOnPage;
                this.PagesCount = pagesCount;
                this.TotalSize = 0;
                this.ActivePageAddress = 0;
            }
        }

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

        public class KeyValue
        {
            public ushort Key { get; set; }
            public ushort Value { get; set; }

            public KeyValue()
            {
                this.Key = 0;
                this.Value = 0;
            }

            public KeyValue(ushort key, ushort value)
            {
                this.Key = key;
                this.Value = value;
            }
        }
        #endregion

        #region PInvoke

        [DllImport("wrapper.dll", SetLastError = true, CharSet = CharSet.Unicode, EntryPoint = "LIB_eeprom_init", CallingConvention = CallingConvention.Cdecl)]
        private static extern Result Init(
            byte[] buffer, ref Configuration config);

        [DllImport("wrapper.dll", SetLastError = true, CharSet = CharSet.Unicode, EntryPoint = "LIB_eeprom_read_value", CallingConvention = CallingConvention.Cdecl)]
        private static extern Result ReadValue(
            byte[] buffer, ref Configuration config,
            UInt16 key, out UInt16 value);

        [DllImport("wrapper.dll", SetLastError = true, CharSet = CharSet.Unicode, EntryPoint = "LIB_eeprom_write_value", CallingConvention = CallingConvention.Cdecl)]
        private static extern Result WriteValue(
            byte[] buffer, ref Configuration config,
            UInt16 key, UInt16 value);

        [DllImport("wrapper.dll", SetLastError = true, CharSet = CharSet.Unicode, EntryPoint = "LIB_eeprom_keys_count", CallingConvention = CallingConvention.Cdecl)]
        private static extern Result GetKeysCount(
            byte[] buffer, ref Configuration config,
            out UInt16 count);

        [DllImport("wrapper.dll", SetLastError = true, CharSet = CharSet.Unicode, EntryPoint = "LIB_eeprom_read_by_index", CallingConvention = CallingConvention.Cdecl)]
        private static extern Result ReadByIndex(
            byte[] buffer, ref Configuration config,
            UInt16 index, out UInt16 key, out UInt16 value);
        #endregion

        private byte[] mDataBuffer;
        private Configuration mConfig;

        public Eeprom()
        {
            this.mDataBuffer = null;
        }

        public Configuration Config
        {
            get { return this.mConfig; }
        }

        public Result Initialize(UInt32 wordsOnPage, UInt32 pagesCount)
        {
            this.mConfig = new Configuration(wordsOnPage, pagesCount);
            this.mDataBuffer = new byte[this.mConfig.PagesCount * this.mConfig.WordsOnPage * sizeof(UInt32)];
            Result result = Eeprom.Init(this.mDataBuffer, ref this.mConfig);
            return result;
        }

        public Result Read(ushort key, out ushort value)
        {
            return Eeprom.ReadValue(this.mDataBuffer, ref this.mConfig, key, out value);
        }

        public Result Write(ushort key, ushort value)
        {
            return Eeprom.WriteValue(this.mDataBuffer, ref this.mConfig, key, value);
        }

        public Result GetKeysCount(out UInt16 count)
        {
            return Eeprom.GetKeysCount(this.mDataBuffer, ref this.mConfig, out count);
        }

        public Result ReadByIndex(UInt16 index, out UInt16 key, out UInt16 value)
        {
            return Eeprom.ReadByIndex(this.mDataBuffer, ref this.mConfig, index, out key, out value);
        }

        public void ShowContent()
        {
            this.mDataBuffer.Print(0, this.mConfig.WordsOnPage * (this.mConfig.PagesCount) * sizeof(UInt32));
        }

        public byte[] GetBufferCopy()
        {
            return (byte[])this.mDataBuffer.Clone();
        }
    }
}
