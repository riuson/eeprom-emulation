using demo.Classes;
using System;
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

namespace demo.Controls.View
{
    /// <summary>
    /// Логика взаимодействия для ControlViewStoredValues.xaml
    /// </summary>
    public partial class ControlViewStoredValues : UserControl
    {
        public static readonly DependencyProperty MemoryDataProperty;

        static ControlViewStoredValues()
        {
            MemoryDataProperty = DependencyProperty.Register("MemoryData",
                typeof(object),
                typeof(ControlViewStoredValues),
                new FrameworkPropertyMetadata(new PropertyChangedCallback(MemoryDataChanged)));
        }

        private static void MemoryDataChanged(DependencyObject d, DependencyPropertyChangedEventArgs e)
        {
        }

        public ControlViewStoredValues()
        {
            InitializeComponent();
            (this.Content as FrameworkElement).DataContext = this;
        }

        public object MemoryData
        {
            get
            {
                return this.GetValue(MemoryDataProperty);
            }
            set
            {
                this.SetValue(MemoryDataProperty, value);
            }
        }
    }
}
