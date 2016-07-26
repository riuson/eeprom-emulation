using System;
using System.Globalization;
using System.Windows.Data;

namespace demo.Classes
{
    public class ByteTypeEqualsConverter : IValueConverter
    {
        public static readonly IValueConverter Instance = new ByteTypeEqualsConverter();

        public object Convert(object value, Type targetType, object parameter, CultureInfo culture)
        {
            ByteType actualValue = (ByteType)value;
            ByteType comparedValue = (ByteType)parameter;

            return (actualValue & comparedValue) == comparedValue;
        }

        public object ConvertBack(object value, Type targetType, object parameter, CultureInfo culture)
        {
            throw new NotImplementedException();
        }
    }
}
