using System;
using System.Collections;
using System.Collections.Generic;
using System.Collections.Specialized;
using System.Linq;

namespace demo.Classes
{
    public class StoredRecord
    {
        private IStoredView mStored;
        private UInt16 mIndex;

        public StoredRecord(IStoredView stored, UInt16 index)
        {
            this.mStored = stored;
            this.mIndex = index;
        }

        public UInt16 Key
        {
            get
            {
                UInt16 key, value;

                if (this.mStored.GetRecord(this.mIndex, out key, out value))
                {
                    return key;
                }

                return 0;
            }
        }

        public UInt16 Value
        {
            get
            {
                UInt16 key, value;

                if (this.mStored.GetRecord(this.mIndex, out key, out value))
                {
                    return value;
                }

                return 0;
            }
        }
    }

    public class StoredRecordsEnumerator : IEnumerator<StoredRecord>
    {
        private StoredRecordsModel mStoredValuesModel;
        private int? mIndex;

        public StoredRecordsEnumerator(StoredRecordsModel storedValuesModel)
        {
            this.mStoredValuesModel = storedValuesModel;
            this.mIndex = null;
        }

        public StoredRecord Current
        {
            get
            {
                if (this.mIndex < this.mStoredValuesModel.RowsCount)
                {
                    return this.mStoredValuesModel[this.mIndex.Value];
                }

                throw new IndexOutOfRangeException();
            }
        }

        object IEnumerator.Current
        {
            get
            {
                if (this.mIndex < this.mStoredValuesModel.RowsCount)
                {
                    return this.mStoredValuesModel[this.mIndex.Value];
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

            return (this.mIndex < this.mStoredValuesModel.RowsCount);
        }

        public void Reset()
        {
            this.mIndex = 0;
        }
    }

    public class StoredRecordsModel : IList<StoredRecord>, INotifyCollectionChanged
    {
        private IStoredView mStored;

        public event NotifyCollectionChangedEventHandler CollectionChanged;

        public UInt32 RowsCount { get { return this.mStored.KeysCount; } }

        public StoredRecordsModel()
        {
            this.mStored = null;
        }

        public void UpdateData(IStoredView stored)
        {
            this.mStored = stored;

            if (this.CollectionChanged != null)
            {
                this.CollectionChanged.Invoke(this, new NotifyCollectionChangedEventArgs(NotifyCollectionChangedAction.Reset));
            }
        }

        public StoredRecord this[int index]
        {
            get
            {
                if (index < 0 || index >= this.RowsCount)
                {
                    throw new IndexOutOfRangeException();
                }

                return new StoredRecord(this.mStored, Convert.ToUInt16(index));
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

        public void Add(StoredRecord item)
        {
            throw new NotImplementedException();
        }

        public void Clear()
        {
            throw new NotImplementedException();
        }

        public bool Contains(StoredRecord item)
        {
            throw new NotImplementedException();
        }

        public void CopyTo(StoredRecord[] array, int arrayIndex)
        {
            throw new NotImplementedException();
        }

        public IEnumerator<StoredRecord> GetEnumerator()
        {
            throw new NotImplementedException();
        }

        public int IndexOf(StoredRecord item)
        {
            throw new NotImplementedException();
        }

        public void Insert(int index, StoredRecord item)
        {
            throw new NotImplementedException();
        }

        public bool Remove(StoredRecord item)
        {
            throw new NotImplementedException();
        }

        public void RemoveAt(int index)
        {
            throw new NotImplementedException();
        }

        IEnumerator IEnumerable.GetEnumerator()
        {
            return new StoredRecordsEnumerator(this);
        }
    }
}
