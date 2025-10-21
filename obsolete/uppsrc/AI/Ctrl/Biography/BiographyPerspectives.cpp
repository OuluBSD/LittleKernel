#include "Biography.h"

NAMESPACE_UPP


BiographyPerspectiveCtrl::BiographyPerspectiveCtrl() {
	MainLayout();
}


ConceptualFrameworkNavigator::ConceptualFrameworkNavigator() {
	CtrlLayout(cf);
	CtrlLayout(story);
	
	
	cfs.AddColumn(t_("Framework"));
	cfs.AddColumn(t_("Snap. rev."));
	cfs.AddColumn(t_("Belief"));
	cfs.ColumnWidths("4 1 2");
	cfs.AddIndex("IDX");
	cfs <<= THISBACK(DataFramework);
	cfs.WhenBar << [this](Bar& bar) {
		bar.Add("Add item", THISBACK1(Do, 2));
		bar.Add(cfs.IsCursor(), "Remove item", THISBACK1(Do, 3));
	};
	cfs.WhenCursor << THISBACK(DataFramework);
	
	cf.name.WhenAction << THISBACK(OnValueChange);
	cf.revision.WhenAction << THISBACK(OnValueChange);
	cf.belief.WhenAction << THISBACK(OnValueChange);
	
	stories.AddColumn(t_("Typeclass"));
	stories.AddColumn(t_("Content"));
	stories.AddColumn(t_("Description"));
	for(int i = 0; i < SCORE_COUNT; i++)
		stories.AddColumn("S" + IntStr(i));
	stories.AddColumn("Total score");
	stories.ColumnWidths("2 2 12 1 1 1 1 1 1 1 1 1 1 1");
	stories.AddIndex("IDX");
	stories.WhenCursor << THISBACK(DataStory);
	stories.WhenBar << [this](Bar& bar) {
		bar.Add("Add item", THISBACK1(Do, 2));
		bar.Add(cfs.IsCursor(), "Remove item", THISBACK1(Do, 3));
	};
	story_sort_column = 3+SCORE_COUNT;
	
	cf.belief.WhenAction << THISBACK(OnValueChange);
	
	story.colors.AddColumn("Color");
	
}

void ConceptualFrameworkNavigator::MainLayout() {
	Add(vsplit.VSizePos(0,20).HSizePos());
	
	vsplit.Vert() << tsplit << bsplit;
	tsplit.Horz() << cfsplit << stories;
	
	cfsplit.Vert() << cfs << cf;
	cfsplit.SetPos(7500);
	
	tsplit.SetPos(2500);
	
	bsplit.Horz() << story << story_struct;
	#if USE_IMPROVED_ELEMENTS
	bsplit << story_improved;
	#endif
}

void ConceptualFrameworkNavigator::SideLayout() {
	Add(vsplit.SizePos());
	vsplit.Vert() << cfsplit << stories << bsplit;
	vsplit.SetPos(1500,0);
	vsplit.SetPos(6666,1);
	
	cfsplit.Horz() << cfs << cf;
	
	bsplit.Horz() << story << story_struct;
	#if USE_IMPROVED_ELEMENTS
	bsplit << story_improved;
	#endif
}

void ConceptualFrameworkNavigator::LockForm() {
	cf.name.SetEditable(false);
	cf.revision.SetEditable(false);
	cf.belief.SetEditable(false);
}

void ConceptualFrameworkNavigator::Data() {
	DataAll(false);
}

void ConceptualFrameworkNavigator::DataAll(bool forced) {
	DatasetPtrs mp; GetDataset(mp);
	
	if (!mp.release)
		return;
	
	TODO
	#if 0
	if (!forced && cfs.GetCount() == mp.release->concepts.GetCount())
		return;
	
	for(int i = 0; i < mp.release->concepts.GetCount(); i++) {
		Concept& c = mp.release->concepts[i];
		cfs.Set(i, 0, c.name);
		cfs.Set(i, 1, c.snap_rev);
		int j = mdb.FindBelief(c.belief_uniq);
		cfs.Set(i, 2, j >= 0 ? mdb.beliefs[j].name : String());
		cfs.Set(i, "IDX", i);
	}
	INHIBIT_CURSOR(cfs);
	cfs.SetCount(mp.release->concepts.GetCount());
	if (!cfs.IsCursor() && cfs.GetCount())
		cfs.SetCursor(0);
	
	
	int appmode = GetAppMode();
	
	story.typeclass.Clear();
	story.typeclass.Add("");
	for (String tc : GetTypeclasses())
		story.typeclass.Add(tc);
	
	story.content.Clear();
	story.content.Add("");
	for (const auto& it : GetContents())
		story.content.Add(it.key);
	
	
	DataFramework();
	#endif
}

void ConceptualFrameworkNavigator::DataFramework() {
	DatasetPtrs mp; GetDataset(mp);
	
	TODO
	#if 0
	if (!cfs.IsCursor() || !mp.release) {
		stories.Clear();
		return;
	}
	int cf_i = cfs.Get("IDX");
	Concept& con = mp.release->concepts[cf_i];
	
	// Header tabs for belief
	int j = mdb.FindBelief(con.belief_uniq);
	if (j >= 0) {
		auto& b = mdb.beliefs[j];
		for(int i = 0; i < SCORE_COUNT && i < b.attrs.GetCount(); i++) {
			Belief::Attr& a = b.attrs[i];
			stories.ColumnAt(3+i).HeaderTab()
				.SetText(a.positive);
		}
	}
	else {
		for(int i = 0; i < SCORE_COUNT; i++) {
			stories.ColumnAt(3+i).HeaderTab()
				.SetText(GetScoreKey(i));
		}
	}
	
	// Form values
	cf.name.SetData(con.name);
	
	
	// Belief list
	if (cf.belief.GetCount() != mdb.beliefs.GetCount()+1) {
		cf.belief.Clear();
		cf.belief.Add("Default");
		for(int i = 0; i < mdb.beliefs.GetCount(); i++) {
			const Belief& b = mdb.beliefs[i];
			cf.belief.Add(b.name);
		}
	}
	int belief_i = mdb.FindBelief(con.belief_uniq) + 1;
	if (belief_i < cf.belief.GetCount())
		cf.belief.SetIndex(belief_i);
	
	
	// Revision list
	cf.revision.Clear();
	int rev_cur = 0;
	for(int i = 0; i < mp.profile->snapshots.GetCount(); i++) {
		auto& snap = mp.profile->snapshots[i];
		cf.revision.Add(snap.revision);
		if (snap.revision == con.snap_rev)
			rev_cur = i;
	}
	if (rev_cur < cf.revision.GetCount())
		cf.revision.SetIndex(rev_cur);
	
	
	// Stories list
	Color high = LtGreen();
	Color low = LtRed();
	int row = 0;
	const auto& tcs = GetTypeclasses();
	const auto& cons = GetContents();
	for(int i = 0; i < con.stories.GetCount(); i++) {
		const ConceptStory& st = con.stories[i];
		stories.Set(row, 0, st.typeclass >= 0 && st.typeclass < tcs.GetCount() ? tcs[st.typeclass] : String());
		stories.Set(row, 1, st.content >= 0 && st.content < cons.GetCount() ? cons[st.content].key : String());
		//stories.Set(row, 2, st.desc);
		SetColoredListValue(stories, row, 2, st.desc, st.GetAverageColor(), true);
		double sum = 0;
		for(int j = 0; j < SCORE_COUNT; j++) {
			double sc = st.AvSingleScore(j);
			if (0) {
				stories.Set(row, 3+j, sc);
			}
			else {
				Color clr = Blend(low, high, sc / 10.0 * 256);
				SetColoredListValue(stories, row, 3+j, DblStr(sc), clr, true);
			}
			sum += sc;
		}
		double av = sum / (double)SCORE_COUNT;
		if (av > 10.0) av = 0;
		if (0) {
			stories.Set(row, 3+SCORE_COUNT, av);
		}
		else {
			Color clr = Blend(low, high, av / 10.0 * 256);
			SetColoredListValue(stories, row, 3+SCORE_COUNT, DblStr(av), clr, true);
		}
		stories.Set(row, "IDX", i);
		row++;
	}
	SetCountWithDefaultCursor(stories, row, story_sort_column, true);
	
	DataStory();
	#endif
}

void ConceptualFrameworkNavigator::DataStory() {
	DatasetPtrs mp; GetDataset(mp);
	
	if (!mp.release || !cfs.IsCursor() || !stories.IsCursor()) {
		story.colors.SetCount(0);
		return;
	}
	
	TODO
	#if 0
	int cf_i = cfs.Get("IDX");
	int story_i = stories.Get("IDX");
	if (cf_i >= mp.release->concepts.GetCount()) return;
	Concept& con = mp.release->concepts[cf_i];
	if (story_i >= con.stories.GetCount()) return;
	ConceptStory& st = con.stories[story_i];
	
	story.desc.SetData(st.desc);
	if (st.typeclass >= -1 && st.typeclass < story.typeclass.GetCount())
		story.typeclass.SetIndex(st.typeclass+1);
	if (st.content >= -1 && st.content < story.content.GetCount())
		story.content.SetIndex(st.content+1);
	
	//
	String full;
	for(int i = 0; i < st.elements.GetCount(); i++) {
		const auto& el = st.elements[i];
		full << "[" << el.key << "]\n";
		full << el.value << "\n\n";
	}
	story_struct.SetData(full);
	
	//
	String improved;
	for(int i = 0; i < st.ELEMENTS_VAR.GetCount(); i++) {
		const auto& el = st.ELEMENTS_VAR[i];
		improved << "[" << el.key << "]\n";
		improved << el.value << "\n\n";
		
		story.colors.Set(i, 0, AttrText(el.key).NormalPaper(el.clr).Paper(el.clr));
	}
	story_improved.SetData(improved);
	story.colors.SetCount(st.ELEMENTS_VAR.GetCount());
	
	//
	#endif
}

void ConceptualFrameworkNavigator::GetElements(ConceptualFrameworkArgs& args) {
	DatasetPtrs mp; GetDataset(mp);
	
	if (!mp.release || !cfs.IsCursor() || !stories.IsCursor())
		return;
	int cf_i = cfs.Get("IDX");
	int story_i = stories.Get("IDX");
	TODO
	#if 0
	if (cf_i >= mp.release->concepts.GetCount()) return;
	Concept& con = mp.release->concepts[cf_i];
	if (story_i >= con.stories.GetCount()) return;
	ConceptStory& st = con.stories[story_i];
	args.elements.Clear();
	for(int i = 0; i < st.ELEMENTS_VAR.GetCount(); i++) {
		const auto& el = st.ELEMENTS_VAR[i];
		args.elements.Add(el.key, el.value);
	}
	#endif
}

int64 ConceptualFrameworkNavigator::GetBeliefUniq() const {
	DatasetPtrs mp;
	const_cast<ConceptualFrameworkNavigator&>(*this).GetDataset(mp);
	
	if (!mp.release || !cfs.IsCursor() || !stories.IsCursor())
		return 0;
	int cf_i = cfs.Get("IDX");
	int story_i = stories.Get("IDX");
	TODO
	#if 0
	if (cf_i >= mp.release->concepts.GetCount()) return 0;
	Concept& con = mp.release->concepts[cf_i];
	return con.belief_uniq;
	#endif
	return 0;
}

void ConceptualFrameworkNavigator::OnValueChange() {
	DatasetPtrs mp; GetDataset(mp);
	
	if (!cfs.IsCursor())
		return;
	int cf_i = cfs.Get("IDX");
	TODO
	#if 0
	Concept& con = mp.release->concepts[cf_i];
	
	con.name = cf.name.GetData();
	
	if (cf.revision.GetCount())
		con.snap_rev = cf.revision.GetValue();
	
	cfs.Set(0, con.name);
	cfs.Set(1, con.snap_rev);
	if (mdb.beliefs.GetCount() && cf.belief.GetCount()) {
		int idx = cf.belief.GetIndex() - 1;
		if (idx >= 0) {
			auto& b = mdb.beliefs[idx];
			cfs.Set(2, b.name);
			con.belief_uniq = b.uniq;
		}
		else {
			cfs.Set(2, Value());
			con.belief_uniq = 0;
		}
	}
	#endif
}

void ConceptualFrameworkNavigator::ToolMenu(Bar& bar) {
	bar.Add(t_("Update"), MetaImgs::BlueRing(), THISBACK1(DataAll, true)).Key(K_CTRL_Q);
	bar.Separator();
	bar.Add(t_("Previous sort column"), MetaImgs::BlueRing(), THISBACK1(MoveSortColumn, -1)).Key(K_F1);
	bar.Add(t_("Next sort column"), MetaImgs::BlueRing(), THISBACK1(MoveSortColumn, +1)).Key(K_F2);
}

void BiographyPerspectiveCtrl::ToolMenu(Bar& bar) {
	ConceptualFrameworkNavigator::ToolMenu(bar);
	bar.Separator();
	bar.Add(t_("Start"), MetaImgs::RedRing(), THISBACK1(Do, 0)).Key(K_F5);
	bar.Add(t_("Stop"), MetaImgs::RedRing(), THISBACK1(Do, 1)).Key(K_F6);
}

void ConceptualFrameworkNavigator::MoveSortColumn(int i) {
	int base = story_sort_column - 3;
	base += i;
	while (base < 0) base += SCORE_COUNT+1;
	base = base % (SCORE_COUNT+1);
	story_sort_column = 3 + base;
	PostCallback(THISBACK(DataFramework));
}

void ConceptualFrameworkNavigator::Do(int fn) {
	DatasetPtrs mp; GetDataset(mp);
	
	TODO
	#if 0
	int appmode = GetAppMode();
	if (!mp.release)
		return;
	if (fn == 0 || fn == 1) {
		if (!cfs.IsCursor())
			return;
		int cf_i = cfs.Get("IDX");
		Concept& c = mp.release->concepts[cf_i];
		if (c.snap_rev < 0) {PromptOK("No snapshot revision set"); return;}
		BiographyPerspectives* snap = mp.profile->FindSnapshotRevision(c.snap_rev);
		if (!snap) {PromptOK("No snapshot revision found"); return;}
		
		ConceptualFrameworkProcess& sdi = ConceptualFrameworkProcess::Get(*mp.profile, c, *snap);
		prog.Attach(sdi);
		sdi.WhenRemaining << [this](String s) {PostCallback([this,s](){remaining.SetLabel(s);});};
		if (fn == 0)
			sdi.Start();
		else
			sdi.Stop();
	}
	else if (fn == 2) {
		Concept& c = mp.release->concepts.Add();
		c.created = GetSysTime();
		c.name = "Unnamed #" + IntStr(mp.release->concepts.GetCount());
		c.snap_rev = mp.profile->snapshots.Top().revision;
		//c.snap_rev = mp.release->profile->snapshots.GetCount()-2; // latest can't be used
		PostCallback(THISBACK(Data));
	}
	else if (fn == 3) {
		if (!cfs.IsCursor())
			return;
		int cf_i = cfs.Get("IDX");
		mp.release->concepts.Remove(cf_i);
	}
	#endif
}



INITIALIZER_COMPONENT_CTRL(BiographyPerspectives, BiographyPerspectiveCtrl)


END_UPP_NAMESPACE
