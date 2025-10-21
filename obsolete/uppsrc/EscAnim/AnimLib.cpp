#include "EscAnim.h"

NAMESPACE_UPP

#define ContextAnimProxy(x) \
	void EscAnimContext::x(EscEscape& e) \
	{ \
		EscAnimProgram* prog = FindProgram(e); \
		ASSERT(prog); \
		if (!prog) Panic("script called from different context"); \
		prog->x(e); \
	}

ContextAnimProxy(ESC_DrawText)


void EscAnimProgram::ESC_DrawText(EscEscape& e) {
	int x = e[0].GetInt();
	int y = e[1].GetInt();
	String str = e[2];
	int ms = e[3].GetInt();
	
	//TODO
	LOG(x << "," << y << ": " << str);
	
	Animation& a = ctx->a;
	AnimPlayer& p = ctx->p;
	
	AnimScene& s = a.GetActiveScene();
	AnimObject& parent = s.GetRoot();
	AnimObject& o = parent.Add();
	o.SetPosition(Point(x,y));
	o.SetText(str, 20, Color(47, 98, 158));
	p.Recompile(parent);
	
	p.AddTimedRemoveObject(1000, o, THISBACK(Continue));
	
	//e.esc.hi.SleepReleasing(ms);
	vm->SleepInfiniteReleasing();
}



END_UPP_NAMESPACE
