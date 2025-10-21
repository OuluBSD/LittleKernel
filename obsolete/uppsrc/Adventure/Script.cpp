#include "Adventure.h"

namespace Adventure {
	
	


/*

void Script::Clear() {
	Stop();
	fn.Clear();
	a0 = EscValue();
	a1 = EscValue();
	flags = 0;
	paused_cam_following = 0;
	type = SCENE_NULL;
	is_esc = false;
	
}

Script& Script::Set(Gate0 cb, EscValue a0, EscValue a1) {
	Clear();
	this->a0 = a0;
	this->a1 = a1;
	is_esc = false;
	fn = cb;
	global = 0;
	running = true;
	return *this;
}

Script& Script::Set(EscGlobal& g, EscValue *self, EscValue fn, EscValue a0, EscValue a1) {
	Clear();
	is_esc = false;
	
	// Find & check lambda before setting fields
	EscValue lambda;
	String fn_name;
	if (fn.IsLambda()) {
		lambda = fn;
		fn_name = "<lambda>";
	}
	else {
		fn_name = fn.ToString();
		fn_name.Replace("\"", "");
		if (self && self->IsMap())
			lambda = self->MapGet(fn_name);
		if (!lambda.IsLambda()) {
			lambda = g.Get(fn_name, EscValue());
			if (!lambda.IsLambda()) {
				LOG("Key '" << fn_name << "' is not lambda");
				return *this;
			}
		}
	}
	
	Vector<EscValue> arg;
	if (!a0.IsVoid()) arg << a0;
	if (!a0.IsVoid() && !a1.IsVoid()) arg << a1;
	
	const HiLambda& l = lambda.GetLambda();
	if (arg.GetCount() != l.arg.GetCount()) {
		String argnames;
		for(int i = 0; i < l.arg.GetCount(); i++)
			argnames << (i ? ", " : "") << l.arg[i];
		LOG(Format("invalid number of arguments (%d passed, expected: %s)", arg.GetCount(), argnames));
		return *this;
	}
	
	// Set fields
	this->a0 = a0;
	this->a1 = a1;
	is_esc = true;
	global = &g;
	this->fn_name = fn_name;
	
	// Initialize esc runner
	op_limit = 1000000;
	esc = new Esc(g, l.code, op_limit, l.filename, l.line);
	auto& e = *esc;
	if (self)
		e.Self() = *self;
	for(int i = 0; i < l.arg.GetCount(); i++)
		e.Var().GetPut(l.arg[i]) = arg[i];
	
	//e.no_return = e.no_break = e.no_continue = true;
	//e.loop = 0;
	//e.skipexp = 0;
	
	running = true;
	LOG("Script::Set: started " << fn_name);
	
	return *this;
}

Script& Script::Start() {
	tc.KillSet(10, THISBACK(Execute));
	return *this;
}

Script& Script::Stop() {
	tc.Kill();
	running = false;
	return *this;
}

void Script::Execute() {
	if (!is_esc) {
		if (!fn || !fn()) {
			tc.Kill();
			running = false;
			LOG("Script::Set: stopped " << fn_name);
			WhenStop(this);
		}
	}
	else {
		tc.Kill();
		ASSERT_(0, "Do not execute esc outside main thread");
	}
}

bool Script::ProcessEsc() {
	LOG("Script::ProcessEsc");
	if (!esc || !RunEscSteps()) {
		tc.Kill();
		running = false;
		LOG("Script::ProcessEsc: stopped " << fn_name);
		WhenStop(this);
	}
	return running;
}

bool Script::RunEscSteps() {
	LOG("Script::RunEscSteps");
	auto& e = *esc;
	int op = 0;
//	try {
//		while(!e.IsEof() && e.no_return && e.no_break && e.no_continue && op < op_limit_at_once) {
//			e.DoStatement();
//			op++;
//		}
//	}
//	catch (CParser::Error e) {
//		LOG("Script::RunEscSteps: error: " << e);
//		return false;
//	}
//	
//	return !e.IsEof();

	
	e.Run();
	
	return false;
}
*/



void Program::Cutscene(SceneType type, EscValue* self, EscValue func_cutscene, EscValue func_override) {
	
	/*cut = {
		flags = type,
		thrd = cocreate(func_cutscene),
		override_ = func_override,
		paused_cam_following = cam_following_actor
	};*/
	
	if (cutscene_override.IsVoid()) {
		cutscene_override = func_override;
		
		Script& cut = AddCutscene("cutscene0");
		cut.user_type = type;
		cut.WhenStop = THISBACK(ClearCutsceneOverride);
		cut.Set(0, func_cutscene, room_curr);
		
		// set as active cutscene
		cutscene_curr = &cut;
	}
	else {
		StartScriptEsc(self, cutscene_override, 0);
	}
}

void Program::ClearCutsceneOverride(Script& s) {
	cutscene_override = EscValue();
	
	if (&s == cutscene_curr)
		cutscene_curr = 0;
}


EscAnimProgram& Program::AddScript(String name, int group) {
	return ctx.CreateProgramT<Script>(name, group);
}

EscAnimProgram& Program::AddLocal(String name) {
	return AddScript(name, SCRIPT_LOCAL);
}

EscAnimProgram& Program::AddGlobal(String name) {
	return AddScript(name, SCRIPT_GLOBAL);
}

EscAnimProgram& Program::AddCutscene(String name) {
	return AddScript(name, SCRIPT_CUTSCENE);
}

EscAnimProgram& Program::StartScript(Gate0 func, bool bg, EscValue noun1, EscValue noun2) {
	RemoveStoppedScripts();
	
	// background || local?
	if (bg)
		return AddGlobal("script").Set(func, noun1, noun2);
	
	else
		return AddLocal("script").Set(func, noun1, noun2);
}

EscAnimProgram& Program::StartScriptEsc(EscValue* self, EscValue script_name, bool bg, EscValue noun1, EscValue noun2) {
	
	//LOG("Program::StartScriptEsc: " << script_name);
	RemoveStoppedScripts();
	
	// background || local?
	if (bg)
		return AddGlobal("hi-script").Set(self, script_name, noun1, noun2);
	
	else
		return AddLocal("hi-script").Set(self, script_name, noun1, noun2);
}

bool Program::ScriptRunning(Script& func)  {
	// loop through both sets of scripts...
	ASSERT(ctx.HasProgram(func));
	bool b = func.IsRunning();
	return b;
}

void Program::StopScript(Script& func) {
	ctx.StopProgram(func);
}

void Program::RemoveStoppedScripts() {
	ctx.RemoveStopped();
}

bool Program::AddEscFunctions() {
	auto& global = ctx.global;
	
	//Escape(global, "Print(x)", SIC_Print);
	//Escape(global, "Input()", SIC_Input);
	//Escape(global, "InputNumber()", SIC_InputNumber);
	Escape(global, "print_line(txt, x, y, col, align, use_caps, duration, big_font)", THISBACK(EscPrintLine));
	Escape(global, "break_time(t)", THISBACK(EscBreakTime));
	Escape(global, "put_at(obj, x, y, room)", THISBACK(EscPutAt));
	Escape(global, "camera_follow(actor)", THISBACK(EscCameraFollow));
	Escape(global, "change_room(new_room, fade)", THISBACK(EscChangeRoom));
	//Escape(global, "set_global_game(game)", THISBACK(EscSetGlobalGame));
	Escape(global, "cutscene(type, func_cutscene, func_override)", THISBACK(EscCutscene));
	Escape(global, "sub(type, func_cutscene, func_override)", THISBACK(EscCutscene));
	Escape(global, "select_actor(name)", THISBACK(EscSelectActor));
	Escape(global, "pickup_obj(me)", THISBACK(EscPickupObject));
	Escape(global, "set_trans_col(clr, boolean)", THISBACK(EscSetTransparencyColor));
	Escape(global, "fades(a, b)", THISBACK(EscFades));
	Escape(global, "map(celx, cely, sx, sy, celw, celh)", THISBACK(EscMap));
	Escape(global, "say_line(str)", THISBACK(EscSayLine));
	Escape(global, "say_line_actor(actor, str, caps, duration)", THISBACK(EscSayLineActor));
	Escape(global, "camera_at(a)", THISBACK(EscCameraAt));
	Escape(global, "camera_pan_to(a)", THISBACK(EscCameraPanTo));
	Escape(global, "camera_pan_to_coord(x,y)", THISBACK(EscCameraPanToCoord));
	Escape(global, "wait_for_camera()", THISBACK(EscWaitForCamera));
	Escape(global, "rectfill(a, b, c, d, e)", THISBACK(EscDrawRectFill));
	Escape(global, "line(a, b, c, d, e)", THISBACK(EscDrawLine));
	Escape(global, "circfill(a, b, c)", THISBACK(EscDrawCircleFill));
	Escape(global, "come_out_door(a, b)", THISBACK(EscComeOutDoor));
	Escape(global, "start_script(a, b)", THISBACK(EscStartScript));
	Escape(global, "stop_script(a, b)", THISBACK(EscStopScript));
	Escape(global, "sfx0()", THISBACK(EscSoundFx0));
	Escape(global, "sfx1()", THISBACK(EscSoundFx1));
	Escape(global, "do_anim(a, b, c)", THISBACK(EscDoAnimation));
	Escape(global, "shake(a)", THISBACK(EscShake));
	Escape(global, "script_running(a)", THISBACK(EscScriptRunning));
	Escape(global, "walk_to(a, b, c)", THISBACK(EscWalkTo));
	Escape(global, "open_door(a, b)", THISBACK(EscOpenDoor));
	Escape(global, "close_door(a)", THISBACK(EscCloseDoor));
	Escape(global, "dialog_set(a)", THISBACK(EscDialogSet));
	Escape(global, "dialog_start(a, b)", THISBACK(EscDialogStart));
	Escape(global, "dialog_hide()", THISBACK(EscDialogHide));
	Escape(global, "dialog_clear()", THISBACK(EscDialogClear));
	
	return true;
}

void Program::EscCameraFollow(EscEscape& e) {
	CameraFollow(e[0]);
}

void Program::EscChangeRoom(EscEscape& e) {
	ChangeRoom(e[0], e[1]);
}

/*void Program::EscSetGlobalGame(EscEscape& e) {
	game = e[0];
}*/

void Program::EscCutscene(EscEscape& e) {
	Cutscene((SceneType)e[0].GetInt(), &e.self, e[1], e[2]);
}

void Program::EscPutAt(EscEscape& e) {
	if (e.arg.GetCount() == 4) {
		PutAt(
			e[0],
			e[1].GetInt(),
			e[2].GetInt(),
			e[3]);
	}
	else {
		ASSERT_(0, "invalid put_at args");
	}
}

void Program::EscPrintLine(EscEscape& e) {
	String txt = e[0].ToString();
	int x = e[1].GetInt();
	int y = e[2].GetInt();
	int col = e[3].GetInt();
	int align = e[4].GetInt();
	bool use_caps = e[5].GetInt();
	float duration = e[6].GetNumber();
	bool big_font = e[7].GetInt();
	
	if (txt.GetCount() >= 2 && txt[0] == '"')
		txt = txt.Mid(1, txt.GetCount()-2);
	
	AddTextObject(e, txt, x, y, col, align, use_caps, duration, big_font);
}

void Program::AddTextObject(EscEscape& e, String txt, int x, int y, int col, int align, bool use_caps, float duration, bool big_font) {
	LOG("Program::AddTextObject: " << x << "," << y << ": " << txt);
	
	if (use_caps)
		txt = ToUpper(txt);
	
	int fnt_h = big_font ? 16 : 8;
	
	Animation& a = ctx.a;
	AnimPlayer& p = ctx.p;
	EscAnimProgram* prog = ctx.FindProgram(e);
	ASSERT(prog);
	
	AnimScene& s = a.GetActiveScene();
	AnimObject& parent = s.GetRoot();
	AnimObject& o = parent.Add();
	o.SetPosition(Point(x,y));
	o.SetText(txt, fnt_h, Color(47, 98, 158));
	p.Recompile(parent);
	
	int ms = duration / 32.0 * 1000;
	
	ms *= dbg_sleep_multiplier;
	
	p.AddTimedRemoveObject(ms, o, prog->ContinueCallback());
	
	//e.esc.hi.SleepReleasing(ms);
	e.esc.esc.SleepInfiniteReleasing();
}

void Program::EscBreakTime(EscEscape& e) {
	int t = e[0].GetInt();
	
	LOG("Program::EscPrintLine: " << t);
	
	TODO
}

void Program::EscSelectActor(EscEscape& e) {
	
	TODO
	
}

void Program::EscPickupObject(EscEscape& e) {
	TODO
}

void Program::EscSetTransparencyColor(EscEscape& e) {
	int col = e[0];
	int is_transparent = e[1];
	
	if (draw) draw->SetTransCol(col, is_transparent);
}

void Program::EscFades(EscEscape& e) {
	TODO
}

void Program::EscMap(EscEscape& e) {
	int celx = e[0];
	int cely = e[1];
	int sx = e[2];
	int sy = e[3];
	int celw = e[4];
	int celh = e[5];
	
	if (draw) draw->PaintMap(*draw->img_draw, celx, cely, sx, sy, celw, celh);
}

void Program::EscSayLine(EscEscape& e) {
	String str = e[0];
	SayLine(str);
}

void Program::EscSayLineActor(EscEscape& e) {
	EscValue actor = e[0];
	String str = e[1];
	int use_caps = e[2];
	double duration = e[3];
	SayLineActor(actor, str, use_caps, duration);
}

void Program::EscCameraAt(EscEscape& e) {
	Point pt;
	pt.x = e[0];
	pt.y = 0;
	CameraAt(pt);
}

void Program::EscCameraPanTo(EscEscape& e) {
	EscValue o = e[0];
	CameraPanTo(o);
}

void Program::EscCameraPanToCoord(EscEscape& e) {
	int x = e[0];
	int y = e[1];
	EscValue o;
	o.SetEmptyMap();
	o.MapSet("x", x);
	o.MapSet("y", y);
	CameraPanTo(o);
}

void Program::EscWaitForCamera(EscEscape& e) {
	if (cam_script && ScriptRunning(*cam_script))
		e.esc.Yield();
}

void Program::EscDrawRectFill(EscEscape& e) {
	TODO
}

void Program::EscDrawLine(EscEscape& e) {
	TODO
}

void Program::EscDrawCircleFill(EscEscape& e) {
	TODO
}

void Program::EscComeOutDoor(EscEscape& e) {
	TODO
}

void Program::EscStartScript(EscEscape& e) {
	TODO
}

void Program::EscStopScript(EscEscape& e) {
	TODO
}

void Program::EscSoundFx0(EscEscape& e) {
	TODO
}

void Program::EscSoundFx1(EscEscape& e) {
	TODO
}

void Program::EscDoAnimation(EscEscape& e) {
	TODO
}

void Program::EscShake(EscEscape& e) {
	TODO
}

void Program::EscScriptRunning(EscEscape& e) {
	TODO
}

void Program::EscWalkTo(EscEscape& e) {
	TODO
}

void Program::EscOpenDoor(EscEscape& e) {
	TODO
}

void Program::EscCloseDoor(EscEscape& e) {
	TODO
}

void Program::EscDialogSet(EscEscape& e) {
	TODO
}

void Program::EscDialogStart(EscEscape& e) {
	TODO
}

void Program::EscDialogHide(EscEscape& e) {
	TODO
}

void Program::EscDialogClear(EscEscape& e) {
	TODO
}

void Program::EscTodo(EscEscape& e) {
	TODO
}


	
}
