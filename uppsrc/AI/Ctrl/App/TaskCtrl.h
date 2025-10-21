#ifndef _AI_TaskCtrl_h_
#define _AI_TaskCtrl_h_

NAMESPACE_UPP

class TaskCtrl : public Ctrl {
	Splitter hsplit, vsplit;
	ArrayCtrl list;
	DocEdit input, output;
	int data_cursor = -1;
	TimeCallback tc;
public:
	typedef TaskCtrl CLASSNAME;
	TaskCtrl();
	~TaskCtrl();

	void Data();
	void DataTask();
	void ToolMenu(Bar& bar) {}
	void ValueChange();
	void ProcessItem();
	void ReturnFail();
	void RetryItem(bool skip_prompt, bool skip_cache);
	void OutputMenu(Bar& bar);
};

END_UPP_NAMESPACE

#endif
 
