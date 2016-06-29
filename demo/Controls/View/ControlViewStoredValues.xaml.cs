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
        public static readonly DependencyProperty ItemsListProperty;

        static ControlViewStoredValues()
        {
            ItemsListProperty = DependencyProperty.Register("ItemsList",
                typeof(object),
                typeof(ControlViewStoredValues),
                new FrameworkPropertyMetadata(new PropertyChangedCallback(ItemsListChanged)));
        }

        private static void ItemsListChanged(DependencyObject d, DependencyPropertyChangedEventArgs e)
        {
        }

        public ControlViewStoredValues()
        {
            InitializeComponent();
            (this.Content as FrameworkElement).DataContext = this;
        }

        public object ItemsList
        {
            get
            {
                return this.GetValue(ItemsListProperty);
            }
            set
            {
                this.SetValue(ItemsListProperty, value);
            }
        }
    }
}
