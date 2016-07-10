using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Input;

namespace demo.Classes
{
    public class Commands
    {
        public static RoutedUICommand ApplyConfiguration { get; private set; }

        static Commands()
        {
            ApplyConfiguration = new RoutedUICommand("ApplyConfiguration", "ApplyConfiguration", typeof(Commands));
        }
    }
}
