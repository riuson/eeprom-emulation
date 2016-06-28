﻿using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Documents;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using System.Windows.Navigation;
using System.Windows.Shapes;

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
                    FrameworkPropertyMetadataOptions.BindsTwoWayByDefault));

            PagesCountDesiredProperty = DependencyProperty.Register("PagesCountDesired",
                typeof(UInt32),
                typeof(ControlSetup),
                new FrameworkPropertyMetadata(
                    0u,
                    FrameworkPropertyMetadataOptions.BindsTwoWayByDefault));

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
            this.Apply?.Invoke(this, EventArgs.Empty);
        }

        public event EventHandler Apply;
    }
}
