#include <AI/Ctrl/Ctrl.h>

NAMESPACE_UPP


PerspectiveCtrl::PerspectiveCtrl() {
	Add(hsplit.VSizePos(0,20).HSizePos());
	
	hsplit.Add(info);
	hsplit.Add(user);
	hsplit.Add(attrs);
	hsplit.SetPos(8500/3,0);
	hsplit.SetPos(8500*2/3,1);
	
	CtrlLayout(info);
	info.description.WhenAction << THISBACK(OnValueChange);
	info.reference.WhenAction << THISBACK(OnValueChange);
	
	user.AddColumn("User input");
	
	attrs.AddColumn("Positive");
	attrs.AddColumn("Negative");
	
	
}

void PerspectiveCtrl::Data() {
	PerspectiveComponent& b = GetExt<PerspectiveComponent>();
	
	info.description.SetData(b.description);
	info.reference.SetData(b.reference);
	
	for(int i = 0; i < b.user.GetCount(); i++) {
		user.Set(i, 0, b.user[i]);
	}
	user.SetCount(b.user.GetCount());
	
	for(int i = 0; i < b.attrs.GetCount(); i++) {
		PerspectiveComponent::Attr& a = b.attrs[i];
		attrs.Set(i, 0, a.positive);
		attrs.Set(i, 1, a.negative);
	}
	attrs.SetCount(b.attrs.GetCount());
	
}

void PerspectiveCtrl::ToolMenu(Bar& bar) {
	bar.Add(t_("Start"), MetaImgs::RedRing(), THISBACK1(Do, 0)).Key(K_F5);
	bar.Add(t_("Stop"), MetaImgs::RedRing(), THISBACK1(Do, 1)).Key(K_F6);
	bar.Separator();
	bar.Add(t_("Paste user data"), MetaImgs::BlueRing(), THISBACK1(Do, 2)).Key(K_F5);
}

void PerspectiveCtrl::Do(int fn) {
	DatasetPtrs mp; GetDataset(mp);
	if (!mp.profile || !mp.release)
		return;
	PerspectiveProcess& ss = PerspectiveProcess::Get(mp);
	if (fn == 0) {
		ss.Start();
	}
	else if (fn == 1) {
		ss.Stop();
	}
	else if (fn == 2) {
		PerspectiveComponent& b = GetExt<PerspectiveComponent>();
		String res = ReadClipboardText();
		RemoveEmptyLines3(res);
		b.user <<= Split(res, "\n");
		Data();
	}
}

void PerspectiveCtrl::OnValueChange() {
	PerspectiveComponent& b = GetExt<PerspectiveComponent>();
	
	b.description = info.description.GetData();
	b.reference = info.reference.GetData();
}













PerspectiveProcess::PerspectiveProcess() {
	
}

int PerspectiveProcess::GetPhaseCount() const {
	return PHASE_COUNT;
}

void PerspectiveProcess::DoPhase() {
	ASSERT(p.perspective);
	
	if (phase == PHASE_GET_POSITIVE_ATTRS) {
		PerspectiveComponent& b = *p.perspective;
		if (b.attrs.GetCount()) {
			NextPhase();
			return;
		}
		
		BeliefArgs args;
		args.fn = 0;
		args.user <<= b.user;
		
		SetWaiting(1);
		TaskMgr& m = AiTaskManager();
		m.GetPerspectiveProcess(args, [this,&b](String res) {
			res = TrimBoth(res);
			if (res.Left(2) != "1.")
				res = "1." + res;
			
			RemoveEmptyLines2(res);
			Vector<String> lines = Split(res, "\n");
			
			b.attrs.Clear();
			for(int i = 0; i < lines.GetCount(); i++) {
				b.attrs.Add().positive = lines[i];
			}
			SetWaiting(0);
			NextPhase();
			
			WhenReady();
		});
	}
	else if (phase == PHASE_GET_NEGATIVE_ATTRS) {
		PerspectiveComponent& b = *p.perspective;
		if (!b.attrs.GetCount() || b.attrs[0].negative.GetCount()) {
			NextPhase();
			return;
		}
		
		BeliefArgs args;
		args.fn = 1;
		for(int i = 0; i < b.attrs.GetCount(); i++)
			args.pos << b.attrs[i].positive;
		
		SetWaiting(1);
		TaskMgr& m = AiTaskManager();
		m.GetPerspectiveProcess(args, [this,&b](String res) {
			res = TrimBoth(res);
			if (res.Left(2) != "1.")
				res = "1." + res;
			
			RemoveEmptyLines2(res);
			Vector<String> lines = Split(res, "\n");
			
			for(int i = 0; i < lines.GetCount(); i++) {
				if (i >= b.attrs.GetCount()) break;
				b.attrs[i].negative = lines[i];
			}
			
			SetWaiting(0);
			NextPhase();
			
			WhenReady();
		});
	}
}

PerspectiveProcess& PerspectiveProcess::Get(DatasetPtrs p) {
	String t = (String)p.perspective->val.GetPath() + ";" + (String)p.snap->val.GetPath();
	hash_t h = t.GetHashValue();
	static ArrayMap<hash_t, PerspectiveProcess> map;
	int i = map.Find(h);
	if (i >= 0)
		return map[i];
	
	PerspectiveProcess& ls = map.Add(h);
	ls.p = p;
	return ls;
}







INITIALIZER_COMPONENT_CTRL(PerspectiveComponent, PerspectiveCtrl)

END_UPP_NAMESPACE
