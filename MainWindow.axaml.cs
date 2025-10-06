using System;
using Avalonia.Controls;
using Avalonia.Interactivity;
using Avalonia.Media;
using System.IO.Ports;
using HPM_Companion.Pages;


namespace HPM_Companion;

public enum PageType
{
    Fingerprints,
    Accounts,
    MicroControllerTester
}
public partial class MainWindow : Window
{
    public string Greeting => "Hello World";
    public MainWindow()
    {
        InitializeComponent();
        MainContent.Content = new Fingerprints();
    }

// Event handler for all navigation buttons
    private void Button_Click(object sender, RoutedEventArgs e)
    {
        if (sender is Button button && button.Tag is PageType pageType)
        {
            Console.WriteLine("Button tag: " + button.Tag);
            LoadPage(pageType);
        }
    }

    // Method to switch the displayed UserControl
    private void LoadPage(PageType pageType)
    {
        UserControl newPage = pageType switch
        {
            PageType.Fingerprints => new Fingerprints(),
            PageType.Accounts => new Accounts(),
            PageType.MicroControllerTester => new Fingerprints(),
            _ => throw new InvalidOperationException($"Unknown PageType: {pageType}")
        };

        // Set the ContentControl's content to the new UserControl instance
        MainContent.Content = newPage;
    }
}
