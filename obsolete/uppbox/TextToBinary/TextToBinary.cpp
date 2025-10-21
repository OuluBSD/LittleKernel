#include <CtrlLib/CtrlLib.h>
using namespace UPP;


int ToHex(int chr) {
	if (chr >= '0' && chr <= '9')
		return chr - '0';
	else if (chr >= 'a' && chr <= 'f')
		return 10 + chr - 'a';
	else if (chr >= 'A' && chr <= 'F')
		return 10 + chr - 'A';
	return -1;
}

GUI_APP_MAIN {
	TopWindow tw;
	tw.Sizeable().MaximizeBox().MinimizeBox().Title("Text To Binary");
	DocEdit edit;
	Button save, savenibbles;
	tw.Add(edit.VSizePos(0,30).HSizePos());
	tw.Add(save.BottomPos(1,28).RightPos(1,100));
	tw.Add(savenibbles.BottomPos(1,28).RightPos(101,100));
	save.SetLabel("Save");
	savenibbles.SetLabel("Save Nibbles");
	edit.SetFocus();
	
	save.WhenAction = [&]{
		String data = edit.GetData();
		if (data.IsEmpty()) return;
		
		String path;
		FileSelNative sel;
		sel.Set(GetDataFile(""));
		if (sel.ExecuteSaveAs("Save as")) {
			String path = sel.Get();
			FileOut fout(path);
			for(int i = 0; i+1 < data.GetCount(); i+=2) {
				int c0 = data[i];
				if (c0 == '\n' || c0 == '\r' || c0 == ' ' || c0 == '\t') {
					i--; // -1 + 2 = 1
					continue;
				}
				int c1 = data[i+1];
				int b0 = ToHex(data[i]);
				int b1 = ToHex(data[i+1]);
				if (b0 < 0 || b1 < 0) {
					PromptOK("Conversion failed");
					break;
				}
				byte b = (byte)b0 << 4 | (byte)b1;
				fout.Put(b);
			}
		}
	};
	
	savenibbles.WhenAction = [&]{
		String data = edit.GetData();
		if (data.IsEmpty()) return;
		
		String path;
		FileSelNative sel;
		sel.Set(GetDataFile(""));
		if (sel.ExecuteSaveAs("Save as")) {
			String path = sel.Get();
			FileOut fout(path);
			for(int i = 0; i < data.GetCount(); i++) {
				int c0 = data[i];
				if (c0 == '\n' || c0 == '\r' || c0 == ' ' || c0 == '\t')
					continue;
				int b0 = ToHex(data[i]);
				if (b0 < 0) {
					PromptOK("Conversion failed");
					break;
				}
				fout.Put(b0);
			}
		}
	};
	
	tw.Run();
}

