using System;
using System.Runtime.InteropServices;
using testlib.Classes;

namespace testlib.Wrapper
{
    public class Eeprom
    {
        #region Types
        [StructLayout(LayoutKind.Sequential)]
        public struct Configuration
        {
            public UInt32 WordsOnPage;
            public UInt32 PagesCount;
            public UInt32 TotalSize;
            public UInt32 ActivePageIndex;

            public Configuration(UInt32 wordOnPage, UInt32 pagesCount)
            {
                this.WordsOnPage = wordOnPage;
                this.PagesCount = pagesCount;
                this.TotalSize = 0;
                this.ActivePageIndex = 0;
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

        [DllImport("wrapper.dll", SetLastError = true, CharSet = CharSet.Unicode, EntryPoint = "LIB_eeprom_low_read_word", CallingConvention = CallingConvention.Cdecl)]
        private static extern Result LowLevelReadWord(
            byte[] buffer, ref Configuration config,
            UInt32 pageIndex, UInt32 cellIndex, out UInt32 value);

        [DllImport("wrapper.dll", SetLastError = true, CharSet = CharSet.Unicode, EntryPoint = "LIB_eeprom_low_write_word", CallingConvention = CallingConvention.Cdecl)]
        private static extern Result LowLevelWriteWord(
            byte[] buffer, ref Configuration config,
            UInt32 pageIndex, UInt32 cellIndex, UInt32 value);

        [DllImport("wrapper.dll", SetLastError = true, CharSet = CharSet.Unicode, EntryPoint = "LIB_eeprom_low_erase_page", CallingConvention = CallingConvention.Cdecl)]
        private static extern Result LowLevelErasePage(
            byte[] buffer, ref Configuration config,
            UInt32 pageIndex);

        [DllImport("wrapper.dll", SetLastError = true, CharSet = CharSet.Unicode, EntryPoint = "LIB_eeprom_low_can_overwrite", CallingConvention = CallingConvention.Cdecl)]
        private static extern Result LowLevelCanOverwrite_(
            UInt32 valueOld, UInt32 valueNew);

        #endregion

        protected byte[] mDataBuffer;
        protected Configuration mConfig;

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

        public Result LowLevelReadWord(UInt32 pageIndex, UInt32 cellIndex, out UInt32 value)
        {
            return Eeprom.LowLevelReadWord(this.mDataBuffer, ref this.mConfig, pageIndex, cellIndex, out value);
        }

        public Result LowLevelWriteWord(UInt32 pageIndex, UInt32 cellIndex, UInt32 value)
        {
            return Eeprom.LowLevelWriteWord(this.mDataBuffer, ref this.mConfig, pageIndex, cellIndex, value);
        }

        public Result LowLevelErasePage(UInt32 pageIndex)
        {
            return Eeprom.LowLevelErasePage(this.mDataBuffer, ref this.mConfig, pageIndex);
        }

        public Result LowLevelCanOverwrite(UInt32 valueOld, UInt32 valueNew)
        {
            return Eeprom.LowLevelCanOverwrite_(valueOld, valueNew);
        }


        public void ShowContent()
        {
            this.mDataBuffer.Print(0, this.mConfig.TotalSize);
        }

        public byte[] GetBufferCopy()
        {
            return (byte[])this.mDataBuffer.Clone();
        }
    }
}
