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

            this.Data.MemorySettings.PagesCountDesired = 1;
            this.Data.MemorySettings.WordsOnPageDesired = 128;
            this.Data.Reinitialize();

            this.DataContext = this.Data;
        }

        private void ControlSetup_Apply(object sender, EventArgs e)
        {
            this.Data.Reinitialize();
        }
    }
}
