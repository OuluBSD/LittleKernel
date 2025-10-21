#include "Shell.h"
#include <ide/ide.h>

NAMESPACE_UPP

namespace Widget {

DraftPad::DraftPad() {
	Add(edit.SizePos());
}

void DraftPad::Data() {
	
}

void DraftPad::ToolMenu(Bar& bar) {
	bar.Add("Clear all", [this]{edit.Clear();});
}


BlogPad::BlogPad() {
	Add(edit.SizePos());
	edit.WhenAction = THISBACK(OnChange);
	PostCallback([this]{edit.SetFocus(); edit.SetCursor(edit.GetLength());});
}

BlogPad::~BlogPad() {
	Save();
}

void BlogPad::OnChange() {
	tc.Set(1000, THISBACK(Save));
}

void BlogPad::ToolMenu(Bar& bar) {
	bar.Add("Save", THISBACK(Save));
	bar.Add("Clear all", [this]{edit.Clear();});
}

void BlogPad::Initialize(Value args) {
	if (args.Is<ValueArray>()) {
		ValueArray arr = args;
		if (arr.GetCount())
			find_group = arr.Get(0);
	}
}

void BlogPad::Save() {
	if (!path.IsEmpty()) {
		String content = edit.GetData();
		FileOut fout(path);
		fout << content;
	}
}

void BlogPad::Data() {
	WorkspaceWork& wspc = *dynamic_cast<Ide*>(TheIdeContext());
	
	Time now = GetSysTime();
	int q = 1 + (now.month-1) / 3;
	String pkg = IntStr(now.year) + "Q" + IntStr(q);
	String file = Format("%Mon%02d", now.month, now.day);
	RealizeDirectory(pkg);
	
	wspc.FindSetPackage(pkg);
	
	String cur_group, found_file;
	
	if (find_group.IsEmpty())
		find_group = "Generic";
	
	bool enable = false;
	int group_begin = -1;
	int group_end = -1;
	for(int i = 0; i < wspc.fileindex.GetCount(); i++) {
		String name = wspc.FileName(i);
		if (wspc.IsSeparator(i)) {
			if (enable)
				group_end = i;
			enable = ToLower(name) == ToLower(find_group);
			cur_group = name;
			if (enable)
				group_begin = i+1;
		}
	}
	if (group_end < 0 && enable) group_end = wspc.fileindex.GetCount();
	
	if (group_begin < 0) {
		wspc.AddItem(find_group, true, false);
		group_begin = group_end = wspc.fileindex.GetCount();
	}
	
	int pos = -1;
	for(int i = group_begin; i < group_end; i++) {
		String name = wspc.FileName(i);
		if (name.Find(file) == 0 && GetFileExt(name) == ".txt") {
			pos = i;
			found_file = name;
			break;
		}
	}
	if (pos < 0) {
		String uniq_file;
		for(int i = 0; i < 100; i++) {
			uniq_file = file;
			if (i)
				uniq_file.Cat('_', i);
			uniq_file += ".txt";
			String path = AppendFileName(wspc.GetActivePackageDir(), uniq_file);
			if (!FileExists(path))
				break;
		}
		wspc.filelist.SetCursor(group_end);
		wspc.AddItem(uniq_file, false, false);
		pos = group_end;
		found_file = uniq_file;
	}
	
	path = AppendFileName(wspc.GetActivePackageDir(), found_file);
	LOG(path);
	
	String content = LoadFile(path);
	edit.SetData(content);
}


Timer::Timer() {
	CtrlLayout(*this);
	hours.SetData(0);
	mins.SetData(1);
	secs.SetData(0);
	start <<= THISBACK(ToggleStart);
	hours.SetFocus();
}

void Timer::ToggleStart() {
	if (running) {
		start.SetLabel(t_("Start"));
		running = false;
		tc.Kill();
	}
	else {
		int h = hours.GetData();
		int m = mins.GetData();
		int s = secs.GetData();
		h = min(100, max(0, h));
		m = min(60, max(0, m));
		s = min(60, max(0, s));
		int total = s + 60 * (m + 60 * h);
		Time now = GetSysTime();
		ready = now + total;
		start.SetLabel(t_("Stop"));
		running = true;
		tc.Set(-1000, THISBACK(Check));
		Check();
	}
}

void Timer::Check() {
	if (!running) return;
	Time now = GetSysTime();
	int64 remaining = max((int64)0, ready.Get() - now.Get());
	int64 remaining_s = remaining % 60;
	remaining /= 60;
	int64 remaining_m = remaining % 60;
	remaining /= 60;
	int64 remaining_h = remaining;
	String s;
	s << "Remaining: ";
	if (remaining_h)
		s << remaining_h << " hours";
	if (remaining_h || remaining_m)
		s << " " << remaining_m << " minutes";
	if (remaining_h || remaining_m || remaining_s)
		s << " " << remaining_s << " seconds";
	this->remaining.SetLabel(s);
	if (now >= ready) {
		tc.Kill();
		running = false;
		start.SetLabel(t_("Start"));
		this->remaining.SetLabel(t_("Timer is not running"));
		OnReady();
	}
}

void Timer::OnReady() {
	PromptOK("[h(255.42.0)/@(255.42.0)$(28.212.255) $$1,1#E0842503E169BCB89B760F238004336D:Ring][*_@(141.42.0) $$0,0#00000000000000000000000000000000:Default][{_} [s1;= `*ring`* `*ring`*&][s0;= Timer is ready!&][s1;= `*ring`*]]");
}

void Timer::Data() {
	
}
void Timer::ToolMenu(Bar& bar) {
	bar.Add(running ? t_("Stop") : t_("Start"), THISBACK(ToggleStart));
}

}

END_UPP_NAMESPACE
