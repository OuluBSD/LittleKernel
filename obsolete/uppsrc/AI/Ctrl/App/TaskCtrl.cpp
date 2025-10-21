#include "App.h"

NAMESPACE_UPP

TaskCtrl::TaskCtrl()
{
	Add(hsplit.SizePos());

	hsplit.Horz() << list << vsplit;
	vsplit.Vert() << input << output;

	list.AddColumn(t_("#"));
	list.AddColumn(t_("Description"));
	list.AddColumn(t_("Status"));
	list.ColumnWidths("1 4 4");
	list <<= THISBACK(DataTask);

	output <<= THISBACK(ValueChange);

	list.WhenBar << THISBACK(OutputMenu);
	
	tc.Set(-200, THISBACK(Data));
}

TaskCtrl::~TaskCtrl()
{
	tc.Kill();
}

void TaskCtrl::Data()
{
	TaskMgr& m = AiTaskManager();

	for(int i = 0; i < m.tasks.GetCount(); i++) {
		AiTask& t = m.tasks[i];
		list.Set(i, 0, i);
		list.Set(i, 1, t.GetDescription());
		String s;
		if(t.failed)
			s = t_("Error") + String(": ") + t.error;
		else if(t.processing)
			s = t_("Processing");
		else if(t.ready)
			s = t_("Ready");
		if(t.tries > 0)
			s << " (" << t_("tries") << " " << t.tries << ")";
		list.Set(i, 2, s);
	}
	list.SetCount(m.tasks.GetCount());

	int cursor = 0;
	if(!list.IsCursor() && cursor >= 0 && cursor < list.GetCount())
		list.SetCursor(cursor);

#if 0
	prog.Set(m.actual, max(1,m.total));
	lbl.SetLabel(m.status.IsEmpty() ? String(t_("Idle")) : m.status);
#endif

	DataTask();
}

void TaskCtrl::DataTask()
{
	TaskMgr& m = AiTaskManager();

	if(!list.IsCursor())
		return;

	int cursor = list.GetCursor();
	AiTask& t = m.tasks[cursor];
	m.active_task = &t;

	if(cursor != data_cursor || (output.GetLength() == 0 && t.output.GetCount())) {
		/*if(t.completion)
			input.SetData(t.completion->prompt);
		else if(!t.input_json.IsEmpty())
			input.SetData(t.input_json.AsJSON(true));
		else
			input.SetData(t.input_basic.AsString().ToWString());*/
		input.SetData(t.MakeInputString(true));
		output.SetData(t.output.ToWString());
		data_cursor = cursor;
	}
}

void TaskCtrl::ValueChange()
{
	TaskMgr& m = AiTaskManager();

	if(!m.active_task)
		return;

	AiTask& t = *m.active_task;
	t.output = TrimBoth(output.GetData());
	t.changed = true;
	t.output.Replace("\r", "");

	t.Store();
}

void TaskCtrl::ProcessItem()
{
	TaskMgr& m = AiTaskManager();

	if(!list.IsCursor())
		return;
	int cursor = list.GetCursor();
	AiTask& t = m.tasks[cursor];
	t.failed = false;
	m.StartSingle(cursor);
}

void TaskCtrl::RetryItem(bool skip_prompt, bool skip_cache)
{
	TaskMgr& m = AiTaskManager();

	if(!list.IsCursor())
		return;
	int cursor = list.GetCursor();
	AiTask& t = m.tasks[cursor];
	t.Retry(skip_prompt, skip_cache);
	m.StartSingle(cursor);
	this->output.Clear();
}

void TaskCtrl::ReturnFail()
{
	TaskMgr& m = AiTaskManager();

	if(!list.IsCursor())
		return;
	int cursor = list.GetCursor();
	AiTask& t = m.tasks[cursor];
	t.ReturnFail();
	this->output.SetData("fail");
}

void TaskCtrl::OutputMenu(Bar& bar)
{
	bar.Add(t_("Process output"), THISBACK(ProcessItem));
	bar.Add(t_("Retry"), THISBACK2(RetryItem, false, false));
	bar.Add(t_("Retry cached prompt"), THISBACK2(RetryItem, true, false));
	bar.Add(t_("Retry and skip cache"), THISBACK2(RetryItem, false, true));
	bar.Add(t_("Return fail"), THISBACK(ReturnFail));
}

END_UPP_NAMESPACE
