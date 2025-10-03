using System;
using Avalonia.Controls;
using Avalonia.Interactivity;
using Avalonia.Media;
using System.IO.Ports;

namespace HPM_Companion.Pages;

public partial class MicroControllerTester : UserControl
{
    private const int BAUD_RATE = 115200;
    private bool _greenLightOn = false;
    
    public MicroControllerTester()
    {
        InitializeComponent();
    }

    public void greenLightButton_Click(object sender, RoutedEventArgs e)
    {
        _greenLightOn = !_greenLightOn;

        try
        {
            using (SerialPort port = new SerialPort("COM6", BAUD_RATE))
            {
                port.Parity = Parity.None;
                port.DataBits = 8;
                port.StopBits = StopBits.One;
                port.Open();

                string command;

                if (_greenLightOn)
                {
                    GreenLightIndicator.Foreground = new SolidColorBrush(Color.Parse("#00EE00"));
                    GreenLightIndicator.Text = "On";

                    command = "green_light_on";
                }
                else
                {
                    GreenLightIndicator.Foreground = new SolidColorBrush(Color.Parse("#EEEEEE"));
                    GreenLightIndicator.Text = "Off";

                    command = "green_light_off";
                }

                port.Write(command);
            }
        }
        catch (Exception error)
        {
            Console.WriteLine(error.Message);
        }
    }
}