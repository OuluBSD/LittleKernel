// 1. Hello World GUI (WinForms)
// 2. Simple button with click -> popup
using System;
using System.Windows.Forms;

public static class WF01
{
    public static void HelloWorld()
    {
        Application.EnableVisualStyles();
        using var form = new Form
        {
            Text = "Hello World program",
            ClientSize = new System.Drawing.Size(320, 240)
        };
        var lbl = new Label
        {
            Text = "Hello world!",
            Dock = DockStyle.Fill,
            TextAlign = System.Drawing.ContentAlignment.MiddleCenter
        };
        form.Controls.Add(lbl);
        Application.Run(form);
    }

    public static void ButtonDemo()
    {
        Application.EnableVisualStyles();
        using var form = new Form
        {
            Text = "Button program",
            ClientSize = new System.Drawing.Size(320, 240)
        };
        var btn = new Button
        {
            Text = "Hello world!",
            Left = 30,
            Top = 30,
            Width = 100,
            Height = 30
        };
        btn.Click += (s, e) => MessageBox.Show(form, "Popup message");
        form.Controls.Add(btn);
        Application.Run(form);
    }
}

