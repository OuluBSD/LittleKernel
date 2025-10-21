#ifndef _ide_Shell_SmallWidgets_h_
#define _ide_Shell_SmallWidgets_h_

NAMESPACE_UPP

namespace Widget {
	
struct DraftPad : WidgetCtrl {
	DraftPad();
	DocEdit edit;
	void Data() override;
	void ToolMenu(Bar& bar) override;
	String GetTitle() const override {return "DraftPad";}
	VfsPath GetCursorPath() const override {return VfsPath();}
};

struct BlogPad : WidgetCtrl{
	typedef BlogPad CLASSNAME;
	DocEdit edit;
	String path, find_group;
	TimeCallback tc;
	BlogPad();
	~BlogPad();
	void OnChange();
	void Save();
	void ToolMenu(Bar& bar) override;
	void Data() override;
	void Initialize(Value args) override;
	String GetTitle() const override {return "BlogPad";}
	VfsPath GetCursorPath() const override {return VfsPath();}
};

struct Timer : WithTimerLayout<WidgetCtrl> {
	typedef Timer CLASSNAME;
	Timer();
	void ToggleStart();
	void Check();
	virtual void OnReady();
	void Data() override;
	void ToolMenu(Bar& bar) override;
	String GetTitle() const override {return "Timer";}
	VfsPath GetCursorPath() const override {return VfsPath();}
	Time ready;
	bool running = false;
	TimeCallback tc;
};

}

END_UPP_NAMESPACE

#endif
