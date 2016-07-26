using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows;

namespace demo.Classes
{
    internal class MemorySettings : DependencyObject
    {
        public static readonly DependencyProperty WordsOnPageProperty;
        public static readonly DependencyProperty PagesCountProperty;
        public static readonly DependencyProperty TotalSizeProperty;
        public static readonly DependencyProperty ActivePageIndexProperty;

        static MemorySettings()
        {
            WordsOnPageProperty = DependencyProperty.Register("WordsOnPage",
                typeof(UInt32), typeof(MemorySettings),
                new FrameworkPropertyMetadata());

            PagesCountProperty = DependencyProperty.Register("PagesCount",
                typeof(UInt32), typeof(MemorySettings),
                new FrameworkPropertyMetadata());

            TotalSizeProperty = DependencyProperty.Register("TotalSize",
                typeof(UInt32), typeof(MemorySettings),
                new FrameworkPropertyMetadata());

            ActivePageIndexProperty = DependencyProperty.Register("ActivePageIndex",
                typeof(UInt32), typeof(MemorySettings),
                new FrameworkPropertyMetadata());
        }

        #region WordsOnPage

        public UInt32 WordsOnPage
        {
            get
            {
                return Convert.ToUInt32(this.GetValue(MemorySettings.WordsOnPageProperty));
            }
            set
            {
                this.SetValue(MemorySettings.WordsOnPageProperty, value);
            }
        }

        #endregion

        #region PagesCount

        public UInt32 PagesCount
        {
            get
            {
                return Convert.ToUInt32(this.GetValue(MemorySettings.PagesCountProperty));
            }
            set
            {
                this.SetValue(MemorySettings.PagesCountProperty, value);
            }
        }

        #endregion

        #region TotalSize

        public UInt32 TotalSize
        {
            get
            {
                return Convert.ToUInt32(this.GetValue(MemorySettings.TotalSizeProperty));
            }
            set
            {
                this.SetValue(MemorySettings.TotalSizeProperty, value);
            }
        }

        #endregion

        #region ActivePageIndex

        public UInt32 ActivePageIndex
        {
            get
            {
                return Convert.ToUInt32(this.GetValue(MemorySettings.ActivePageIndexProperty));
            }
            set
            {
                this.SetValue(MemorySettings.ActivePageIndexProperty, value);
            }
        }

        #endregion

        public void Initialize_Click(object sender, RoutedEventArgs e)
        {
        }
    }
}
