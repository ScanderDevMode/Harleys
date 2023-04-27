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
using Harleys.HoverUX;

namespace Harleys
{
    /// <summary>
    /// Interaction logic for MainWindow.xaml
    /// </summary>
    public partial class MainWindow : Window
    {
        WidgetRenderer? widgetRenderer = null;

        public MainWindow()
        {
            InitializeComponent();

            widgetRenderer = new WidgetRenderer(this, MiddleResnderPositions.BOTTOM);

            //hides the main window
            //this.Visibility = Visibility.Hidden; //hidden for now, will be used later for settings panel


            //init the different components of the application

            //init Harleys Secrets - The Password Manager
            HarleysSecrets hs = new HarleysSecrets();
            hs.Show();
            
            
        }
    }
}
