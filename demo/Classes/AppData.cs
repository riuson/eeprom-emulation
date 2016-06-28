using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace demo.Classes
{
    internal class AppData
    {
        public MemoryDataSource MemoryData { get; private set; }
        public MemorySettings MemorySettings { get; private set; }

        public AppData()
        {
            this.MemoryData = new MemoryDataSource();

            this.MemorySettings = new MemorySettings();
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
