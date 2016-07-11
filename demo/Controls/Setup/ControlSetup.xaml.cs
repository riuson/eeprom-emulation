using demo.Classes;
using System;
using System.Windows;
using System.Windows.Controls;

namespace demo.Controls.Setup
{
    /// <summary>
    /// Логика взаимодействия для ControlSetup.xaml
    /// </summary>
    public partial class ControlSetup : UserControl
    {
        public static readonly DependencyProperty WordsOnPageDesiredProperty;
        public static readonly DependencyProperty PagesCountDesiredProperty;

        public static readonly DependencyProperty WordsOnPageProperty;
        public static readonly DependencyProperty PagesCountProperty;
        public static readonly DependencyProperty TotalSizeProperty;

        static ControlSetup()
        {
            WordsOnPageDesiredProperty = DependencyProperty.Register("WordsOnPageDesired",
                typeof(UInt32),
                typeof(ControlSetup),
                new FrameworkPropertyMetadata(
                    0u,
                    FrameworkPropertyMetadataOptions.BindsTwoWayByDefault,
                    new PropertyChangedCallback(WordsOnPageDesiredChangedCallback),
                    new CoerceValueCallback(WordsOnPageDesiredCoerceValueCallback)));

            PagesCountDesiredProperty = DependencyProperty.Register("PagesCountDesired",
                typeof(UInt32),
                typeof(ControlSetup),
                new FrameworkPropertyMetadata(
                    0u,
                    FrameworkPropertyMetadataOptions.BindsTwoWayByDefault,
                    new PropertyChangedCallback(PagesCountDesiredChangedCallback),
                    new CoerceValueCallback(PagesCountDesiredCoerceValueCallback)));

            WordsOnPageProperty = DependencyProperty.Register("WordsOnPage",
                typeof(UInt32),
                typeof(ControlSetup),
                new FrameworkPropertyMetadata());

            PagesCountProperty = DependencyProperty.Register("PagesCount",
                typeof(UInt32),
                typeof(ControlSetup),
                new FrameworkPropertyMetadata());

            TotalSizeProperty = DependencyProperty.Register("TotalSize",
                typeof(UInt32),
                typeof(ControlSetup),
                new FrameworkPropertyMetadata());
        }

        public ControlSetup()
        {
            InitializeComponent();
            (this.Content as FrameworkElement).DataContext = this;
        }

        #region WordsOnPageDesired
        public UInt32 WordsOnPageDesired
        {
            get
            {
                return Convert.ToUInt32(this.GetValue(WordsOnPageDesiredProperty));
            }
            set
            {
                this.SetValue(WordsOnPageDesiredProperty, value);
            }
        }

        private static void WordsOnPageDesiredChangedCallback(DependencyObject d, DependencyPropertyChangedEventArgs e)
        {
        }

        private static object WordsOnPageDesiredCoerceValueCallback(DependencyObject d, object baseValue)
        {
            UInt32 wordsOnPage = Convert.ToUInt32(baseValue);
            return Math.Min(wordsOnPage, 1024u);
        }
        #endregion

        #region PagesCountDesired
        public UInt32 PagesCountDesired
        {
            get
            {
                return Convert.ToUInt32(this.GetValue(PagesCountDesiredProperty));
            }
            set
            {
                this.SetValue(PagesCountDesiredProperty, value);
            }
        }

        private static void PagesCountDesiredChangedCallback(DependencyObject d, DependencyPropertyChangedEventArgs e)
        {
        }

        private static object PagesCountDesiredCoerceValueCallback(DependencyObject d, object baseValue)
        {
            UInt32 wordsOnPage = Convert.ToUInt32(baseValue);
            return Math.Min(wordsOnPage, 10u);
        }
        #endregion

        public UInt32 WordsOnPage
        {
            get
            {
                return Convert.ToUInt32(this.GetValue(WordsOnPageProperty));
            }
            set
            {
                this.SetValue(WordsOnPageProperty, value);
            }
        }

        public UInt32 PagesCount
        {
            get
            {
                return Convert.ToUInt32(this.GetValue(PagesCountProperty));
            }
            set
            {
                this.SetValue(PagesCountProperty, value);
            }
        }

        public UInt32 TotalSize
        {
            get
            {
                return Convert.ToUInt32(this.GetValue(TotalSizeProperty));
            }
            set
            {
                this.SetValue(TotalSizeProperty, value);
            }
        }

        private void OnApplyClick(object sender, RoutedEventArgs e)
        {
            Commands.ApplyConfiguration.Execute(new Tuple<UInt32, UInt32>(this.WordsOnPageDesired, this.PagesCountDesired), (Button)sender);
        }
    }
}
