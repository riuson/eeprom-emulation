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

namespace demo
{
    /// <summary>
    /// Логика взаимодействия для MainWindow.xaml
    /// </summary>
    public partial class MainWindow : Window
    {
        private AppData Data { get; set; }

        public MainWindow()
        {
            InitializeComponent();

            this.Data = new AppData();

            this.Data.Reinitialize(128, 1);

            this.DataContext = this.Data;

            this.Data.UpdateSubscribers();
        }

        private void ApplyConfiguration_Executed(object sender, ExecutedRoutedEventArgs e)
        {
            var args = e.Parameter as Tuple<UInt32, UInt32, UInt32>;

            if (args != null)
            {
                this.Data.Reinitialize(args.Item1, args.Item2);
                this.Data.GenerateTestValuesList(args.Item3);
                this.Data.UpdateSubscribers();
            }
        }

        private void StoreValue_Executed(object sender, ExecutedRoutedEventArgs e)
        {
            var args = e.Parameter as Tuple<UInt16, UInt16>;

            if (args != null)
            {
                var res = this.Data.MemoryData.Write(args.Item1, args.Item2);
                this.Data.UpdateSubscribers();

                if (res != testlib.Wrapper.Eeprom.Result.Success)
                {
                    MessageBox.Show(res.ToString(), "Store value result");
                }
            }
        }
    }
}
