using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows;
using System.Windows.Forms;
using System.Windows.Media;

namespace Harleys.HoverUX
{

    public enum MiddleResnderPositions { 
        TOP,
        BOTTOM,
        LEFT,
        RIGHT
    }


    public class WidgetRenderer
    {
        //locals
        private Window wind;
        private double screenWidth = -1, screenHeight = -1;
        private double startBarPadding = 20; //only used when rendering at the bottom
        private double padding = 5; //default value
        private double left = -1, right = -1, top = -1, bottom = -1;


        //private function to fetch the pre calculated details
        private void getScreenResolution() {
            //check if already found
            if (right == -1 || left == -1 || top == -1 || bottom == -1) {
                //get the screen resolution
                screenWidth = System.Windows.SystemParameters.PrimaryScreenWidth;
                screenHeight = System.Windows.SystemParameters.PrimaryScreenHeight;

                //get the taskbar height
                int PSBH = System.Windows.Forms.Screen.PrimaryScreen.Bounds.Height;
                startBarPadding = PSBH - System.Windows.Forms.Screen.PrimaryScreen.WorkingArea.Height;

                //calculate the positions
                top = padding;
                bottom = screenHeight - (wind.Height + padding + startBarPadding);
                left = padding;
                right = screenWidth - (wind.Width + padding);
            }
            return;
        }


        //Is the initiate function for a widget
        //sets some of the must have widget properties for the widget window
        //already gets called at the time of object creation
        public void setWidgetMusts() {
            wind.ShowInTaskbar = false;
            wind.ResizeMode = ResizeMode.NoResize;
            wind.WindowStyle = WindowStyle.None;
            wind.AllowsTransparency = true;
        }


        //render at one of the 4 corners
        public void renderAt(bool isTop, bool isLeft) {
            wind.Top = (isTop) ? top : bottom;
            wind.Left = (isLeft) ? left : right;
        }

        //render at the given position
        public void renderAt(double x, double y) {
            wind.Left = x;
            wind.Top = y;
        }

        //render at the middle of the 4 sides
        public void renderAt(MiddleResnderPositions middleRenderPositions) {
            switch (middleRenderPositions) {
                case MiddleResnderPositions.TOP:
                    wind.Top = top;
                    wind.Left = (screenWidth / 2) - (wind.Width / 2);
                    break;
                case MiddleResnderPositions.BOTTOM:
                    wind.Top = bottom;
                    wind.Left = (screenWidth / 2) - (wind.Width / 2);
                    break;
                case MiddleResnderPositions.LEFT:
                    wind.Left = left;
                    wind.Top = (screenWidth / 2);
                    break;
                case MiddleResnderPositions.RIGHT:
                    wind.Left = right;
                    wind.Top = (screenWidth / 2);
                    break;
                default:
                    return;
            }
        }

        //constructor
        public WidgetRenderer(Window window, bool isTop = false, bool isLeft = false, double padding = 5){
            //store the operatable window
            this.padding = padding;
            wind = window;
            //get the screen resolution
            getScreenResolution();
            //set musts for a widget
            setWidgetMusts();
            //reander at
            renderAt(isTop, isLeft);
        }

        //constructor
        public WidgetRenderer(Window window, MiddleResnderPositions middleRenderPositions, double padding = 5) {
            //store the operatable window
            this.padding = padding;
            wind = window;
            //get the screen resolution
            getScreenResolution();
            //set musts for a widget
            setWidgetMusts();
            //reander at
            renderAt(middleRenderPositions);
        }
                
    }
}
