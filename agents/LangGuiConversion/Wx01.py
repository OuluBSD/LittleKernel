
# 1. Hello World GUI (wxPython)
# 2. Simple button with click -> message box
import wx


def hello_world():
    app = wx.App(False)
    frame = wx.Frame(None, title="Hello World program", size=(320, 240))
    panel = wx.Panel(frame)
    sizer = wx.BoxSizer(wx.VERTICAL)
    label = wx.StaticText(panel, label="Hello world!")
    sizer.AddStretchSpacer(1)
    sizer.Add(label, 0, wx.ALIGN_CENTER)
    sizer.AddStretchSpacer(1)
    panel.SetSizer(sizer)
    frame.Centre()
    frame.Show()
    app.MainLoop()


def button_demo():
    app = wx.App(False)
    frame = wx.Frame(None, title="Button program", size=(320, 240))
    panel = wx.Panel(frame)
    btn = wx.Button(panel, label="Hello world!", pos=(30, 30), size=(100, 30))

    def on_click(_evt):
        wx.MessageBox("Popup message", "", wx.OK | wx.ICON_INFORMATION, parent=frame)

    btn.Bind(wx.EVT_BUTTON, on_click)
    frame.Centre()
    frame.Show()
    app.MainLoop()
