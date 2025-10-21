#include <AI/Ctrl/Ctrl.h>
	 
NAMESPACE_UPP

void BiographyPlatformCtrl::Platforms::Messaging::Ctor() {
	this->p.tabs.Add(vsplit.SizePos(), "Messaging");
	
	CtrlLayout(entry);
	
	vsplit.Vert() << threadsplit << entry;
	vsplit.SetPos(3333);
	
	threadsplit.Horz() << entries << threads << comments;
	threadsplit.SetPos(2000, 0);
	threadsplit.SetPos(2500, 1);
	
	entries.AddColumn(t_("Sub-Forum"));
	entries.AddColumn(t_("Entry Title"));
	entries.AddIndex("IDX");
	entries.ColumnWidths("1 3");
	entries.WhenBar << THISBACK(EntryListMenu);
	entries.WhenCursor << THISBACK(DataEntry);
	
	threads.AddColumn(t_("Thread Title"));
	threads.AddIndex("IDX");
	threads.WhenBar << THISBACK(ThreadListMenu);
	threads.WhenCursor << THISBACK(DataThread);
	
	comments.AddColumn(t_("#"));
	comments.AddColumn(t_("Published"));
	comments.AddColumn(t_("User"));
	comments.AddColumn(t_("Message"));
	comments.AddColumn(t_("Keywords"));
	comments.AddColumn(t_("Comments"));
	comments.AddColumn(t_("Score"));
	comments.AddIndex("IDX");
	comments.ColumnWidths("1 3 2 10 4 1 1");
	comments.WhenBar << THISBACK(CommentListMenu);
	comments.WhenCursor << THISBACK(DataComment);
	
	entry.entry_subforum.WhenAction << THISBACK(OnValueChange);
	entry.entry_title.WhenAction << THISBACK(OnValueChange);
	entry.thread_title.WhenAction << THISBACK(OnValueChange);
	entry.user.WhenAction << THISBACK(OnValueChange);
	entry.generate.WhenAction << THISBACK(OnValueChange);
	entry.message.WhenAction << THISBACK(OnValueChange);
	entry.orig_message.WhenAction << THISBACK(OnValueChange);
	entry.keywords.WhenAction << THISBACK(OnValueChange);
	entry.location.WhenAction << THISBACK(OnValueChange);
	entry.date.WhenAction << THISBACK(OnValueChange);
	entry.clock.WhenAction << THISBACK(OnValueChange);
	entry.clock.WhenDeactivate << THISBACK(OnValueChange);
	entry.clock.WhenPopDown << THISBACK(OnValueChange);
}

void BiographyPlatformCtrl::Platforms::Messaging::DataPlatform() {
	DatasetPtrs p; o.GetDataset(p);
	if (!o.p.platforms.IsCursor()) {
		entries.Clear();
		threads.Clear();
		comments.Clear();
		ClearEntry();
		return;
	}
	
	Profile& prof = *p.profile;
	ProfileData& pd = ProfileData::Get(prof);
	int plat_i = o.p.platforms.Get("IDX");
	PlatformData& pld = pd.platforms[plat_i];
	
	int row = 0;
	for (const PlatformEntry& e : pld.entries) {
		int score = 0;
		entries.Set(row, "IDX", row);
		entries.Set(row, 0, e.subforum);
		entries.Set(row, 1, e.title);
		row++;
	}
	INHIBIT_CURSOR(entries);
	entries.SetCount(pld.entries.GetCount());
	entries.SetSortColumn(0, true);
	if (entries.GetCount() && !entries.IsCursor())
		entries.SetCursor(0);
	
	DataEntry();
}

void BiographyPlatformCtrl::Platforms::Messaging::DataEntry() {
	DatasetPtrs p; o.GetDataset(p);
	if (!o.p.platforms.IsCursor() || !entries.IsCursor()) {
		threads.Clear();
		comments.Clear();
		ClearEntry();
		return;
	}
	
	Profile& prof = *p.profile;
	ProfileData& pd = ProfileData::Get(prof);
	int plat_i = o.p.platforms.Get("IDX");
	int entry_i = entries.Get("IDX");
	PlatformData& pld = pd.platforms[plat_i];
	PlatformEntry& e = pld.entries[entry_i];
	
	entry.entry_subforum.SetData(e.subforum);
	entry.entry_title.SetData(e.title);
	
	int row = 0;
	for (const PlatformThread& t : e.threads) {
		int score = 0;
		threads.Set(row, "IDX", row);
		threads.Set(row, 0, t.title);
		row++;
	}
	INHIBIT_CURSOR(threads);
	threads.SetCount(e.threads.GetCount());
	threads.SetSortColumn(0, true);
	if (threads.GetCount() && !threads.IsCursor())
		threads.SetCursor(0);
	
	DataThread();
}

void BiographyPlatformCtrl::Platforms::Messaging::DataThread() {
	DatasetPtrs p; o.GetDataset(p);
	if (!o.p.platforms.IsCursor() || !entries.IsCursor() || !threads.IsCursor()) {
		comments.Clear();
		ClearEntry();
		return;
	}
	
	Profile& prof = *p.profile;
	ProfileData& pd = ProfileData::Get(prof);
	int plat_i = o.p.platforms.Get("IDX");
	int entry_i = entries.Get("IDX");
	int thrd_i = threads.Get("IDX");
	PlatformData& pld = pd.platforms[plat_i];
	PlatformEntry& e = pld.entries[entry_i];
	PlatformThread& thrd = e.threads[thrd_i];
	
	entry.thread_title.SetData(thrd.title);
	
	int row = 0;
	for (const PlatformComment& c : thrd.comments) {
		int score = 0;
		comments.Set(row, "IDX", row);
		comments.Set(row, 0, 1+row);
		comments.Set(row, 1, c.published);
		comments.Set(row, 2, c.user);
		comments.Set(row, 3, c.message);
		comments.Set(row, 4, c.keywords);
		comments.Set(row, 5, c.responses.GetCount());
		comments.Set(row, 6, score);
		row++;
	}
	INHIBIT_CURSOR(comments);
	comments.SetCount(thrd.comments.GetCount());
	comments.SetSortColumn(0, true);
	if (comments.GetCount() && !comments.IsCursor())
		comments.SetCursor(0);
	
	DataComment();
}

void BiographyPlatformCtrl::Platforms::Messaging::DataComment() {
	DatasetPtrs p; o.GetDataset(p);
	if (!o.p.platforms.IsCursor() || !threads.IsCursor() || !entries.IsCursor() || !comments.IsCursor()) {
		ClearEntry();
		return;
	}
	Profile& prof = *p.profile;
	ProfileData& pd = ProfileData::Get(prof);
	int plat_i = o.p.platforms.Get("IDX");
	int entry_i = entries.Get("IDX");
	int thrd_i = threads.Get("IDX");
	int comm_i = comments.Get("IDX");
	PlatformData& pld = pd.platforms[plat_i];
	PlatformEntry& e = pld.entries[entry_i];
	PlatformThread& thrd = e.threads[thrd_i];
	PlatformComment& c = thrd.comments[comm_i];
	
	entry.user = c.user;
	entry.generate = c.generate;
	entry.message.SetData(c.message);
	entry.orig_message.SetData(c.orig_message);
	entry.keywords.SetData(c.keywords);
	entry.location.SetData(c.location);
	entry.date.SetData(c.published);
	entry.clock.SetData(c.published);
	entry.merged.SetData(c.text_merged_status);
}

void BiographyPlatformCtrl::Platforms::Messaging::ClearEntry() {
	entry.message.SetData("");
	entry.orig_message.SetData("");
	entry.keywords.Clear();
	entry.location.Clear();
	entry.generate.Set(0);
	entry.user.Clear();
	entry.date.SetData(GetSysTime());
	entry.clock.SetData(GetSysTime());
}

void BiographyPlatformCtrl::Platforms::Messaging::Clear() {
	threads.Clear();
	entries.Clear();
	comments.Clear();
	ClearEntry();
}

void BiographyPlatformCtrl::Platforms::Messaging::OnValueChange() {
	DatasetPtrs p; o.GetDataset(p);
	if (!o.p.platforms.IsCursor() || !entries.IsCursor())
		return;
	Profile& prof = *p.profile;
	ProfileData& pd = ProfileData::Get(prof);
	int plat_i = o.p.platforms.Get("IDX");
	int entry_i = entries.Get("IDX");
	PlatformData& pld = pd.platforms[plat_i];
	PlatformEntry& e = pld.entries[entry_i];
	
	e.title = entry.entry_title.GetData();
	e.subforum = entry.entry_subforum.GetData();
	entries.Set(0, e.subforum);
	entries.Set(1, e.title);
	
	if (!threads.IsCursor())
		return;
	
	int thrd_i = threads.Get("IDX");
	if (thrd_i < 0 || thrd_i >= e.threads.GetCount()) return;
	PlatformThread& thrd = e.threads[thrd_i];
	thrd.title = entry.thread_title.GetData();
	threads.Set(0, thrd.title);
	
	if (!comments.IsCursor())
		return;
	
	int comm_i = comments.Get("IDX");
	int comm_cur = comments.GetCursor();
	PlatformComment& c = thrd.comments[comm_i];
	c.message = entry.message.GetData();
	c.orig_message = entry.orig_message.GetData();
	c.keywords = entry.keywords.GetData();
	c.location = entry.location.GetData();
	c.user = entry.user.GetData();
	c.generate = entry.generate.Get();
	
	Date date = entry.date.GetDate();
	Time time = entry.clock.GetTime();
	time.year = date.year;
	time.month = date.month;
	time.day = date.day;
	c.published = time;
	
	comments.Set(2, c.user);
	comments.Set(3, c.message);
	comments.Set(4, c.keywords);
}

void BiographyPlatformCtrl::Platforms::Messaging::AddEntry() {
	DatasetPtrs p; o.GetDataset(p);
	if (!o.p.platforms.IsCursor())
		return;
	Profile& prof = *p.profile;
	ProfileData& pd = ProfileData::Get(prof);
	int plat_i = o.p.platforms.Get("IDX");
	PlatformData& pld = pd.platforms[plat_i];
	PlatformEntry& e = pld.entries.Add();
	e.threads.Add();
	
	DataPlatform();
}

void BiographyPlatformCtrl::Platforms::Messaging::RemoveEntry() {
	DatasetPtrs p; o.GetDataset(p);
	if (!o.p.platforms.IsCursor() || !entries.IsCursor())
		return;
	Profile& prof = *p.profile;
	ProfileData& pd = ProfileData::Get(prof);
	int plat_i = o.p.platforms.Get("IDX");
	PlatformData& pld = pd.platforms[plat_i];
	int entry_i = entries.Get("IDX");
	if (entry_i >= 0 && entry_i < pld.entries.GetCount())
		pld.entries.Remove(entry_i);
	
	DataPlatform();
}

void BiographyPlatformCtrl::Platforms::Messaging::AddThread() {
	DatasetPtrs p; o.GetDataset(p);
	if (!o.p.platforms.IsCursor())
		return;
	Profile& prof = *p.profile;
	ProfileData& pd = ProfileData::Get(prof);
	int plat_i = o.p.platforms.Get("IDX");
	PlatformData& pld = pd.platforms[plat_i];
	int entry_i = entries.Get("IDX");
	PlatformEntry& e = pld.entries[entry_i];
	PlatformThread& t = e.threads.Add();
	
	DataEntry();
}

void BiographyPlatformCtrl::Platforms::Messaging::RemoveThread() {
	DatasetPtrs p; o.GetDataset(p);
	if (!o.p.platforms.IsCursor() || !entries.IsCursor())
		return;
	Profile& prof = *p.profile;
	ProfileData& pd = ProfileData::Get(prof);
	int plat_i = o.p.platforms.Get("IDX");
	PlatformData& pld = pd.platforms[plat_i];
	int entry_i = entries.Get("IDX");
	PlatformEntry& e = pld.entries[entry_i];
	int thrd_i = threads.Get("IDX");
	if (thrd_i >= 0 && thrd_i < e.threads.GetCount())
		e.threads.Remove(thrd_i);
	
	DataEntry();
}

void BiographyPlatformCtrl::Platforms::Messaging::AddComment() {
	DatasetPtrs p; o.GetDataset(p);
	if (!o.p.platforms.IsCursor() || !threads.IsCursor())
		return;
	Profile& prof = *p.profile;
	ProfileData& pd = ProfileData::Get(prof);
	int plat_i = o.p.platforms.Get("IDX");
	PlatformData& pld = pd.platforms[plat_i];
	int entry_i = entries.Get("IDX");
	PlatformEntry& e = pld.entries[entry_i];
	int thrd_i = threads.Get("IDX");
	PlatformThread& t = e.threads[thrd_i];
	PlatformComment& c = t.comments.Add();
	c.published = GetSysTime();
	
	DataThread();
}

void BiographyPlatformCtrl::Platforms::Messaging::RemoveComment() {
	DatasetPtrs p; o.GetDataset(p);
	if (!o.p.platforms.IsCursor() || !threads.IsCursor() || !entries.IsCursor())
		return;
	Profile& prof = *p.profile;
	ProfileData& pd = ProfileData::Get(prof);
	int plat_i = o.p.platforms.Get("IDX");
	PlatformData& pld = pd.platforms[plat_i];
	int entry_i = entries.Get("IDX");
	PlatformEntry& e = pld.entries[entry_i];
	int thrd_i = threads.Get("IDX");
	PlatformThread& t = e.threads[thrd_i];
	int comm_i = comments.Get("IDX");
	if (comm_i >= 0 && comm_i < t.comments.GetCount())
		t.comments.Remove(comm_i);
	
	DataThread();
}

void BiographyPlatformCtrl::Platforms::Messaging::EntryListMenu(Bar& bar) {
	bar.Add(t_("Add Entry"), MetaImgs::BlueRing(), THISBACK(AddEntry)).Key(K_CTRL_W);
	if (entries.IsCursor())
		bar.Add(t_("Remove Entry"), MetaImgs::BlueRing(), THISBACK(RemoveEntry)).Key(K_CTRL_D);
}

void BiographyPlatformCtrl::Platforms::Messaging::ThreadListMenu(Bar& bar) {
	bar.Add(t_("Add Thread"), MetaImgs::BlueRing(), THISBACK(AddThread)).Key(K_CTRL_E);
	if (entries.IsCursor())
		bar.Add(t_("Remove Thread"), MetaImgs::BlueRing(), THISBACK(RemoveThread)).Key(K_CTRL_F);
}

void BiographyPlatformCtrl::Platforms::Messaging::CommentListMenu(Bar& bar) {
	bar.Add(t_("Add Comment"), MetaImgs::BlueRing(), THISBACK(AddComment)).Key(K_CTRL_T);
	if (entries.IsCursor())
		bar.Add(t_("Remove Comment"), MetaImgs::BlueRing(), THISBACK(RemoveComment)).Key(K_CTRL_H);
}

void BiographyPlatformCtrl::Platforms::Messaging::ToolMenu(Bar& bar) {
	bar.Add(t_("Start"), MetaImgs::RedRing(), THISBACK1(Do, 0)).Key(K_F5);
	bar.Add(t_("Stop"), MetaImgs::RedRing(), THISBACK1(Do, 1)).Key(K_F6);
	bar.Separator();
	bar.Add(t_("Clear thread's merged text"), MetaImgs::BlueRing(), THISBACK1(Do, 2)).Key(K_F7);
	bar.Separator();
	bar.Add(t_("Add their response from clipboard"), MetaImgs::BlueRing(), THISBACK1(PasteResponse, 0)).Key(K_CTRL_Q);
	bar.Add(t_("Add own response from clipboard"), MetaImgs::BlueRing(), THISBACK1(PasteResponse, 1)).Key(K_CTRL_W);
	bar.Add(t_("Generate response"), MetaImgs::RedRing(), THISBACK1(Do, 3)).Key(K_F8);
	bar.Add(t_("Create keywords"), MetaImgs::RedRing(), THISBACK1(Do, 4)).Key(K_F9);
}

void BiographyPlatformCtrl::Platforms::Messaging::PasteResponse(int fn) {
	DatasetPtrs mp; o.GetDataset(mp);
	if (!mp.profile) return;
	if (!o.p.platforms.IsCursor() || !threads.IsCursor()) return;
	
	Profile& prof = *mp.profile;
	ProfileData& pd = ProfileData::Get(prof);
	int plat_i = o.p.platforms.Get("IDX");
	PlatformData& pld = pd.platforms[plat_i];
	int entry_i = entries.Get("IDX");
	PlatformEntry& e = pld.entries[entry_i];
	int thrd_i = threads.Get("IDX");
	PlatformThread& t = e.threads[thrd_i];
	
	String message = ReadClipboardText();
	if (message.IsEmpty()) return;
	
	if (fn == 0) {
		String user;
		bool generate_response = false;
		for (int i = t.comments.GetCount()-1; i >= 0; i--) {
			PlatformComment& pc = t.comments[i];
			if (!pc.user.IsEmpty()) {
				user = pc.user;
				generate_response = pc.generate;
				break;
			}
		}
		
		PlatformComment& pc = t.comments.Add();
		pc.user = user;
		pc.message = message;
		pc.generate = generate_response;
		pc.published = GetSysTime();
	}
	else if (fn == 1) {
		PlatformComment& pc = t.comments.Add();
		pc.message = message;
		pc.generate = true;
		pc.published = GetSysTime();
	}
	o.PostCallback(THISBACK(DataThread));
}

void BiographyPlatformCtrl::Platforms::Messaging::Do(int fn) {
	DatasetPtrs mp; o.GetDataset(mp);
	if (!mp.profile || !mp.release)
		return;
	SocialContentProcess& ss = SocialContentProcess::Get(mp);
	if (fn == 0) {
		ss.Start();
	}
	else if (fn == 1) {
		ss.Stop();
	}
	else if (fn == 2) {
		if (!o.p.platforms.IsCursor() || !threads.IsCursor())
			return;
		Profile& prof = *mp.profile;
		ProfileData& pd = ProfileData::Get(prof);
		int plat_i = o.p.platforms.Get("IDX");
		PlatformData& pld = pd.platforms[plat_i];
		int entry_i = entries.Get("IDX");
		PlatformEntry& e = pld.entries[entry_i];
		int thrd_i = threads.Get("IDX");
		PlatformThread& t = e.threads[thrd_i];
		for(int i = 0; i < t.comments.GetCount(); i++)
			t.comments[i].ClearMerged();
	}
	else if (fn == 3) {
		if (!o.p.platforms.IsCursor() || !threads.IsCursor())
			return;
		Profile& prof = *mp.profile;
		ProfileData& pd = ProfileData::Get(prof);
		int plat_i = o.p.platforms.Get("IDX");
		PlatformData& pld = pd.platforms[plat_i];
		int entry_i = entries.Get("IDX");
		PlatformEntry& e = pld.entries[entry_i];
		int thrd_i = threads.Get("IDX");
		PlatformThread& t = e.threads[thrd_i];
		int c = t.comments.GetCount();
		if (c < 2) return;
		PlatformComment& pc0 = t.comments[c-1];
		PlatformComment& pc1 = t.comments[c-2];
		if (pc1.text_merged_status.IsEmpty()) {
			PromptOK("The merged text is needed first. Run social data update (F5)");
			return;
		}
		TaskMgr& m = AiTaskManager();
		SocialArgs args;
		args.fn = 18;
		args.text = pc1.text_merged_status;
		args.profile = pc0.user.GetCount() ? pc0.user : "me";
		
		m.GetSocial(args, [this,&pc0,&pc1](String res) {
			RemoveEmptyLines2(res);
			Vector<String> l = Split(res, "\n");
			String suggs;
			for (String& s : l) {
				if (!suggs.IsEmpty()) suggs << "\n\n";
				RemoveQuotes(s);
				suggs << "- " << s;
			}
			pc0.message = suggs;
			o.PostCallback(THISBACK(DataThread));
		});
	}
	else if (fn == 4) {
		if (!o.p.platforms.IsCursor() || !comments.IsCursor())
			return;
		Profile& prof = *mp.profile;
		ProfileData& pd = ProfileData::Get(prof);
		int plat_i = o.p.platforms.Get("IDX");
		PlatformData& pld = pd.platforms[plat_i];
		int entry_i = entries.Get("IDX");
		PlatformEntry& e = pld.entries[entry_i];
		int thrd_i = threads.Get("IDX");
		PlatformThread& t = e.threads[thrd_i];
		int comment_i = comments.Get("IDX");
		PlatformComment& pc = t.comments[comment_i];
		
		TaskMgr& m = AiTaskManager();
		SocialArgs args;
		args.fn = 19;
		args.text = pc.message;
		
		m.GetSocial(args, [this,&pc](String res) {
			res = TrimBoth(res);
			RemoveQuotes(res);
			pc.keywords = res;
			o.PostCallback(THISBACK(DataComment));
		});
	}
}



END_UPP_NAMESPACE
