using System;
using System.Collections.Generic;
using System.Data;
using System.Globalization;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Data;

namespace demo.Classes
{
    public class MemoryToDataViewConverter : IValueConverter
    {
        public object Convert(object value, Type targetType, object parameter, CultureInfo culture)
        {
            var memory = value as MemoryDataSource;

            if (memory == null)
            {
                return null;
            }

            byte[] buffer = memory.GetBufferCopy();

            var columns = 16;
            var rows = buffer.Length / columns;

            var table = new DataTable();

            table.Columns.Add(new DataColumn("Offset"));

            for (var column = 0; column < columns; column++)
            {
                table.Columns.Add(new DataColumn(String.Format("{0:X2}", column)));
            }

            for (var row = 0; row < rows; row++)
            {
                var newRow = table.NewRow();
                newRow["Offset"] = String.Format("{0:x8}", row * columns);

                for (var column = 0; column < columns; column++)
                {
                    var v = buffer[row * columns + column];

                    // Round if parameter is set
                    //if (parameter != null)
                    //{
                    //    int digits;
                    //    if (int.TryParse(parameter.ToString(), out digits))
                    //        v = Math.Round(v, digits);
                    //}

                    newRow[column + 1] = String.Format("{0:X2}", v);
                }

                table.Rows.Add(newRow);
            }


            return table.DefaultView;
        }

        public object ConvertBack(object value, Type targetType, object parameter, CultureInfo culture)
        {
            throw new NotImplementedException();
        }
    }
}
