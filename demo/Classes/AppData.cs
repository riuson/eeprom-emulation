using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace demo.Classes
{
    internal class AppData
    {
        public MemorySettings MemorySettings { get; private set; }

        public MemoryDataSource MemoryData { get; private set; }
        public StoredValuesList StoredValues { get; private set; }

        public AppData()
        {
            this.MemorySettings = new MemorySettings();

            this.MemoryData = new MemoryDataSource();
            this.StoredValues = new StoredValuesList();
        }

        internal void Reinitialize()
        {
            var result = this.MemoryData.Initialize(this.MemorySettings.WordsOnPageDesired, this.MemorySettings.PagesCountDesired);

            if (result == testlib.Wrapper.Eeprom.Result.Success)
            {
                this.MemorySettings.WordsOnPage = this.MemoryData.Config.WordsOnPage;
                this.MemorySettings.PagesCount = this.MemoryData.Config.PagesCount;
                this.MemorySettings.TotalSize = this.MemoryData.Config.TotalSize;
            }
        }
    }
}
