#include "Shell.h"

#ifndef flagV1
#include <AI/Core/Core.h>
#endif

NAMESPACE_UPP

ConsoleCtrl::ConsoleCtrl() : cmd(*this) {
	SetView();
	
	AddProgram("blog",	THISBACK(SimpleExt<Widget::BlogPad>));
	AddProgram("help",	THISBACK(SimpleExt<Widget::DraftPad>));
	AddProgram("draft",	THISBACK(SimpleExt<Widget::DraftPad>));
	AddProgram("timer",	THISBACK(SimpleExt<Widget::Timer>));
	
	#if 0
	AddProgram("mkdir",	THISBACK(CreateDirectory));
	AddProgram("rm",	THISBACK(RemoveFile));
	AddProgram("cat",	THISBACK(ShowFile));
	AddProgram("get",	THISBACK(DownloadFile));
	#endif
	
	cwd = GetHomeDirectory();
}

ConsoleCtrl::~ConsoleCtrl() {
	RemoveCtrl(true);
}

ConsoleCtrl* ConsoleCtrl::GetConsole() {
	return this;
}

bool ConsoleCtrl::RealizeFocus() {
	if (active && !active->HasFocusDeep()) {
		active->SetFocus();
		return true;
	}
	return false;
}

void ConsoleCtrl::ClearActive() {
	if (active) {
		if (internal_menubar && (ext || tool))
			RemoveMenuBar();
		RemoveChild(active);
		active = 0;
	}
}

void ConsoleCtrl::Data() {
	if (ext)
		ext->Data();
}

void ConsoleCtrl::SetView() {
	ClearActive();
	
	if (ext)
		active = &*ext;
	else if (tool)
		active = &*tool;
	else
		active = &cmd;
	
	if (active) {
		if (internal_menubar && (ext || tool))
			AddMenuBar();
		Add(active->SizePos());
		PostCallback([=]{
			this->LoadEditPos();
		});
	}
	
	WhenViewChange();
	WhenTitle();
}

void ConsoleCtrl::AddMenuBar() {
	AddFrame(menu);
	menu.Set([this](Bar& b) {
		b.Add(Shell::AK_LEAVE_PROGRAM, THISBACK1(RemoveCtrl, false));
		if (ext)
			ext->ToolMenu(b);
		else if (tool)
			tool->ToolMenu(b);
	});
}

void ConsoleCtrl::RemoveMenuBar() {
	RemoveFrame(menu);
}

void ConsoleCtrl::Menu(Bar& bar) {
	if (ext)  ext->ToolMenu(bar);
	if (tool) tool->ToolMenu(bar);
	
	if (active && active != &cmd) {
		bar.Separator();
		bar.Add(Shell::AK_LEAVE_PROGRAM, THISBACK1(RemoveCtrl, false));
	}
}

void ConsoleCtrl::RemoveCtrl(bool fast_exit) {
	if (ext)  RemoveExt(fast_exit);
	if (tool) RemoveTool(fast_exit);
}

void ConsoleCtrl::RemoveExt(bool fast_exit) {
	if (!ext) return;
	
	if (active == &*ext) {
		SaveEditPos();
		SaveEditPos.Clear();
		LoadEditPos.Clear();
		RemoveChild(&*ext);
		RemoveMenuBar();
		ext.Clear();
		active = 0;
		if (!fast_exit)
			SetView();
	}
	else ext.Clear();
}

void ConsoleCtrl::RemoveTool(bool fast_exit) {
	if (!tool) return;
	
	if (active == &*tool) {
		//SaveEditPos();
		SaveEditPos.Clear();
		LoadEditPos.Clear();
		RemoveChild(&*tool);
		RemoveMenuBar();
		tool.Clear();
		active = 0;
		if (!fast_exit)
			SetView();
	}
	else tool.Clear();
}

String ConsoleCtrl::GetTitle() {
	String s;
	if (ext) {
		s = ext->GetTitle();
		if (s.IsEmpty())
			s = "App";
	}
	else if (tool) {
		s = tool->GetStatusText();
		if (s.IsEmpty())
			s = "App";
	}
	else
		s = "Console";
	return s;
}

END_UPP_NAMESPACE
