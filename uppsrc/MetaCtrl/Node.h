#ifndef _MetaCtrl_Node_h_
#define _MetaCtrl_Node_h_

class WidgetCtrl : public Ctrl {
	
public:
	typedef WidgetCtrl CLASSNAME;
	WidgetCtrl();
	virtual ~WidgetCtrl();
	
	virtual void Data() = 0;
	virtual void DataTree(TreeCtrl& tree);
	virtual void ToolMenu(Bar& bar) = 0;
	virtual void EditPos(JsonIO& json) {};
	virtual void Initialize(Value args) {}
	virtual String GetTitle() const {return String();}
	virtual Value* GetPolyValue() {return 0;}
	virtual VfsPath GetCursorPath() const = 0;
	Event<> WhenTitle;
};

struct VfsValueExtCtrl : WidgetCtrl {
	Ptr<VfsValueExt> ext;
	Event<> WhenEditorChange;
	Event<> WhenTitle;
	Event<> WhenSaveEditPos;
	Ptr<ToolAppCtrl> owner;
	
	VfsValueExtCtrl();
	VfsValue& GetValue();
	const VfsValue& GetValue() const;
	VfsValueExt& GetExt();
	String GetFilePath() const;
	VfsPath GetCursorPath() const override;
	void ToolMenu(Bar& bar) override;
	
	template <class T> T& GetExt() {return dynamic_cast<T&>(*ext);}
	template <class T> T* FindExt() {return dynamic_cast<T*>(&*ext);}
};

#endif
