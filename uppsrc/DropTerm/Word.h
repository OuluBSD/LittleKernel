#ifndef _DropTerm_Word_h_
#define _DropTerm_Word_h_

NAMESPACE_UPP

class Word : public VfsValueExtCtrl {
public:
	void DragAndDrop(Point, PasteClip& d) override;
	void FrameDragAndDrop(Point, PasteClip& d) override;

protected:
	RichEdit   editor;
	ToolBar    toolbar;
	StatusBar  statusbar;
	String     filename;

	static LRUList& lrufile() { static LRUList l; return l; }

	void Load(const String& filename);
	void OpenFile(const String& fn);
	void Open();
	void Save0();
	void Save();
	void SaveAs();
	void Print();
	void Pdf();
	void About();
	void Destroy();
	void AboutMenu(Bar& bar);
	void MainBar(Bar& bar);
	void ToolMenu(Bar& bar) override;

public:
	typedef Word CLASSNAME;

	static void SerializeApp(Stream& s);
	
	void FileBar(Bar& bar);
	String GetTitle();
	
	Word();
	void Data() override;
	String GetTitle() const override;
	void EditPos(JsonIO& json) override;
	
	Callback WhenTitle;
	
};

void InitWord();

END_UPP_NAMESPACE

#endif
