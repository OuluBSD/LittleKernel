
// 1. Hello World GUI (WPF)
// 2. Simple button with click -> popup
using System;
using System.Windows;
using System.Windows.Controls;

public static class WPF01
{
    [STAThread]
    public static void HelloWorld()
    {
        var app = new Application();
        var win = new Window
        {
            Title = "Hello World program",
            Width = 320,
            Height = 240,
            WindowStartupLocation = WindowStartupLocation.CenterScreen
        };
        var lbl = new Label
        {
            Content = "Hello world!",
            HorizontalContentAlignment = HorizontalAlignment.Center,
            VerticalContentAlignment = VerticalAlignment.Center
        };
        win.Content = lbl;
        app.Run(win);
    }

    [STAThread]
    public static void ButtonDemo()
    {
        var app = new Application();
        var win = new Window
        {
            Title = "Button program",
            Width = 320,
            Height = 240,
            WindowStartupLocation = WindowStartupLocation.CenterScreen
        };
        var btn = new Button
        {
            Content = "Hello world!",
            Width = 100,
            Height = 30,
            HorizontalAlignment = HorizontalAlignment.Left,
            VerticalAlignment = VerticalAlignment.Top,
            Margin = new Thickness(30, 30, 0, 0)
        };
        btn.Click += (s, e) => MessageBox.Show(win, "Popup message");
        var grid = new Grid();
        grid.Children.Add(btn);
        win.Content = grid;
        app.Run(win);
    }
}
