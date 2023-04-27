using Harleys.HoverUX;
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
using System.Windows.Shapes;

namespace Harleys
{
    /// <summary>
    /// Interaction logic for HarleysSecrets.xaml
    /// </summary>
    public partial class HarleysSecrets : Window
    {
        public WidgetRenderer widgetRenderer;

        public HarleysSecrets()
        {
            InitializeComponent();

            //initiate as a widget
            widgetRenderer = new WidgetRenderer(this);
        }
    }
}
