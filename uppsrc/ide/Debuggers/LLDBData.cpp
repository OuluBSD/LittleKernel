#include "Debuggers.h"

#define METHOD_NAME UPP_METHOD_NAME("LLDB")

void SkipLLDBInfo(CParser& p)
{
	int level = 1;
	while(level > 0 && !p.IsEof())
		if(p.Char('<'))
			level++;
		else
		if(p.Char('>'))
			level--;
		else
			p.GetChar();
}

String DataClean(CParser& p);
String DataClean(const char *s);

String LLDBData(const String& exp)
{
	int q = exp.Find('{');
	return q < 0 ? exp : TrimBoth(exp.Mid(q));
}

void LLDB::Locals()
{
	VectorMap<String, String> prev = DataMap(locals);
	locals.Clear();
	String s = FastCmd("info locals");
	StringStream ss(s);
	while(!ss.IsEof()) {
		String ln = ss.GetLine();
		const char *s = ln;
		const char *q = strchr(s, '=');
		if(!q) break;
		const char *e = q;
		while(e > s && e[-1] == ' ')
			e--;
		q++;
		while(*q == ' ')
			q++;
		String h = LLDBData(q);
		locals.Add(String(s, e), DataClean(h), h);
	}
	MarkChanged(prev, locals);
}

String LLDB::Print0(const String& exp)
{
	String q = FastCmd("expression " + exp);
	StringStream ss(q);
	String ln = ss.GetLine();
	const char *s = strchr(ln, '=');
	if(s) {
		s++;
		while(*s == ' ')
			s++;
		return LLDBData(s);
	}
	else
		return LLDBData(ln);
}

String LLDB::Print(const String& exp)
{
	int q = expression_cache.Find(exp);
	if(q >= 0)
		return expression_cache[q];
	String r = Print0(exp);
	expression_cache.Add(exp, r);
	return r;
}

void LLDB::Watches()
{
	VectorMap<String, String> prev = DataMap(watches);
	for(int i = 0; i < watches.GetCount(); i++) {
		String h = Print(watches.Get(i, 0));
		watches.Set(i, 1, DataClean(h));
		watches.Set(i, 2, h);
	}
	MarkChanged(prev, watches);
}

void LLDB::TryAuto(Index<String>& tested, const String& exp)
{
	if(tested.Find(exp) < 0 && findarg(exp, "true", "false") < 0) {
		tested.Add(exp);
		String val = Print(exp);
		if(!IsNull(val) && !val.EndsWith(")}") && !val.EndsWith(")>") &&
		   (!IsAlpha(*val) || findarg(val, "true", "false") >= 0))
			autos.Add(exp, DataClean(val), val);
	}
}

void LLDB::Autos()
{
	VectorMap<String, String> prev = DataMap(autos);
	autos.Clear();
	Index<String> tested;
	try {
		CParser p(autoline);
		while(!p.IsEof())
			if(p.IsId()) {
				String exp = p.ReadId();
				TryAuto(tested, exp);
				for(;;) {
					if(p.Char('.') && p.IsId())
						exp << '.';
					else
					if(p.Char2('-', '>') && p.IsId())
						exp << "->";
					else
						break;
					exp << p.ReadId();
					TryAuto(tested, exp);
				}
			}
			else
				p.SkipTerm();
	}
	catch(CParser::Error) {}
	autos.Sort();
	MarkChanged(prev, autos);
}

void LLDB::ReadLLDBValues(CParser& p, VectorMap<String, String>& val)
{
	if(p.Char('{')) {
		while(!p.Char('}') && !p.IsEof()) {
			if(p.IsChar('{')) {
				ReadLLDBValues(p, val);
				continue;
			}
			String id;
			String value;
			const char *b = p.GetPtr();
			while(!p.IsEof())
				if(p.IsChar('=')) {
					id = TrimBoth(String(b, p.GetPtr()));
					p.Char('=');
					break;
				}
				else
					p.SkipTerm();
			b = p.GetPtr();
			int lvl = 0;
			while(!p.IsEof())
				if((p.IsChar(',') || p.IsChar('}')) && lvl == 0) {
					String v = TrimBoth(String(b, p.GetPtr()));
					if(*id == '<') // Base class
						ReadLLDBValues(v, val);
					else
						val.Add(id, v);
					p.Char(',');
					break;
				}
				else
				if(p.Char('{'))
					lvl++;
				else
				if(p.Char('}'))
					lvl--;
				else
					p.SkipTerm();
		}
	}
}

void LLDB::ReadLLDBValues(const String& h, VectorMap<String, String>& val)
{
	try {
		CParser p(h);
		ReadLLDBValues(p, val);
	}
	catch(CParser::Error) {}
}

void LLDB::Self()
{
	VectorMap<String, String> prev = DataMap(locals);
	self.Clear();
	VectorMap<String, String> val;
	ReadLLDBValues(Print("*this"), val);
	for(int i = 0; i < val.GetCount(); i++)
		self.Add(val.GetKey(i), DataClean(val[i]), val[i]);
	MarkChanged(prev, locals);
}

void LLDB::Cpu()
{
#ifdef PLATFORM_WIN32 // mingw dbg is crashing if listing all registers
	String s = FastCmd("info registers rax rbx rcx rdx rsi rdi rbp rsp r8  r9  r10 r11 r12 r13 r14 r15");
#else
	String s = FastCmd("info registers");
#endif
	StringStream ss(s);
	cpu.Clear();
	bool even = false;
	while(!ss.IsEof()) {
		String ln = ss.GetLine();
		cpu.Add(AttrText(ln).SetFont(Courier(Ctrl::HorzLayoutZoom(12)))
		        .Paper(even ? SColorEvenRow() : SColorPaper())
		        .Ink(SColorText));
		even = !even;
	}
}

void LLDB::ObtainData()
{
	switch(tab.Get()) {
	case 0: Autos(); break;
	case 1: Locals(); break;
	case 2: Watches(); break;
	case 3: Self(); break;
	case 4: Cpu(); break;
	case 5: Memory(); break;
	}
}

void LLDB::SetTab(int q)
{
	if(IdeIsDebugLock())
		return;
	tab.Set(q);
	ObtainData();
}

void LLDB::ClearWatches()
{
	if(IdeIsDebugLock())
		return;
	watches.Clear();
	SetTab(2);
}

void LLDB::QuickWatch()
{
	if(IdeIsDebugLock())
		return;
	quickwatch.expression << quickwatch.Breaker(999);
	FastCmd("set print pretty on");
	for(;;) {
		int q = quickwatch.Run();
		if(findarg(q, IDCANCEL, IDOK) >= 0) {
			FastCmd("set print pretty off");
			if(q == IDOK) {
				watches.Add(~quickwatch.expression);
				SetTab(2);
			}
			quickwatch.Close();
			return;
		}
		String s = FastCmd("p " + (String)~quickwatch.expression);
		const char *a = strchr(s, '=');
		if(a) {
			a++;
			while(*a == ' ')
				a++;
			quickwatch.value <<= a;
			quickwatch.expression.AddHistory();
		}
		else
			quickwatch.value <<= s;
	}
}

bool LLDB::Tip(const String& exp, CodeEditor::MouseTip& mt)
{
	if(IsNull(exp))
		return false;
	String val = DataClean(Print(exp));
	if(!IsNull(val) && !val.EndsWith(")}") && !IsAlpha(*val)) {
		if(val.GetCount() > 100) {
			val.Trim(100);
			val << "...";
		}
		mt.display = &StdDisplay();
		mt.value = val;
		mt.sz = mt.display->GetStdSize(String(mt.value) + "X");
		return true;
	}
	return false;
}

void LLDB::SetTree(ArrayCtrl *a)
{
	tree.Clear();
	if(a->IsCursor()) {
		String h = a->Get(2);
		tree.SetRoot(Null, h, String().Cat() << a->Get(0) << " = " << DataClean(h));
		tree.Open(0);
	}
}

Value LLDB::ObtainValueFromTreeCursor(int cursor) const
{
	auto cursor_str = tree.Get(cursor).ToString();
	if (cursor_str.IsEmpty())
		return {};
	
	Value val;
	try {
		CParser parser(cursor_str);
		while (!parser.IsEof()) {
			if (parser.IsString()) {
				val = parser.ReadString();
				break;
			}
			
			parser.SkipTerm();
		}
	} catch(CParser::Error e) {
		Loge() << METHOD_NAME << e;
	}
	
	return val;
}

void LLDB::OnTreeBar(Bar& bar)
{
	auto cursor = tree.GetCursor();
	if(cursor < 0)
		return;
	
	if(tree.GetChildCount(cursor) > 0)
		return;
	
	Value val = ObtainValueFromTreeCursor(cursor);
	
	bar.Add(t_("Copy value to clipboard"), [this, val]() { OnValueCopyToClipboard(val); }).Enable(!val.IsNull());
	
	String var;
	while(cursor >= 0) {
		String s = tree.GetValue(cursor);
		try {
			CParser p(s);
			if(p.IsId())
				var = Merge(".", p.ReadId(), var);
			cursor = tree.GetParent(cursor);
		}
		catch(CParser::Error) {
			break;
		}
	}

	if(var.GetCount())
		MemoryMenu(bar, var);
}

void LLDB::OnValueCopyToClipboard(const Value& val)
{
	if (IsString(val))
		AppendClipboardText(val.ToString());
}

void LLDB::OnTreeExpand(int node)
{
	if(tree.GetChildCount(node))
		return;
	VectorMap<String, String> val;
	ReadLLDBValues(tree.Get(node), val);
	for(int i = 0; i < val.GetCount(); i++)
		tree.Add(node, Null, val[i], val.GetKey(i) + " = " + DataClean(val[i]), *val[i] == '{');
}
