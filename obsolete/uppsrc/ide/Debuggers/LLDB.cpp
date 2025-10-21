#include "Debuggers.h"

#define METHOD_NAME UPP_METHOD_NAME("LLDB")

void LLDB::WatchMenu(Bar& bar)
{
	bool b = !IdeIsDebugLock();
	bar.Add(b, PdbKeys::AK_CLEARWATCHES, THISBACK(ClearWatches));
	bar.Add(b, PdbKeys::AK_ADDWATCH, THISBACK(QuickWatch));
}

void LLDB::DebugBar(Bar& bar)
{
	using namespace PdbKeys;

	bar.Add("Stop debugging", DbgImg::StopDebug(), THISBACK(Stop))
	   .Key(K_SHIFT_F5);
	bar.Separator();
	bool b = !IdeIsDebugLock();
	bar.Add(b, AK_STEPINTO, DbgImg::StepInto(), THISBACK1(Step, disas.HasFocus() ? "stepi"
	                                                                             : "step"));
	bar.Add(b, AK_STEPOVER, DbgImg::StepOver(), THISBACK1(Step, disas.HasFocus() ? "nexti"
	                                                                             : "next"));
	bar.Add(b, AK_STEPOUT, DbgImg::StepOut(), THISBACK1(Step, "finish"));
	bar.Add(b, AK_RUNTO, DbgImg::RunTo(), THISBACK(DoRunTo));
	bar.Add(b, AK_RUN, DbgImg::Run(), THISBACK(Run));
	bar.Add(!b && pid, AK_BREAK, DbgImg::Stop(), THISBACK(BreakRunning));
	bar.MenuSeparator();
	bar.Add(b, AK_AUTOS, THISBACK1(SetTab, 0));
	bar.Add(b, AK_LOCALS, THISBACK1(SetTab, 1));
	bar.Add(b, AK_THISS, THISBACK1(SetTab, 2));
	bar.Add(b, AK_WATCHES, THISBACK1(SetTab, 3));
	WatchMenu(bar);
	bar.Add(b, AK_CPU, THISBACK1(SetTab, 4));
	bar.MenuSeparator();
	bar.Add(b, "Copy backtrace", THISBACK(CopyStack));
	bar.Add(b, "Copy backtrace of all threads", THISBACK(CopyStackAll));
	bar.Add(b, "Copy dissassembly", THISBACK(CopyDisas));
}

String FirstLine(const String& s);

String FormatFrameLLDB(const char *s)
{
	for(int i = 0; i < 6; i++)
		if(*s++ == 0)
			return Null;
	if(*s++ != '#')
		return Null;
	while(IsDigit(*s))
		s++;
	if(*s++ != ':')
		return Null;
	while(*s == ' ')
		s++;
	if(s[0] == '0' && ToUpper(s[1]) == 'X') {
		s += 2;
		while(IsXDigit(*s))
			s++;
		while(*s == ' ')
			s++;
		if(s[0] == 'i' && s[1] == 'n')
			s += 2;
		while(*s == ' ')
			s++;
	}
	String result;
	const char *w = strchr(s, '\r');
	if(!w)
		w = strchr(s, '\n');
	if(w)
		result = String(s, w);
	else
		result = s;
	if(!IsAlpha(*s)) {
		int q = result.ReverseFind(' ');
		if(q >= 0)
			result = result.Mid(q + 1);
	}
	return result.GetCount() > 2 ? result : Null;
}


void LLDB::CopyStack()
{
	if(IdeIsDebugLock())
		return;
	DropFrames();
	String s;
	for(int i = 0; i < frame.GetCount(); i++)
		s << frame.GetValue(i) << "\n";
	WriteClipboardText(s);
}

void LLDB::CopyStackAll()
{
	String s = FastCmd("thread list");
	StringStream ss(s);
	String r;
	while(!ss.IsEof()) {
		String s = ss.GetLine();
		CParser p(s);
		try {
			p.Char('*');
			
			if (!p.Id("thread") || !p.Char('#'))
				continue;
			
			if(p.IsNumber()) {
				int id = p.ReadInt();
				r << "----------------------------------\r\n"
				  << "Thread: " << id << "\r\n\r\n";

				FastCmd(Sprintf("thread select %d", id));

				int i = 0;
				for(;;) {
					String s = FormatFrameLLDB(FastCmd("frame select " + AsString(i++)));
					if(IsNull(s)) break;
					r << s << "\r\n";
				}
				r << "\r\n";
			}
		}
		catch(CParser::Error) {}
	}
	FastCmd("thread select " + ~~threads);
	WriteClipboardText(r);
}

void LLDB::CopyDisas()
{
	if(IdeIsDebugLock())
		return;
	disas.WriteClipboard();
}

int CharFilterReSlash(int c);

String Bpoint(const String& file, int line);

bool LLDB::TryBreak(const char *text)
{
	return FindTag(FastCmd(text), "Breakpoint");
}

bool LLDB::SetBreakpoint(const String& filename, int line, const String& bp)
{
	if(IdeIsDebugLock()) {
		BreakRunning();
		bp_filename = filename;
		bp_line = line;
		bp_val = bp;
		return true;
	}

	String bi = Bpoint(filename, line);

	String command;
	if(bp.IsEmpty())
		command = "breakpoint clear -file \"" + filename + "\" --line " + IntStr(line);
	else if(bp[0]==0xe || bp == "1")
		command = "breakpoint set -file \"" + filename + "\" --line " + IntStr(line);
	else
		command = "breakpoint set -file \"" + filename + "\" --line " + IntStr(line) + " --condition \"" + bp + "\"";
	return !FastCmd(command).IsEmpty();
}

void LLDB::SetDisas(const String& text)
{
	disas.Clear();
	StringStream ss(text);
	while(!ss.IsEof()) {
		String ln = ss.GetLine();
		const char *s = ln;
		while(*s && !IsDigit(*s))
			s++;
		adr_t adr = 0;
		String code, args;
		if(s[0] == '0' && ToLower(s[1]) == 'x')
			adr = (adr_t)ScanInt64(s + 2, NULL, 16);
		int q = nodebuginfo ? ln.Find(":\t") : ln.Find(">:");
		if(q >= 0) {
			s = ~ln + q + 2;
			while(IsSpace(*s))
				s++;
			while(*s && !IsSpace(*s))
				code.Cat(*s++);
			while(IsSpace(*s))
				s++;
			args = s;
			q = args.Find("0x");
			if(q >= 0)
				disas.AddT(ScanInt(~args + q + 2, NULL, 16));
			disas.Add(adr, code, args);
		}
	}
}

void LLDB::SyncDisas(bool fr)
{
	if(!disas.IsVisible())
		return;
	if(!disas.InRange(addr))
		SetDisas(FastCmd("disassemble"));
	disas.SetCursor(addr);
	disas.SetIp(addr, fr ? DbgImg::FrameLinePtr() : DbgImg::IpLinePtr());
}

bool ParsePos(const String& s, String& fn, int& line, adr_t & adr);

void LLDB::CheckEnd(const char *s)
{
	if(!dbg.IsRunning()) {
		Stop();
		return;
	}
	if(FindTag(s, "error: Process must be launched.")) {
		Stop();
		return;
	}
	if(FindTag(s, "Program exited normally.")) {
		Stop();
		return;
	}
	const char *q = FindTag(s, "Program exited with code ");
	if(q) {
		PutConsole(q);
		Stop();
		return;
	}
}

String LLDB::Cmdp(const char *cmdline, bool fr, bool setframe)
{
	expression_cache.Clear();
	IdeHidePtr();
	String s = Cmd(cmdline);
	exception.Clear();
	if(!running_interrupted) {
		int q = s.Find("received signal SIG");
		if(q >= 0) {
			int l = s.ReverseFind('\n', q);
			if(l < 0)
				l = 0;
			int r = s.Find('\n', q);
			if(r < 0)
				r = s.GetCount();
			if(l < r)
				exception = s.Mid(l, r - l);
		}
	}
	else {
		running_interrupted = false;
	}
	
	if(ParsePos(s, file, line, addr)) {
		IdeSetDebugPos(file, line - 1, fr ? DbgImg::FrameLinePtr()
		                                  : DbgImg::IpLinePtr(), 0);
		IdeSetDebugPos(file, line - 1,
		               disas.HasFocus() ? fr ? DbgImg::FrameLinePtr() : DbgImg::IpLinePtr()
		                                : Image(), 1);
		SyncDisas(fr);
		autoline.Clear();
		for(int i = -4; i <= 4; i++)
			autoline << ' ' << IdeGetLine(line + i);
	}
	else {
		file = Null;
		try {
			int q = s.ReverseFind("0x");
			if(q >= 0) {
				CParser pa(~s + q + 2);
				addr = (adr_t)pa.ReadNumber64(16);
				SetDisas(FastCmd(String() << "disassemble --start-address 0x" << FormatHex((void *)addr) << " --end-address 0x" << FormatHex((void *)(addr + 1024))));
				disas.SetCursor(addr);
				disas.SetIp(addr, DbgImg::IpLinePtr());
			}
		}
		catch(CParser::Error) {}
	}

	if(setframe) {
		frame.Clear();
		String f = FastCmd("frame info");
		frame.Add(0, nodebuginfo ? FirstLine(f) : FormatFrameLLDB(f));
		frame <<= 0;
		SyncFrameButtons();
	}
	
	if (dbg.IsRunning()) {
		if (IsProcessExitedNormally(s))
			Stop();
		else {
			s = ObtainThreadsInfo();
			
			ObtainData();
		}
	}
	return s;
}

bool LLDB::IsProcessExitedNormally(const String& cmd_output) const
{
	const auto phrase = String().Cat() << "*stopped,reason=\"exited-normally\"";
	return cmd_output.Find(phrase) >= 0;
}

String LLDB::ObtainThreadsInfo()
{
	threads.Clear();
	String output = FastCmd("thread list");
	StringStream ss(output);
	int active_thread = -1;
	bool main = true;
	while(!ss.IsEof()) {
		String s = ss.GetLine();
		CParser p(s);
		try {
			bool is_active = p.Char('*');
			
			if (!p.Id("thread") || !p.Char('#'))
				continue;
			
			if(!p.IsNumber())
				continue;
			
			int id = p.ReadInt();
			
			String name;
			if (p.Char(':')) {
				while (!p.IsEof()) {
					if (p.Id("name") && p.Char('=')) {
						name = p.ReadString('\'');
						break;
					}
						
					p.SkipTerm();
				}
			}
			
			AttrText text(String() << "Thread " << id);
			if (!name.IsEmpty())
				text.Set(text.ToString() << " (" << name << ")");
			if(is_active) {
				active_thread = id;
				text.Bold();
			}
			if(main) {
				text.Underline();
				main = false;
			}
			threads.Add(id, text);
			threads.GoBegin();
		}
		catch(CParser::Error e) {
			Loge() << METHOD_NAME << e;
		}
	}
		
	if(active_thread >= 0)
		threads <<= active_thread;
	
	return output;
}

void LLDB::ShowException()
{
	if(exception.GetCount())
		ErrorOK(exception);
	exception.Clear();
}

String LLDB::DoRun()
{
	if(firstrun) {
		firstrun = false;
		nodebuginfo_bg.Show();
		nodebuginfo = true;
		String t = Cmd("process launch --stop-at-entry");;
		int q = t.ReverseFind("exited with status = 0 ");
		if(t.GetLength() - q < 20) {
			Stop();
			return Null;
		}
		if(!IsFinished()) {
			String s = Cmd("process status");
			int q = s.FindAfter("Process");
			pid = atoi(~s + q);
		}
		IdeSetBar();
	}
	
	String s;
	for(;;) {
		ClearCtrls();
		s = Cmdp("continue");
		if(IsNull(bp_filename))
			break;
		if(!IdeIsDebugLock())
			SetBreakpoint(bp_filename, bp_line, bp_val);
		bp_filename.Clear();
	}
	ShowException();
	return s;
}

bool LLDB::RunTo()
{
	if(IdeIsDebugLock() || nodebuginfo)
		return false;
	String bi;
	bool df = disas.HasFocus();
	if(df) {
		if(!disas.GetCursor())
			return false;
		FastCmd(Sprintf("breakpoint set --address 0x%X", disas.GetCursor()));
	}
	else
		FastCmd("breakpoint set --file \"" + IdeGetFileName() + "\" --line " + IntStr(IdeGetFileLine()));
	String e = DoRun();
	FastCmd("breakpoint delete --file \"" + IdeGetFileName() + "\" --line " + IntStr(IdeGetFileLine()));
	if(df)
		disas.SetFocus();
	CheckEnd(e);
	IdeActivateBottom();
	return true;
}

void LLDB::BreakRunning()
{
	Logd() << METHOD_NAME << "PID: " << pid << "\n";
	
	auto error = lldb_utils->BreakRunning(pid);
	if(!error.IsEmpty()) {
		Loge() << METHOD_NAME << error;
		ErrorOK(error);
		return;
	}
	
	running_interrupted = true;
}

void LLDB::Run()
{
	if(IdeIsDebugLock())
		return;
	String s = DoRun();
	CheckEnd(s);
	IdeActivateBottom();
}

void LLDB::Step(const char *cmd)
{
	if(IdeIsDebugLock())
		return;
	bool b = disas.HasFocus();
	String s = Cmdp(cmd);
	if(b) disas.SetFocus();
	CheckEnd(s);
	IdeActivateBottom();
	ShowException();
}

void LLDB::DisasCursor()
{
	if(!disas.HasFocus())
		return;
	int line;
	String file;
	adr_t addr;
	if(ParsePos(FastCmd(Sprintf("image lookup -v --address 0x%X", disas.GetCursor())), file, line, addr))
		IdeSetDebugPos(file, line - 1, DbgImg::DisasPtr(), 1);
	disas.SetFocus();
}

void LLDB::DisasFocus()
{
}

void LLDB::DropFrames()
{
	if(frame.GetCount() < 2)
		LoadFrames();
	SyncFrameButtons();
}

void LLDB::LoadFrames()
{
	if(frame.GetCount())
		frame.Trim(frame.GetCount() - 1);
	int i = frame.GetCount();
	int n = 0;
	for(;;) {
		String f = FastCmd(Sprintf("frame select %d", i));
		String s;
		if(nodebuginfo) {
			s = FirstLine(f);
			int q = s.Find("0x");
			if(q < 0)
				break;
			try {
				CParser p(~s + q + 2);
				if(p.ReadNumber64(16) == 0)
					break;
			}
			catch(CParser::Error) {
				break;
			}
		}
		else
			s = nodebuginfo ? FirstLine(f) : FormatFrameLLDB(f);
		if(IsNull(s))
			break;
		if(n++ >= max_stack_trace_size) {
			frame.Add(Null, Sprintf("<load more> (%d loaded)", frame.GetCount()));
			break;
		}
		frame.Add(i++, s);
	}
	SyncFrameButtons();
}

void LLDB::SwitchFrame()
{
	int i = ~frame;
	if(IsNull(i)) {
		i = frame.GetCount() - 1;
		LoadFrames();
		frame <<= i;
	}
	Cmdp("frame info " + AsString(i), i, false);
	SyncFrameButtons();
}

void LLDB::FrameUpDown(int dir)
{
	if(frame.GetCount() < 2)
		LoadFrames();
	int q = frame.GetIndex() + dir;
	if(q >= 0 && q < frame.GetCount()) {
		frame.SetIndex(q);
		SwitchFrame();
	}
}

void LLDB::SwitchThread()
{
	int i = ~threads;
	Cmdp("thread " + AsString(i));
	SyncFrameButtons();
}

void LLDB::ClearCtrls()
{
	threads.Clear();
	disas.Clear();
	
	locals.Clear();
	autos.Clear();
	self.Clear();
	cpu.Clear();
	
	tree.Clear();
}

bool LLDB::Key(dword key, int count)
{
	if(key >= 32 && key < 65535 && tab.Get() == 2) {
		watches.DoInsertAfter();
		Ctrl* f = GetFocusCtrl();
		if(f && watches.HasChildDeep(f))
			f->Key(key, count);
		return true;
	}
	return Ctrl::Key(key, count);
}

bool LLDB::Create(Host& host, const String& exefile, const String& cmdline, bool console)
{
	String lldb_command = LLDBCommand(console) + NormalizeExePath(exefile);

#ifdef PLATFORM_POSIX
#ifndef PLATFORM_MACOS
	IGNORE_RESULT(HostSys("setxkbmap -option grab:break_actions")); // to be able to recover capture in breakpoint
	String xdotool_chk = ConfigFile("xdotool_chk");
	String out;
	if(!FileExists(xdotool_chk) && HostSys("xdotool key XF86Ungrab", out)) {
		Exclamation("[* xdotool] utility is not installed or does not work properly.&"
		            "Debugger will be unable to ungrab debugee's mouse capture - "
		            "mouse might become unusable when debugee stops.");
		SaveFile(xdotool_chk, "1");
	}
#endif
#endif

	if(!host.StartProcess(dbg, lldb_command)) {
		Loge() << METHOD_NAME << "Failed to launch lldb (\"" << lldb_command << "\").";
		
		ErrorOK("Error while invoking lldb! For details check TheIDE logs.");
		return false;
	}

	IdeSetBottom(*this);
	IdeSetRight(disas);

	disas.WhenCursor = THISBACK(DisasCursor);
	disas.WhenFocus = THISBACK(DisasFocus);
	frame.WhenDrop = THISBACK(DropFrames);
	frame <<= THISBACK(SwitchFrame);
	
	threads <<= THISBACK(SwitchThread);

	watches.WhenAcceptEdit = THISBACK(ObtainData);
	tab <<= THISBACK(ObtainData);

	LOG(lldb_command);
    //Cmd("settings set prompt " LLDB_PROMPT);
	Cmd("settings set target.x86-disassembly-flavor intel");
	//Cmd("settings set exec-done-display off");
	//Cmd("settings set annotate 1");
	//Cmd("settings set height 0");
	Cmd("settings set term-width 1024");
	Cmd("settings set auto-confirm true");
	//Cmd("settings set target.symbols.enable-demangling true");
	//Cmd("settings set print static-members off");
	//Cmd("settings set print vtbl off");
	Cmd("settings set target.max-children-count 0");
	//Cmd("settings set print null-stop");

	if(!IsNull(cmdline))
		Cmd("settings set target.run-args " + cmdline);

	firstrun = true;
	running_interrupted = false;

	return true;
}

LLDB::~LLDB()
{
	StringStream ss;
	Store(callback(this, &LLDB::SerializeSession), ss);
	WorkspaceConfigData("lldb-debugger") = ss;

	IdeRemoveBottom(*this);
	IdeRemoveRight(disas);
	KillDebugTTY();
}

void LLDB::Periodic()
{
	if(TTYQuit())
		Stop();
}

void LLDB::SerializeSession(Stream& s)
{
	int version = 0;
	s / version;
	int n = watches.GetCount();
	s / n;
	if(n < 0)
		s.LoadError();
	for(int i = 0; i < n; i++) {
		String w;
		if(s.IsStoring())
			w = watches.Get(i, 0);
		s % w;
		if(s.IsLoading())
			watches.Add(w);
	}
}

LLDB::LLDB()
	: lldb_utils(LLDBUtilsFactory().Create())
{
	auto Mem = [=](Bar& bar, ArrayCtrl& h) {
		String s = h.GetKey();
		if(s.GetCount()) {
			if(!IsAlpha(*s))
				s = '(' + s + ')';
			MemoryMenu(bar, s);
		}
	};
	locals.NoHeader();
	locals.AddColumn("", 1);
	locals.AddColumn("", 6);
	locals.EvenRowColor();
	locals.WhenSel = THISBACK1(SetTree, &locals);
	locals.WhenBar = [=](Bar& bar) { Mem(bar, locals); };
	watches.NoHeader();
	watches.AddColumn("", 1).Edit(watchedit);
	watches.AddColumn("", 6);
	watches.Inserting().Removing();
	watches.EvenRowColor();
	watches.WhenSel = THISBACK1(SetTree, &watches);
	watches.WhenBar = [=](Bar& bar) { Mem(bar, watches); WatchMenu(bar); };
	autos.NoHeader();
	autos.AddColumn("", 1);
	autos.AddColumn("", 6);
	autos.EvenRowColor();
	autos.WhenSel = THISBACK1(SetTree, &autos);
	autos.WhenBar = [=](Bar& bar) { Mem(bar, autos); };
	self.NoHeader();
	self.AddColumn("", 1);
	self.AddColumn("", 6);
	self.EvenRowColor();
	self.WhenSel = THISBACK1(SetTree, &self);
	self.WhenBar = [=](Bar& bar) { Mem(bar, self); };
	cpu.Columns(3);
	cpu.ItemHeight(Courier(Ctrl::HorzLayoutZoom(12)).GetCy());

	pane.Add(tab.SizePos());
	tab.Add(autos.SizePos(), "Autos");
	tab.Add(locals.SizePos(), "Locals");
	tab.Add(watches.SizePos(), "Watches");
	tab.Add(self.SizePos(), "this");
	tab.Add(cpu.SizePos(), "CPU");
	tab.Add(memory.SizePos(), "Memory");
	pane.Add(threads.LeftPosZ(330, 150).TopPos(2));

	memory.WhenGotoDlg = [=] { MemoryGoto(); };

	int bcx = min(EditField::GetStdHeight(), DPI(16));
	pane.Add(frame.HSizePos(Zx(484), 2 * bcx).TopPos(2));
	pane.Add(frame_up.RightPos(bcx, bcx).TopPos(2, EditField::GetStdHeight()));
	frame_up.SetImage(DbgImg::FrameUp());
	frame_up << [=] { FrameUpDown(-1); };
	frame_up.Tip("Previous Frame");
	pane.Add(frame_down.RightPos(0, bcx).TopPos(2, EditField::GetStdHeight()));
	frame_down.SetImage(DbgImg::FrameDown());
	frame_down << [=] { FrameUpDown(1); };
	frame_down.Tip("Next Frame");

	split.Horz(pane, tree.SizePos());
	split.SetPos(8000);
	Add(split);

	tree.WhenOpen = THISBACK(OnTreeExpand);
	tree.WhenBar = THISBACK(OnTreeBar);

	frame.Ctrl::Add(dlock.SizePos());
	dlock = "  Running..";
	dlock.SetFrame(BlackFrame());
	dlock.SetInk(Red);
	dlock.NoTransparent();
	dlock.Hide();

	CtrlLayoutOKCancel(quickwatch, "Watch");
	quickwatch.WhenClose = quickwatch.Breaker(IDCANCEL);
	quickwatch.value.SetReadOnly();
	quickwatch.value.SetFont(CourierZ(11));
	quickwatch.Sizeable().Zoomable();
	quickwatch.SetRect(0, 150, 300, 400);

	Transparent();

	periodic.Set(-50, THISBACK(Periodic));

	StringStream ss(WorkspaceConfigData("lldb-debugger"));
	Load(callback(this, &LLDB::SerializeSession), ss);

	const char *text = "No symbolic information available";
	Size sz = GetTextSize(text, StdFont().Italic().Bold());
	nodebuginfo_bg.Background(Gray())
	              .RightPos(0, sz.cx + DPI(8))
	              .BottomPos(0, sz.cy + DPI(6))
	              .Add(nodebuginfo_text.SizePos());
	nodebuginfo_text = text;
	nodebuginfo_text.AlignCenter().SetInk(Yellow()).SetFont(StdFont().Italic().Bold());
	
	pane.Add(nodebuginfo_bg);
}

One<Debugger> LLDBCreate(Host& host, const String& exefile, const String& cmdline, bool console)
{
	auto dbg = MakeOne<LLDB>();
	if(!dbg->Create(host, exefile, cmdline, console))
		return nullptr;
	return pick(dbg); // CLANG does not like this without pick
}
