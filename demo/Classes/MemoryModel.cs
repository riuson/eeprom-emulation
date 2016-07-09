using System;
using System.Collections;
using System.Collections.Generic;
using System.Collections.Specialized;
using System.Linq;

namespace demo.Classes
{
    public class MemoryModelRecordSubItem
    {
        private IMemoryView mMemory;
        public UInt32 Index { get; private set; }

        public MemoryModelRecordSubItem(IMemoryView memory, UInt32 index)
        {
            this.mMemory = memory;
            this.Index = index;
        }

        public byte Value { get { return this.mMemory.GetByte(this.Index); } }

        public ByteType ByteType { get { return this.mMemory.GetByteType(this.Index); } }
    }

    public class MemoryModelRecord
    {
        private IMemoryView mMemory;
        private UInt32 mIndex;
        private UInt32 mCount;

        public MemoryModelRecord(IMemoryView memory, UInt32 index, UInt32 count)
        {
            this.mMemory = memory;
            this.mIndex = index;
            this.mCount = count;
        }

        public UInt32 Offset { get { return this.mIndex * this.mCount; } }

        public MemoryModelRecordSubItem[] Items
        {
            get
            {
                return Enumerable.Range(0, (int)this.mCount)
                        .Select(x => new MemoryModelRecordSubItem(this.mMemory, this.Offset + (uint)x))
                        .ToArray();
            }
        }

        public override string ToString()
        {
            return
                String.Format("{0:X8}: ", this.Offset)
                +
                String.Join(
                    " ",
                    Enumerable.Range(0, (int)this.mCount)
                        .Select(x => this.mMemory.GetByte(this.mIndex + (uint)x).ToString("X2")));
        }
    }

    public class MemoryModelEnumerator : IEnumerator<MemoryModelRecord>
    {
        private MemoryModel mMemoryModel;
        private int? mIndex;

        public MemoryModelEnumerator(MemoryModel memoryModel)
        {
            this.mMemoryModel = memoryModel;
            this.mIndex = null;
        }

        public MemoryModelRecord Current
        {
            get
            {
                if (this.mIndex < this.mMemoryModel.RowsCount)
                {
                    return this.mMemoryModel[this.mIndex.Value];
                }

                throw new IndexOutOfRangeException();
            }
        }

        object IEnumerator.Current
        {
            get
            {
                if (this.mIndex < this.mMemoryModel.RowsCount)
                {
                    return this.mMemoryModel[this.mIndex.Value];
                }

                throw new IndexOutOfRangeException();
            }
        }

        public void Dispose()
        {
        }

        public bool MoveNext()
        {
            if (!this.mIndex.HasValue)
            {
                this.mIndex = 0;
            }
            else
            {
                this.mIndex++;
            }

            return (this.mIndex < this.mMemoryModel.RowsCount);
        }

        public void Reset()
        {
            this.mIndex = 0;
        }
    }

    public class MemoryModel : IList<MemoryModelRecord>, INotifyCollectionChanged
    {
        private IMemoryView mMemory;

        public event NotifyCollectionChangedEventHandler CollectionChanged;

        public UInt32 ColumnsCount { get; private set; }
        public UInt32 RowsCount { get { return this.mMemory.BytesCount / this.ColumnsCount; } }

        public MemoryModel()
        {
            this.ColumnsCount = 0;
            this.mMemory = null;
        }

        public void UpdateData(IMemoryView memory, UInt32 columns)
        {
            this.mMemory = memory;
            this.ColumnsCount = columns;

            this.CollectionChanged?.Invoke(this, new NotifyCollectionChangedEventArgs(NotifyCollectionChangedAction.Reset));
        }

        public MemoryModelRecord this[int index]
        {
            get
            {
                if (index < 0 || index >= this.RowsCount)
                {
                    throw new IndexOutOfRangeException();
                }

                return new MemoryModelRecord(this.mMemory, Convert.ToUInt32(index), this.ColumnsCount);
            }

            set
            {
                throw new NotImplementedException();
            }
        }

        public int Count
        {
            get
            {
                return Convert.ToInt32(this.RowsCount);
            }
        }

        public bool IsReadOnly
        {
            get
            {
                return true;
            }
        }

        public void Add(MemoryModelRecord item)
        {
            throw new NotImplementedException();
        }

        public void Clear()
        {
            throw new NotImplementedException();
        }

        public bool Contains(MemoryModelRecord item)
        {
            throw new NotImplementedException();
        }

        public void CopyTo(MemoryModelRecord[] array, int arrayIndex)
        {
            throw new NotImplementedException();
        }

        public IEnumerator<MemoryModelRecord> GetEnumerator()
        {
            throw new NotImplementedException();
        }

        public int IndexOf(MemoryModelRecord item)
        {
            throw new NotImplementedException();
        }

        public void Insert(int index, MemoryModelRecord item)
        {
            throw new NotImplementedException();
        }

        public bool Remove(MemoryModelRecord item)
        {
            throw new NotImplementedException();
        }

        public void RemoveAt(int index)
        {
            throw new NotImplementedException();
        }

        IEnumerator IEnumerable.GetEnumerator()
        {
            return new MemoryModelEnumerator(this);
        }
    }
}
