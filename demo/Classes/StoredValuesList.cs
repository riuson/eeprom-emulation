using System;
using System.Collections.Generic;
using System.Collections.ObjectModel;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace demo.Classes
{
    public class StoredValueItem
    {
        public UInt16 Key { get; set; }
        public UInt16 Value { get; set; }

        public StoredValueItem()
        {
            this.Key = 0;
            this.Value = 0;
        }
    }

    public class StoredValuesList
    {
        public ObservableCollection<StoredValueItem> List { get; private set; }

        public StoredValuesList()
        {
            this.List = new ObservableCollection<StoredValueItem>();
            this.List.Add(new StoredValueItem() { Key = 1, Value = 2 });
            this.List.Add(new StoredValueItem() { Key = 3, Value = 4 });
            this.List.Add(new StoredValueItem() { Key = 1, Value = 2 });
            this.List.Add(new StoredValueItem() { Key = 3, Value = 4 });
            this.List.Add(new StoredValueItem() { Key = 1, Value = 2 });
            this.List.Add(new StoredValueItem() { Key = 3, Value = 4 });
            this.List.Add(new StoredValueItem() { Key = 1, Value = 2 });
            this.List.Add(new StoredValueItem() { Key = 3, Value = 4 });
        }
    }
}
