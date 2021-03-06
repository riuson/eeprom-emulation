﻿using System;
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
        public StoredRecordsModel StoredModel { get; private set; }
        public MemoryModel MemoryModel { get; private set; }

        public AppData()
        {
            this.MemorySettings = new MemorySettings();

            this.MemoryData = new MemoryDataSource();
            this.MemoryModel = new MemoryModel();
            this.MemoryModel.UpdateData(this.MemoryData, 16);

            this.StoredModel = new StoredRecordsModel();
            this.StoredModel.UpdateData(this.MemoryData);
        }

        internal void Reinitialize(UInt32 wordsOnPageDesired, UInt32 pagesCountDesired)
        {
            var result = this.MemoryData.Initialize(wordsOnPageDesired, pagesCountDesired);

            if (result == testlib.Wrapper.Eeprom.Result.Success)
            {
                this.MemorySettings.WordsOnPage = this.MemoryData.Config.WordsOnPage;
                this.MemorySettings.PagesCount = this.MemoryData.Config.PagesCount;
                this.MemorySettings.TotalSize = this.MemoryData.Config.TotalSize;
            }
        }

        public void UpdateSubscribers()
        {
            this.MemoryModel.UpdateData(this.MemoryData, 16);
            this.StoredModel.UpdateData(this.MemoryData);
        }

        public void GenerateTestValuesList(UInt32 count)
        {
            for (ushort i = 0; i < count; i++)
            {
                this.MemoryData.Write(i, i);
            }
        }
    }
}
