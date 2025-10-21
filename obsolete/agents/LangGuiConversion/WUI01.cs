// 1. Hello World GUI (WinUI 3)
// 2. Simple button with click -> ContentDialog
// Note: In a real project, these windows run within a WinUI 3 App.
using Microsoft.UI.Xaml;
using Microsoft.UI.Xaml.Controls;

public sealed class WUI01_HelloWindow : Window
{
    public WUI01_HelloWindow()
    {
        Title = "Hello World program";
        var grid = new Grid();
        var text = new TextBlock
        {
            Text = "Hello world!",
            HorizontalAlignment = HorizontalAlignment.Center,
            VerticalAlignment = VerticalAlignment.Center
        };
        grid.Children.Add(text);
        Content = grid;
        Activate();
    }
}

public sealed class WUI01_ButtonWindow : Window
{
    public WUI01_ButtonWindow()
    {
        Title = "Button program";
        var grid = new Grid();
        var btn = new Button
        {
            Content = "Hello world!",
            Width = 100,
            Height = 30,
            HorizontalAlignment = HorizontalAlignment.Left,
            VerticalAlignment = VerticalAlignment.Top,
            Margin = new Thickness(30, 30, 0, 0)
        };
        btn.Click += async (s, e) =>
        {
            var dialog = new ContentDialog
            {
                Title = "",
                Content = "Popup message",
                CloseButtonText = "OK",
                XamlRoot = (Content as FrameworkElement)?.XamlRoot
            };
            await dialog.ShowAsync();
        };
        grid.Children.Add(btn);
        Content = grid;
        Activate();
    }
}

