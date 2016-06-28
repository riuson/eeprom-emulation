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
        public static readonly DependencyProperty WordsOnPageDesiredProperty;
        public static readonly DependencyProperty PagesCountDesiredProperty;

        public static readonly DependencyProperty WordsOnPageProperty;
        public static readonly DependencyProperty PagesCountProperty;
        public static readonly DependencyProperty TotalSizeProperty;
        public static readonly DependencyProperty ActivePageIndexProperty;

        static MemorySettings()
        {
            WordsOnPageDesiredProperty = DependencyProperty.Register("WordsOnPageDesired",
                typeof(UInt32), typeof(MemorySettings),
                new FrameworkPropertyMetadata(
                    256u,
                    FrameworkPropertyMetadataOptions.BindsTwoWayByDefault,
                    new PropertyChangedCallback(MemorySettings.WordsOnPageDesiredChanged),
                    new CoerceValueCallback(MemorySettings.WordsOnPageDesiredCoerce),
                    false,
                    System.Windows.Data.UpdateSourceTrigger.LostFocus),
                new ValidateValueCallback(MemorySettings.IsValidWordsOnPageDesired));

            PagesCountDesiredProperty = DependencyProperty.Register("PagesCountDesired",
                typeof(UInt32), typeof(MemorySettings),
                new FrameworkPropertyMetadata(
                    2u,
                    FrameworkPropertyMetadataOptions.BindsTwoWayByDefault,
                    new PropertyChangedCallback(MemorySettings.PagesCountDesiredChanged),
                    new CoerceValueCallback(MemorySettings.PagesCountDesiredCoerce),
                    false,
                    System.Windows.Data.UpdateSourceTrigger.LostFocus),
                new ValidateValueCallback(MemorySettings.IsValidPagesCountDesired));

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

        #region WordsOnPageDesired
        private static void WordsOnPageDesiredChanged(DependencyObject d, DependencyPropertyChangedEventArgs e)
        {
        }

        private static object WordsOnPageDesiredCoerce(DependencyObject d, object baseValue)
        {
            UInt32 v = Convert.ToUInt32(baseValue);

            if (v < 1024u)
            {
                return baseValue;
            }

            return 1024u;
        }

        private static bool IsValidWordsOnPageDesired(object value)
        {
            return true;
        }

        public UInt32 WordsOnPageDesired
        {
            get
            {
                return Convert.ToUInt32(this.GetValue(MemorySettings.WordsOnPageDesiredProperty));
            }
            set
            {
                this.SetValue(MemorySettings.WordsOnPageDesiredProperty, value);
            }
        }

        #endregion

        #region PagesCountDesired
        private static void PagesCountDesiredChanged(DependencyObject d, DependencyPropertyChangedEventArgs e)
        {
        }

        private static object PagesCountDesiredCoerce(DependencyObject d, object baseValue)
        {
            UInt32 v = Convert.ToUInt32(baseValue);

            if (v < 10u)
            {
                return baseValue;
            }

            return 10u;
        }

        private static bool IsValidPagesCountDesired(object value)
        {
            return true;
        }

        public UInt32 PagesCountDesired
        {
            get
            {
                return Convert.ToUInt32(this.GetValue(MemorySettings.PagesCountDesiredProperty));
            }
            set
            {
                this.SetValue(MemorySettings.PagesCountDesiredProperty, value);
            }
        }

        #endregion

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
