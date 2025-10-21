#include <AI/Ctrl/Ctrl.h>

NAMESPACE_UPP


LeadSourceCtrl::LeadSourceCtrl() {
	Add(hsplit.SizePos());
	
	hsplit.Horz() << vsplit << mainsplit;
	hsplit.SetPos(1500);
	
	vsplit.Vert() << payouts << prices;
	
	mainsplit.Vert() << list << bsplit << bssplit;
	mainsplit.SetPos(4500,0);
	mainsplit.SetPos(9000,1);
	
	bsplit.Horz() << attrs << bools << strings << bvsplit;
	bvsplit.Vert() << list_names << list_values;
	//bvsplit.SetPos(2500);
	bssplit.Horz() << song_typecasts << lyrics_ideas << music_styles;
	
	payouts.AddColumn(t_("Payout"));
	payouts.AddColumn(t_("Count"));
	payouts.AddIndex("IDX");
	payouts.AddIndex("MIN");
	payouts.AddIndex("MAX");
	payouts.ColumnWidths("3 1");
	payouts.WhenCursor << THISBACK(DataPayout);
	
	prices.AddColumn(t_("Submission price"));
	prices.AddColumn(t_("Count"));
	prices.AddIndex("IDX");
	prices.AddIndex("MIN");
	prices.AddIndex("MAX");
	prices.ColumnWidths("3 1");
	prices.WhenCursor << THISBACK(DataPrice);
	
	
	list.AddColumn(t_("Name"));
	list.AddColumn(t_("Price"));
	list.AddColumn(t_("Payout"));
	list.AddColumn(t_("Description"));
	list.AddColumn(t_("Money-score"));
	list.AddColumn(t_("Money-score-rank"));
	list.AddColumn(t_("Opp-score"));
	list.AddColumn(t_("Opp-score-rank"));
	list.AddColumn(t_("Weighted-rank"));
	list.AddColumn(t_("Chance %"));
	list.AddColumn(t_("Av. Payout"));
	list.AddIndex("IDX");
	list.ColumnWidths("4 1 1 10 1 1 1 1 1 1 1");
	list.WhenCursor << THISBACK(DataOpportunity);
	
	attrs.AddColumn(t_("Key"));
	attrs.AddColumn(t_("Value"));
	attrs.ColumnWidths("2 3");
	
	bools.AddColumn(t_("Key"));
	bools.AddColumn(t_("Value"));
	bools.ColumnWidths("5 1");
	
	strings.AddColumn(t_("Key"));
	strings.AddColumn(t_("Value"));
	strings.ColumnWidths("2 3");
	
	list_names.AddColumn(t_("Key"));
	list_names.WhenCursor << THISBACK(DataAnalyzedList);
	list_values.AddColumn(t_("Value"));
	
	song_typecasts.AddColumn(t_("#"));
	song_typecasts.AddColumn(t_("Typecast"));
	song_typecasts.AddColumn(t_("Content"));
	song_typecasts.ColumnWidths("1 3 3");
	song_typecasts.WhenBar << [this](Bar& bar) {
		bar.Add(t_("Copy Typecast"), [this]() {
			if (!this->song_typecasts.IsCursor()) return;
			String txt = this->song_typecasts.Get(1);
			WriteClipboardText(txt);
		});
		bar.Add(t_("Copy Content"), [this]() {
			if (!this->song_typecasts.IsCursor()) return;
			String txt = this->song_typecasts.Get(2);
			WriteClipboardText(txt);
		});
		bar.Add(t_("Copy All"), [this]() {
			if (!this->song_typecasts.IsCursor()) return;
			String txt0 = this->song_typecasts.Get(1);
			String txt1 = this->song_typecasts.Get(2);
			WriteClipboardText(txt0 + "\n" + txt1);
		});
		bar.Separator();
		bar.Add(t_("Copy All Bottom list cursor values"), [this]() {
			if (!this->song_typecasts.IsCursor()) return;
			if (!this->lyrics_ideas.IsCursor()) return;
			if (!this->music_styles.IsCursor()) return;
			String txt0 = this->song_typecasts.Get(1);
			String txt1 = this->song_typecasts.Get(2);
			String txt2 = this->lyrics_ideas.Get(1);
			String txt3 = this->music_styles.Get(1);
			WriteClipboardText(txt0 + "\n" + txt1 + "\n" + txt2 + "\n" + txt3);
		});
	};
	
	lyrics_ideas.AddColumn(t_("#"));
	lyrics_ideas.AddColumn(t_("Lyrics idea"));
	lyrics_ideas.ColumnWidths("1 10");
	lyrics_ideas.WhenBar << [this](Bar& bar) {
		bar.Add(t_("Copy text"), [this]() {
			if (!this->lyrics_ideas.IsCursor()) return;
			String txt = this->lyrics_ideas.Get(1);
			WriteClipboardText(txt);
		});
	};
	
	music_styles.AddColumn(t_("#"));
	music_styles.AddColumn(t_("Music style"));
	music_styles.ColumnWidths("1 10");
	music_styles.WhenBar << [this](Bar& bar) {
		bar.Add(t_("Copy text"), [this]() {
			if (!this->music_styles.IsCursor()) return;
			String txt = this->music_styles.Get(1);
			WriteClipboardText(txt);
		});
	};
}

void LeadSourceCtrl::Data() {
	DatasetPtrs p; GetDataset(p);
	if (!p.lead_data)
		return;
	LeadData& ld = *p.lead_data;
	
	VectorMap<Tuple2<double,double>,int> counts;
	counts.Add(Tuple2<double,double>(0,1), 0);
	counts.Add(Tuple2<double,double>(1,10), 0);
	counts.Add(Tuple2<double,double>(10,100), 0);
	counts.Add(Tuple2<double,double>(100,1000), 0);
	counts.Add(Tuple2<double,double>(10000,10000), 0);
	counts.Add(Tuple2<double,double>(10000,100000000), 0);
	
	for (LeadOpportunity& o : ld.opportunities) {
		int range_i = 0;
		for(int j = 0; j < counts.GetCount(); j++) {
			const auto& range = counts.GetKey(j);
			if (o.min_compensation >= range.a && o.min_compensation < range.b) {
				range_i = j;
				break;
			}
		}
		counts[range_i]++;
	}
	int total = 0;
	
	
	
	payouts.Set(0,0,t_("All"));
	payouts.Set(0,1,total);
	payouts.Set(0,"IDX",-1);
	payouts.Set(0,"MIN",0);
	payouts.Set(0,"MAX",INT_MAX);
	for(int i = 0; i < counts.GetCount(); i++) {
		const auto& range = counts.GetKey(i);
		String str = IntStr((int)range.a) + " - " + IntStr((int)range.b);
		payouts.Set(1+i, 0, str);
		payouts.Set(1+i, 1, counts[i]);
		payouts.Set(1+i, "IDX", i);
		payouts.Set(1+i, "MIN", range.a);
		payouts.Set(1+i, "MAX", range.b);
	}
	
	INHIBIT_CURSOR(payouts);
	payouts.SetCount(1+counts.GetCount());
	//payouts.SetSortColumn(1, true);
	if (!payouts.IsCursor() && payouts.GetCount())
		payouts.SetCursor(0);
	
	DataPayout();
}

void LeadSourceCtrl::DataPayout() {
	DatasetPtrs p; GetDataset(p);
	LeadData& ld = *p.lead_data;
	
	
	if (!payouts.IsCursor())
		return;
	
	int payout_min = payouts.Get("MIN");
	int payout_max = payouts.Get("MAX");
	
	VectorMap<Tuple2<double,double>,int> counts;
	counts.Add(Tuple2<double,double>(0,1), 0);
	counts.Add(Tuple2<double,double>(1,10), 0);
	counts.Add(Tuple2<double,double>(10,100), 0);
	counts.Add(Tuple2<double,double>(100,1000), 0);
	counts.Add(Tuple2<double,double>(10000,10000), 0);
	counts.Add(Tuple2<double,double>(10000,100000000), 0);
	
	
	for (LeadOpportunity& o : ld.opportunities) {
		if (o.min_compensation < payout_min || o.min_compensation >= payout_max)
			continue;
		double price = 0.01 * o.min_entry_price_cents;
		int range_i = 0;
		for(int j = 0; j < counts.GetCount(); j++) {
			const auto& range = counts.GetKey(j);
			if (price >= range.a && price < range.b) {
				range_i = j;
				break;
			}
		}
		counts[range_i]++;
	}
	int total = 0;
	
	
	
	prices.Set(0,0,t_("All"));
	prices.Set(0,1,total);
	prices.Set(0,"IDX",-1);
	prices.Set(0,"MIN",0);
	prices.Set(0,"MAX",INT_MAX);
	for(int i = 0; i < counts.GetCount(); i++) {
		const auto& range = counts.GetKey(i);
		String str = IntStr((int)range.a) + " - " + IntStr((int)range.b);
		prices.Set(1+i, 0, str);
		prices.Set(1+i, 1, counts[i]);
		prices.Set(1+i, "IDX", i);
		prices.Set(1+i, "MIN", range.a);
		prices.Set(1+i, "MAX", range.b);
	}
	
	
	INHIBIT_CURSOR(prices);
	prices.SetCount(1+counts.GetCount());
	//prices.SetSortColumn(1, true);
	if (!prices.IsCursor() && prices.GetCount())
		prices.SetCursor(0);
	
	
	DataPrice();
}

void LeadSourceCtrl::DataPrice() {
	DatasetPtrs p; GetDataset(p);
	LeadData& ld = *p.lead_data;
	
	
	if (!payouts.IsCursor() || !prices.IsCursor())
		return;
	
	int payout_min = payouts.Get("MIN");
	int payout_max = payouts.Get("MAX");
	int price_min = prices.Get("MIN");
	int price_max = prices.Get("MAX");
	
	Time last_seen_limit = GetSysTime() - this->last_seen_limit_mins;
	
	int row = 0;
	int i = -1;
	int last_seen_skipped = 0;
	for (LeadOpportunity& o : ld.opportunities) {
		i++;
		if (have_last_seen_limit && o.last_seen < last_seen_limit) {
			last_seen_skipped++;
			continue;
		}
		if (o.min_compensation < payout_min || o.min_compensation >= payout_max)
			continue;
		double price = 0.01 * o.min_entry_price_cents;
		if (price < price_min || price >= price_max)
			continue;
		
		list.Set(row, 0, o.name);
		list.Set(row, 1, price);
		list.Set(row, 2, o.min_compensation);
		if (o.request_opportunity_description.GetCount()) {
			String s;
			s	<< o.request_opportunity_description << "\n"
				<< o.request_band_description << "\n"
				<< o.request_selection_description;
			list.Set(row, 3, s);
		}
		else {
			list.Set(row, 3, o.request_description);
		}
		list.Set(row, 4, o.money_score);
		list.Set(row, 5, o.money_score_rank);
		list.Set(row, 6, o.opp_score);
		list.Set(row, 7, o.opp_score_rank);
		list.Set(row, 8, o.weighted_rank);
		list.Set(row, 9, o.chance_of_acceptance);
		list.Set(row, 10, o.average_payout_estimation);
		list.Set(row, "IDX", i);
		
		row++;
	}
	double last_seen_skip_perc = last_seen_skipped * 100.0 / ld.opportunities.GetCount();
	
	INHIBIT_CURSOR(list);
	list.SetCount(row);
	list.SetSortColumn(11, true);
	if (!list.IsCursor() && list.GetCount())
		list.SetCursor(0);
	
	DataOpportunity();
}

void LeadSourceCtrl::DataOpportunity() {
	DatasetPtrs p; GetDataset(p);
	LeadData& ld = *p.lead_data;
	
	
	if (!list.IsCursor())
		return;
	
	int idx = list.Get("IDX");
	LeadOpportunity& o = ld.opportunities[idx];
	
	{
		int row = 0;
		for(int i = 0; i < o.GetCount(); i++) {
			const char* key = o.GetKey(i);
			Value val = o[i];
			if (val.IsVoid() || val.IsNull())
				continue;
			attrs.Set(row, 0, key);
			attrs.Set(row, 1, val);
			row++;
		}
		INHIBIT_CURSOR(attrs);
		attrs.SetCount(row);
	}
	
	{
		for(int i = 0; i < o.analyzed_booleans.GetCount(); i++) {
			String key = GetSongListingBooleanKey(i);
			bool b = o.analyzed_booleans[i];
			bools.Set(i, 0, key);
			bools.Set(i, 1, b ? "true" : "false");
		}
		INHIBIT_CURSOR(bools);
		bools.SetCount(o.analyzed_booleans.GetCount());
	}
	
	{
		for(int i = 0; i < o.analyzed_string.GetCount(); i++) {
			String key = GetSongListingStringKey(i);
			const String& s = o.analyzed_string[i];
			strings.Set(i, 0, key);
			strings.Set(i, 1, s);
		}
		INHIBIT_CURSOR(strings);
		strings.SetCount(o.analyzed_string.GetCount());
	}
	
	{
		for(int i = 0; i < o.analyzed_lists.GetCount(); i++) {
			String key = GetSongListingListKey(i);
			list_names.Set(i, 0, key);
		}
		INHIBIT_CURSOR(list_names);
		list_names.SetCount(o.analyzed_lists.GetCount());
		if (list_names.GetCount() && !list_names.IsCursor())
			list_names.SetCursor(0);
		DataAnalyzedList();
	}
	
	{
		const auto& tc_list = UPP::GetTypeclasses(DB_SONG);
		const auto& co_list = UPP::GetContents(DB_SONG);
		int c = min(o.typeclasses.GetCount(), o.contents.GetCount());
		for(int i = 0; i < c; i++) {
			int tc_i = o.typeclasses[i];
			int co_i = o.contents[i];
			String tc = tc_i < tc_list.GetCount() ? tc_list[tc_i] : String();
			String co = co_i < co_list.GetCount() ? co_list[co_i].key : String();
			song_typecasts.Set(i, 0, 1+i);
			song_typecasts.Set(i, 1, tc);
			song_typecasts.Set(i, 2, co);
		}
		INHIBIT_CURSOR(song_typecasts);
		song_typecasts.SetCount(o.typeclasses.GetCount());
		if (song_typecasts.GetCount() && !song_typecasts.IsCursor())
			song_typecasts.SetCursor(0);
	}
	
	{
		for(int i = 0; i < o.lyrics_ideas.GetCount(); i++) {
			String idea = o.lyrics_ideas[i];
			lyrics_ideas.Set(i, 0, 1+i);
			lyrics_ideas.Set(i, 1, idea);
		}
		INHIBIT_CURSOR(lyrics_ideas);
		lyrics_ideas.SetCount(o.lyrics_ideas.GetCount());
		if (lyrics_ideas.GetCount() && !lyrics_ideas.IsCursor())
			lyrics_ideas.SetCursor(0);
	}
	
	{
		for(int i = 0; i < o.music_styles.GetCount(); i++) {
			String s = o.music_styles[i];
			music_styles.Set(i, 0, 1+i);
			music_styles.Set(i, 1, s);
		}
		INHIBIT_CURSOR(music_styles);
		music_styles.SetCount(o.music_styles.GetCount());
		if (music_styles.GetCount() && !music_styles.IsCursor())
			music_styles.SetCursor(0);
	}
	
}

void LeadSourceCtrl::DataAnalyzedList() {
	DatasetPtrs p; GetDataset(p);
	LeadData& ld = *p.lead_data;
	
	
	if (!list.IsCursor() || !list_names.IsCursor())
		return;
	
	int idx = list.Get("IDX");
	int list_idx = list_names.GetCursor();
	LeadOpportunity& o = ld.opportunities[idx];
	
	const Vector<String>& l = o.analyzed_lists[list_idx];
	
	{
		for(int i = 0; i < l.GetCount(); i++) {
			const String& v = l[i];
			list_values.Set(i, 0, v);
		}
		INHIBIT_CURSOR(list_values);
		list_values.SetCount(l.GetCount());
	}
	
}

void LeadSourceCtrl::ToolMenu(Bar& bar) {
	bar.Add(t_("Refresh"), MetaImgs::BlueRing(), THISBACK(Data)).Key(K_CTRL_Q);
	bar.Separator();
	//bar.Add(t_("Update website leads"), MetaImgs::RedRing(), THISBACK1(Do, 0)).Key(K_F5);
	bar.Add(t_("Update website leads (with MetaEntity)"), MetaImgs::RedRing(), THISBACK1(Do, 1)).Key(K_F6);
	bar.Separator();
	bar.Add(t_("Create script"), MetaImgs::BlueRing(), THISBACK(CreateScript)).Key(K_F7);
	bar.Add(t_("Copy script header to clipboard"), MetaImgs::BlueRing(), THISBACK(CopyHeaderClipboard)).Key(K_F8);
	bar.Separator();
	bar.Add(t_("Import json"), THISBACK(ImportJson));
}

void LeadSourceCtrl::Do(int fn) {
	if (fn == 0) {
		DatasetPtrs p; GetDataset(p);
		LeadSolver& tm = LeadSolver::Get(p);
		tm.Start();
	}
	else if (fn == 1) {
		DatasetPtrs p; GetDataset(p);
		if (p.owner) {
			LeadSolver& tm = LeadSolver::Get(p);
			tm.Start();
		}
	}
}

void LeadSourceCtrl::CreateScript() {
	DatasetPtrs p; GetDataset(p);
	LeadData& ld = *p.lead_data;
	
	int appmode = DB_SONG; // TODO remove
	LOG("LeadSourceCtrl::CreateScript: warning: TODO: int appmode = DB_SONG;");
	
	if (!list.IsCursor())
		return;
	
	int idx = list.Get("IDX");
	LeadOpportunity& o = ld.opportunities[idx];
	
	if (o.typeclasses.IsEmpty() ||
		o.contents.IsEmpty() ||
		o.lyrics_ideas.IsEmpty() ||
		o.music_styles.IsEmpty()) {
		PromptOK(DeQtf(t_("No typecast/idea/style")));
		return;
	}
	
	Entity& e = *p.entity;
	
	int tc_i = o.typeclasses[0];
	int con_i = o.contents[0];
	String lyrics_idea = o.lyrics_ideas[0];
	String music_style = o.music_styles[0];
	
	if (tc_i < 0 || tc_i >= UPP::GetTypeclasses(appmode).GetCount()) {
		PromptOK(DeQtf(t_("Invalid typeclass")));
		return;
	}
	
	if (con_i < 0 || con_i >= UPP::GetContents(appmode).GetCount()) {
		PromptOK(DeQtf(t_("Invalid content")));
		return;
	}
	String snap_title = Format("%d %Month", o.first_seen.year, o.first_seen.month);
	String script_title = "Lead #" + IntStr(idx);
	
	PromptOK("TODO");
	#if 0
	Release& snap = e.GetAddSnapshot(snap_title);
	Component& comp = snap.GetAddComponent(script_title);
	//e.RealizeTypeclasses(appmode);
	Script& script = e.GetAddScript(script_title);
	
	comp.style = music_style;
	if (mp.owner)
		script.copyright = mp.owner->name;
	script.typeclass = tc_i;
	script.content = con_i;
	script.content_vision = lyrics_idea;
	//script.user_structure = GetDefaultScriptStructureString(GetAppMode());
	#endif
}

void LeadSourceCtrl::CopyHeaderClipboard() {
	DatasetPtrs mp; GetDataset(mp);
	LeadData& ld = *mp.lead_data;
	
	if (!list.IsCursor())
		return;
	
	int idx = list.Get("IDX");
	LeadOpportunity& o = ld.opportunities[idx];
	
	if (o.typeclasses.IsEmpty() ||
		o.contents.IsEmpty() ||
		o.lyrics_ideas.IsEmpty() ||
		o.music_styles.IsEmpty()) {
		PromptOK(DeQtf(t_("No typecast/idea/style")));
		return;
	}
	
	SongHeaderArgs args;
	args.tc_i = o.typeclasses[0];
	args.con_i = o.contents[0];
	args.lyrics_idea = o.lyrics_ideas[0];
	args.music_style = o.music_styles[0];
	
	String txt = args.Get();
	WriteClipboardText(txt);
}

void LeadSourceCtrl::ImportJson() {
	DatasetPtrs p; GetDataset(p);
	LeadData& ld = GetExt<LeadData>();
	if (LoadFromJsonFile_VisitorNodePrompt(ld)) {
		PostCallback(THISBACK(Data));
	}
}

















ArrayMap<hash_t, LeadSolver>& __LeadSolvers() {
	static ArrayMap<hash_t, LeadSolver> map;
	return map;
}


LeadSolver::LeadSolver() {
	
}

LeadSolver& LeadSolver::Get(DatasetPtrs p) {
	ASSERT(p.entity && p.owner);
	String t = p.entity->val.GetPath();
	hash_t h = t.GetHashValue();
	ArrayMap<hash_t, LeadSolver>& map = __LeadSolvers();
	int i = map.Find(h);
	if (i >= 0)
		return map[i];
	
	LeadSolver& ls = map.Add(h);
	ls.p = p;
	return ls;
}

int LeadSolver::GetPhaseCount() const {
	return LS_COUNT;
}

void LeadSolver::DoPhase() {
	LeadData& ld = *p.lead_data;
	
	// Don't process all data with AI when using generic updater profile,
	// because more costly AI profile is used:
	// skip after booleans
	bool reduce_load = false; TODO // owner == &Owner::DatabaseUpdate();
	
	if (phase == LS_DOWNLOAD_WEBSITES) {
		ProcessDownloadWebsites(false);
	}
	else if (phase == LS_PARSE_WEBSITES) {
		ProcessDownloadWebsites(true); // this won't actually download the website again
	}
	else if (phase == LS_ANALYZE_BOOLEANS) {
		ProcessAnalyzeBooleans();
	}
	else if (phase == LS_ANALYZE_STRINGS) {
		if (reduce_load) {
			phase = LS_COUNT;
			return;
		}
		ProcessAnalyzeStrings();
	}
	else if (phase == LS_ANALYZE_LISTS) {
		ProcessAnalyzeLists();
	}
	else if (phase == LS_COARSE_RANKING) {
		ProcessCoarseRanking();
	}
	else if (phase == LS_AVERAGE_PAYOUT_ESTIMATION) {
		ProcessAveragePayoutEstimation();
	}
	else if (phase == LS_ANALYZE_POTENTIAL_SONG_TYPECAST) {
		ProcessAnalyzeSongTypecast();
	}
	else if (phase == LS_ANALYZE_POTENTIAL_SONG_IDEAS) {
		ProcessAnalyzeLyricsIdeas();
	}
	else if (phase == LS_ANALYZE_POTENTIAL_MUSIC_STYLE_TEXT) {
		ProcessAnalyzeMusicStyle();
	}
	else if (phase == LS_TEMPLATE_TITLE_AND_TEXT) {
		ProcessTemplateTitleAndText();
	}
	else if (phase == LS_TEMPLATE_ANALYZE) {
		ProcessTemplateAnalyze();
	}
}

String LeadSolver::GetLeadCacheDir() {
	String dir = ConfigFile("lead-cache");
	RealizeDirectory(dir);
	return dir;
}

void LeadSolver::ProcessDownloadWebsites(bool parse) {
	String id = ToLower(p.entity->val.id);
	if (id.Find("taxi") == 0) {
		String url = "https://www.taxi.com/industry";
		String content = ProcessDownloadWebsiteUrl(url);
		if (parse)
			ParseWebsite(batch, content);
		NextPhase();
	}
	else if (id.Find("sonicbids") == 0) {
		int page = sub_batch+1;
		String url = "https://www.sonicbids.com/find-gigs?type=LICENSING&type=COMPETITION&page=" + IntStr(page);
		String content = ProcessDownloadWebsiteUrl(url);
		if (parse)
			ParseWebsite(batch, content);
		if (content.Find(";page=" + IntStr(page+1) + "\"") >= 0)
			NextSubBatch();
		else
			NextPhase();
	}
	else {
		SetWaiting(0);
		SetNotRunning();
		SetError("unexpected id: " + id);
	}
}

void LeadSolver::ParseWebsite(int batch, String content) {
	LeadData& ld = *p.lead_data;
	content.Replace("\r", "");
	
	String id = ToLower(p.entity->val.id);
	
	if (id.Find("taxi") == 0) {
		Vector<String> categories = Split(content, "<div class='genre-title'>");
		if (categories.IsEmpty()) return;
		categories.Remove(0);
		for (String& cat_str : categories) {
			Vector<String> listings = Split(cat_str, "<div class='listing' id='");
			if (listings.IsEmpty()) continue;
			listings.Remove(0);
			
			
			for (String& listing_str : listings) {
				// Pick unique listing id from the beginning of the string
				int a = listing_str.Find("'");
				if (a < 0) continue;
				String id = listing_str.Left(a);
				
				// Trim begin
				a = listing_str.Find("\n");
				if (a < 0) continue;
				listing_str = listing_str.Mid(a+1);
				
				// Trim end
				a = listing_str.Find("<p class='buttons-title'>");
				if (a < 0) continue;
				listing_str = listing_str.Left(a);
				
				Vector<String> links;
				String text = DeHtml(listing_str, links);
				
				
				double payout = 0;
				a = text.Find("$");
				if (a >= 0) {
					a++;
					String payout_str;
					for(int i = a; i < text.GetCount(); i++) {
						int chr = text[i];
						if (chr == ',')
							continue;
						if (IsDigit(chr) || chr == '.')
							payout_str.Cat(chr);
						else break;
					}
					payout = ScanDouble(payout_str);
				}
				
				/*LOG(id);
				LOG(text);
				DUMPC(links);*/
				
				
				String title;
				{
					Vector<String> words = Split(text, " ");
					for(int i = 0; i < words.GetCount(); i++) {
						String& w = words[i];
						if (w.Left(1) == "'" || IsAllUpper(w) ||
							w == "A" || w == "Bunch" || w == "Wide" || w == "Range" ||
							w == "Lots" || w == "Tons" || w == "of") {
							if (!title.IsEmpty()) title.Cat(' ');
							title << w;
						}
						else break;
					}
					if (title.IsEmpty()) {
						for(int i = 0; i < 4 && i < words.GetCount(); i++) {
							if (!title.IsEmpty()) title.Cat(' ');
							title << words[i];
						}
					}
				}
				
				LeadOpportunity& o = ld.GetAddOpportunity(batch, id);
				o.name = title;
				o.request_description = text;
				o.links <<= links;
				o.min_compensation = o.max_compensation = (int)payout;
				
				// Add very coarse royalty estimates. Without any of these, calculations will
				// break.
				o.min_compensation += 500;
				o.max_compensation += 6000;
			}
		}
	}
	else if (id.Find("sonicbids") == 0) {
		//LOG(content);
		
		int a = content.Find("require.config['opportunity-search']");
		if (a < 0) return;
		a = content.Find("{", a);
		if (a < 0) return;
		a = content.Find("data", a);
		if (a < 0) return;
		a = content.Find(":", a);
		if (a < 0) return;
		a++;
		int b = content.Find("</script>", a);
		if (b < 0) return;
		b = content.ReverseFind("};", b);
		String json = content.Mid(a,b-a);
		//DUMP(json);
		
		Value v = ParseJSON(json);
		//LOG(AsJSON(v, true));
		ValueMap root = v;
		ValueArray opportunities = root.GetAdd("opportunities");
		for(int i = 0; i < opportunities.GetCount(); i++) {
			ValueMap opp = opportunities[i];
			String id_str = IntStr(opp.GetAdd("id"));
			LeadOpportunity& o = ld.GetAddOpportunity(batch, id_str);
			
			o.genres.Clear();
			ValueArray genres = opp.GetAdd("entry_genres");
			for(int j = 0; j < genres.GetCount(); j++) {
				ValueMap genre = genres[j];
				LeadOpportunity::Genre& g = o.genres.Add();
				g.id = genre.GetAdd("id");
				g.name = genre.GetAdd("name");
				g.primary = genre.GetAdd("primary");
			}
			
			o.promoter_group_genres.Clear();
			ValueArray promoter_genres = opp.GetAdd("promoter_group_genres");
			for(int j = 0; j < promoter_genres.GetCount(); j++) {
				ValueMap genre = promoter_genres[j];
				LeadOpportunity::Genre& g = o.promoter_group_genres.Add();
				g.id = genre.GetAdd("id");
				g.name = genre.GetAdd("name");
				g.primary = genre.GetAdd("primary");
			}
			
			o.name = opp.GetAdd("name");
			o.band_opportunity_type = opp.GetAdd("band_opportunity_type");
			o.obj_class = opp.GetAdd("obj_class");
			o.request_entry_fee = opp.GetAdd("request_entry_fee");
			o.request_featured = opp.GetAdd("request_featured");
			o.request_exclusive = opp.GetAdd("request_exclusive");
			o.request_curated = opp.GetAdd("request_curated");
			o.request_contest = opp.GetAdd("request_contest");
			o.request_comments = opp.GetAdd("request_comments");
			o.request_first_name = opp.GetAdd("request_first_name");
			o.request_last_name = opp.GetAdd("request_last_name");
			o.request_email = opp.GetAdd("request_email");
			o.request_phone = opp.GetAdd("request_phone");
			o.request_description = opp.GetAdd("request_description");
			o.request_opportunity_description = opp.GetAdd("request_opportunity_description");
			o.request_band_description = opp.GetAdd("request_band_description");
			o.request_selection_description = opp.GetAdd("request_selection_description");
			o.vanity_url_id = opp.GetAdd("vanity_url_id");
			o.vanity_url_name = opp.GetAdd("vanity_url_name");
			o.status_text = opp.GetAdd("status_text");
			o.id = id_str;
			o.description = opp.GetAdd("description");
			o.band_opportunity_type_text = opp.GetAdd("band_opportunity_type_text");
			o.local_event_end_datetime = opp.GetAdd("local_event_end_datetime");
			o.public_image_url = opp.GetAdd("public_image_url");
			o.logo_image_url = opp.GetAdd("logo_image_url");
			o.promoter_group_name = opp.GetAdd("promoter_group_name");
			o.promoter_group_main_image_url = opp.GetAdd("promoter_group_main_image_url");
			o.promoter_group_facebook_url = opp.GetAdd("promoter_group_facebook_url");
			o.promoter_group_twitter_url = opp.GetAdd("promoter_group_twitter_url");
			o.promoter_group_youtube_url = opp.GetAdd("promoter_group_youtube_url");
			o.promoter_group_instagram_url = opp.GetAdd("promoter_group_instagram_url");
			o.promoter_group_talent_description = opp.GetAdd("promoter_group_talent_description");
			o.promoter_group_short_description = opp.GetAdd("promoter_group_short_description");
			o.promoter_group_talent_roster = opp.GetAdd("promoter_group_talent_roster");
			o.promoter_group_opportunity_frequency_count = opp.GetAdd("promoter_group_opportunity_frequency_count");
			o.promoter_group_opportunity_frequency = opp.GetAdd("promoter_group_opportunity_frequency");
			o.min_compensation = opp.GetAdd("min_compensation");
			o.max_compensation = opp.GetAdd("max_compensation");
			o.pay_to_apply = opp.GetAdd("pay_to_apply");
			o.free_to_apply = opp.GetAdd("free_to_apply");
			o.entry_count = opp.GetAdd("entry_count");
			o.entry_end_datetime = opp.GetAdd("entry_end_datetime");
			o.date_created = opp.GetAdd("date_created");
			o.compensated = opp.GetAdd("compensated");
			o.min_entry_price_cents = opp.GetAdd("min_entry_price_cents");
			
			if (o.compensated && o.max_compensation == 0) {
				o.min_compensation = o.max_compensation = 1;
			}
			
			if (o.pay_to_apply && o.min_entry_price_cents == 0) {
				o.min_entry_price_cents = 1;
			}
			
			o.request_description.Replace("\r", "");
			
			Vector<String> links;
			o.request_opportunity_description = DeHtml(o.request_opportunity_description, links);
			o.request_band_description = DeHtml(o.request_band_description, links);
			o.request_selection_description = DeHtml(o.request_selection_description, links);
			o.links <<= links;
		}
	}
	
}

static size_t CurlWriteCallback(void *contents, size_t size, size_t nmemb, void *userp)
{
    ((String*)userp)->Cat((char*)contents, (int)(size * nmemb));
    return size * nmemb;
}

String LeadSolver::ProcessDownloadWebsiteUrl(String url) {
	LeadCache cache;
	Time last_update = cache.last_update.Get(url, Time(1970,1,1));
	Time limit = GetSysTime() - 60*60*24;
	String dir = GetLeadCacheDir();
	String fname = Base64Encode(url) + ".html";
	fname.Replace("/", "");
	fname.Replace("\\", "");
	String path = AppendFileName(dir, fname);
	String prev = LoadFile(path);
	if (last_update <= limit || prev.IsEmpty()) {
		String proxy = GetGlobalProxy();
		String content;
		
		if (1) {
			int proxy_port = 0;
			
			int a = proxy.Find("://");
			if (a >= 0) a = proxy.Find(":", a+1);
			if (a >= 0) {
				proxy_port = ScanInt(proxy.Mid(a+1));
				proxy = proxy.Left(a);
			}
			a = proxy.Find("http://");
			if (a >= 0) proxy = proxy.Mid(a+7);
			a = proxy.Find("https://");
			if (a >= 0) proxy = proxy.Mid(a+8);
			
			HttpRequest http;
			//http.Trace();
			http.Url(url);
			http.UserAgent("Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/123.0.0.0 Safari/537.3");
			
			if (proxy_port > 0) {
				http.Proxy(proxy, proxy_port);
				http.SSLProxy(proxy, proxy_port);
			}
			content = http.Execute();
		}
		#if 0
		else {
			CURL *curl;
			CURLcode res;
			
			curl = curl_easy_init();
			if(curl) {
				curl_easy_setopt(curl, CURLOPT_URL, url.Begin());
				
				// url is redirected, so we tell libcurl to follow redirection
				curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
				
				if (proxy.GetCount())
					curl_easy_setopt(curl, CURLOPT_PROXY, proxy.Begin());
				
				curl_easy_setopt(curl, CURLOPT_WRITEDATA, &content);
				curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, CurlWriteCallback);
				
				// Perform the request, res gets the return code
				res = curl_easy_perform(curl);
				
				// Check for errors
				if(res != CURLE_OK)
					fprintf(stderr, "curl_easy_perform() failed: %s\n",
				curl_easy_strerror(res));
				
				// always cleanup
				curl_easy_cleanup(curl);
			}
		}
		#endif
		
		FileOut fout(path);
		fout << content;
		fout.Close();
		cache.last_update.GetAdd(url) = GetSysTime();
		return content;
	}
	return prev;
}

void LeadSolver::ProcessAnalyzeFn(int fn, Event<String> cb) {
	LeadData& ld = *p.lead_data;
	LeadSolverArgs args;
	args.opp_i = batch;
	args.fn = fn;
	
	if (batch >= ld.opportunities.GetCount()) {
		NextPhase();
		return;
	}
	
	
	SetWaiting(1);
	TaskMgr& m = AiTaskManager();
	m.GetLeadSolver(args, cb);
}

void LeadSolver::ProcessAnalyzeBooleans() {
	LeadData& ld = *p.lead_data;
	if (batch >= ld.opportunities.GetCount()) {
		NextPhase();
		return;
	}
	LeadOpportunity& opp = ld.opportunities[batch];
	if ((skip_ready && !opp.analyzed_booleans.IsEmpty())) {
		NextBatch();
		return;
	}
	
	ProcessAnalyzeFn(0, THISBACK(OnProcessAnalyzeBooleans));
}

void LeadSolver::OnProcessAnalyzeBooleans(String res) {
	LeadData& ld = *p.lead_data;
	LeadOpportunity& opp = ld.opportunities[batch];
	
	RemoveEmptyLines2(res);
	
	Vector<String> lines = Split(res, "\n");
	
	if (lines.GetCount() == LISTING_SONG_BOOLEAN_COUNT+1)
		lines.Remove(0);
	
	bool fast_analyse = lines.GetCount() == LISTING_SONG_BOOLEAN_COUNT;
	
	opp.analyzed_booleans.SetCount(LISTING_SONG_BOOLEAN_COUNT, 0);
	
	for(int i = 0; i < lines.GetCount(); i++) {
		String& line = lines[i];
		line = TrimBoth(line);
		
		int a = line.Find(":");
		if (a < 0) continue;
		String key = TrimBoth(line.Left(a));
		String value = ToLower(TrimBoth(line.Mid(a+1)));
		
		int idx = i;
		if (!fast_analyse) {
			idx = -1;
			for(int j = 0; j < LISTING_SONG_BOOLEAN_COUNT; j++) {
				if (key == GetSongListingBooleanKey(j)) {
					idx = j;
					break;
				}
			}
			if (idx < 0)
				continue;
		}
		
		bool b = value.Find("true") == 0;
		opp.analyzed_booleans[idx] = b;
	}
	
	
	NextBatch();
	SetWaiting(0);
}

double LeadSolver::GetAverageOpportunityScore() {
	LeadData& ld = *p.lead_data;
	double score_sum = 0;
	for (const LeadOpportunity& opp : ld.opportunities)
		score_sum += p.owner->GetOpportunityScore(opp);
	double score_av = score_sum / ld.opportunities.GetCount();
	return score_av;
}

bool LeadSolver::SkipLowScoreOpportunity() {
	LeadData& ld = *p.lead_data;
	double score_limit = GetAverageOpportunityScore() * score_limit_factor;
	LeadOpportunity& opp = ld.opportunities[batch];
	int score = p.owner->GetOpportunityScore(opp);
	return score < score_limit && opp.min_compensation <= 0;
}

void LeadSolver::ProcessAnalyzeStrings() {
	LeadData& ld = *p.lead_data;
	if (batch >= ld.opportunities.GetCount()) {
		NextPhase();
		return;
	}
	
	LeadOpportunity& opp = ld.opportunities[batch];
	if ((skip_ready && (!opp.analyzed_string.IsEmpty())) ||
		SkipLowScoreOpportunity()) {
		NextBatch();
		return;
	}
	
	ProcessAnalyzeFn(1, THISBACK(OnProcessAnalyzeStrings));
}

void LeadSolver::OnProcessAnalyzeStrings(String res) {
	LeadData& ld = *p.lead_data;
	LeadOpportunity& opp = ld.opportunities[batch];
	
	/*
	 deal structure (e.g. exclusive): not specified
	3. deal type (e.g. song placement, radio play): not specified
	4. artist's royalty percentage: not specified
	5. who is the decision maker: Jared Hassan Foles - Producer/Chief Engineer of World Eater Recordings
	6. what kind of sound the song should have: able to achieve desired results in sound
	7. type of the target movie / advertisement (e.g. romantic, sport product): not specified
	8. based on the language and tone, what type of company/person wrote this listing: professional and experienced in the music industry
	9. based on the language and tone, what type of artist does the company/person want to work with: open to collaborating with all types of artists
	10. based on general assumptions, what information is lacking about the context: specific goals or end result desired by the company/person
	11. based on general assumptions, what information is lacking about the song: current genre or style of the song
	12. based on general assumptions, what guidelines could be used while deciding what kind of song to make: any type of song that fits within the artist's goals and desired results
	13. based on general assumptions,  what kind of monetary income can be expected by getting accepted in this listing: not specified
	14. based on general assumptions, what kind of level of competition is expected for this listing: not specified, but it can be assumed that there will be competition among songwriters and musicians.
	*/
	RemoveEmptyLines2(res);
	
	Vector<String> lines = Split(res, "\n");
	
	if (lines.GetCount() == LISTING_SONG_STRING_COUNT+1)
		lines.Remove(0);
	
	bool fast_analyse = lines.GetCount() == LISTING_SONG_STRING_COUNT;
	
	opp.analyzed_string.SetCount(LISTING_SONG_STRING_COUNT);
	
	for(int i = 0; i < lines.GetCount(); i++) {
		String& line = lines[i];
		line = TrimBoth(line);
		
		int a = line.Find(":");
		if (a < 0) continue;
		String key = TrimBoth(line.Left(a));
		String value = TrimBoth(line.Mid(a+1));
		
		int idx = i;
		if (!fast_analyse) {
			idx = -1;
			for(int j = 0; j < LISTING_SONG_STRING_COUNT; j++) {
				if (key == GetSongListingStringKey(j)) {
					idx = j;
					break;
				}
			}
			if (idx < 0)
				continue;
		}
		
		opp.analyzed_string[idx] = value;
	}
	
	
	NextBatch();
	SetWaiting(0);
}

void LeadSolver::ProcessAnalyzeLists() {
	LeadData& ld = *p.lead_data;
	if (batch >= ld.opportunities.GetCount()) {
		NextPhase();
		return;
	}
	LeadOpportunity& opp = ld.opportunities[batch];
	if ((skip_ready && (!opp.analyzed_lists.IsEmpty())) ||
		SkipLowScoreOpportunity()) {
		NextBatch();
		return;
	}
	
	ProcessAnalyzeFn(2, THISBACK(OnProcessAnalyzeLists));
}

void LeadSolver::OnProcessAnalyzeLists(String res) {
	LeadData& ld = *p.lead_data;
	LeadOpportunity& opp = ld.opportunities[batch];
	
	/*
	list of similar sounding artists: [Calvin Harris, Martin Garrix, Avicii]
	3. list of Data, what can be interpreted from this: [completed single, EDM, writer, producer, management, artist development, production, publishing, co-publishing, record deal]
	4. what kind of tones and moods could be suggested for the song for this opportunity: [energetic, danceable, electronic]
	5. List of "does this listing have increased chances of" for "Based on assumptions about pop music and music producers/industry": [getting your single noticed, securing a record deal or co-publishing agreement, gaining exposure and potential success in the EDM genre]
	6. List of "does this kind of song get selected" for "Based on assumptions about pop music and music producers/industry": [songs that are well-produced and have a strong EDM influence, songs with high energy and a catchy beat, songs that are unique and stand out from the crowd]
	*/
	
	RemoveEmptyLines2(res);
	
	Vector<String> lines = Split(res, "\n");
	
	if (lines.GetCount() == LISTING_SONG_LIST_COUNT+1)
		lines.Remove(0);
	
	bool fast_analyse = lines.GetCount() == LISTING_SONG_LIST_COUNT;
	
	opp.analyzed_lists.SetCount(LISTING_SONG_LIST_COUNT);
	
	for(int i = 0; i < lines.GetCount(); i++) {
		String& line = lines[i];
		line = TrimBoth(line);
		
		int a = line.Find(":");
		if (a < 0) continue;
		String key = TrimBoth(line.Left(a));
		String value = ToLower(TrimBoth(line.Mid(a+1)));
		
		int idx = i;
		if (!fast_analyse) {
			idx = -1;
			for(int j = 0; j < LISTING_SONG_LIST_COUNT; j++) {
				if (key == GetSongListingListKey(j)) {
					idx = j;
					break;
				}
			}
			if (idx < 0)
				continue;
		}
		
		Vector<String>& list = opp.analyzed_lists[idx];
		
		a = value.Find("[");
		if (a >= 0) {
			a++;
			int b = value.ReverseFind("]");
			if (b >= 0) {
				value = value.Mid(a,b-a);
			}
		}
		
		list = Split(value, ", ");
	}
	
	
	
	NextBatch();
	SetWaiting(0);
}

void LeadSolver::ProcessCoarseRanking() {
	LeadData& ld = *p.lead_data;
	VectorMap<int,double> money_scores, opp_scores;
	
	for(int i = 0; i < ld.opportunities.GetCount(); i++) {
		LeadOpportunity& o = ld.opportunities[i];
		
		double price = 0.01 * o.min_entry_price_cents;
		
		double av_compensation = 0.5 * (o.min_compensation + max(o.min_compensation, o.max_compensation));
		
		double money_score = 0;
		if (price > 0 && av_compensation) {
			money_score = av_compensation / price * 1000;
		}
		else if (av_compensation > 0) {
			money_score = av_compensation;
		}
		// Punish expensive listings
		if (price > 50)
			money_score -= 100;
		// Reward easy and significant income
		if (LISTING_SONG_BOOLEAN_MONETARY_SIGNIFICANT_INCOME < o.analyzed_booleans.GetCount() &&
			o.analyzed_booleans[LISTING_SONG_BOOLEAN_MONETARY_SIGNIFICANT_INCOME] > 0) {
			money_score += 100;
			if (o.analyzed_booleans[LISTING_SONG_BOOLEAN_MONETARY_DIFFICULT_TO_DETERMINE] == 0)
				money_score += 200;
		}
		
		money_scores.Add(i, money_score);
		
		int opp_score =
			p.owner ?
				p.owner->GetOpportunityScore(o) :
				-1;
		opp_scores.Add(i, opp_score);
	}
	
	SortByValue(money_scores, StdGreater<double>());
	SortByValue(opp_scores, StdGreater<double>());
	
	for(int i = 0; i < money_scores.GetCount(); i++) {
		int opp_i = money_scores.GetKey(i);
		LeadOpportunity& o = ld.opportunities[opp_i];
		int rank = min(max_rank, 1+i);
		o.money_score = money_scores[i];
		o.money_score_rank = rank;
	}
	for(int i = 0; i < opp_scores.GetCount(); i++) {
		int opp_i = opp_scores.GetKey(i);
		LeadOpportunity& o = ld.opportunities[opp_i];
		int rank = min(max_rank, 1+i);
		o.opp_score = opp_scores[i];
		o.opp_score_rank = rank;
	}
	for (LeadOpportunity& o : ld.opportunities) {
		o.weighted_rank =
			o.money_score_rank * 0.3 +
			o.opp_score_rank * 0.7;
	}
	
	NextPhase();
}

void LeadSolver::ProcessAveragePayoutEstimation() {
	LeadData& ld = *p.lead_data;
	if (batch >= ld.opportunities.GetCount()) {
		NextPhase();
		return;
	}
	LeadOpportunity& opp = ld.opportunities[batch];
	if (opp.weighted_rank >= (double)max_rank) {
		NextBatch();
		return;
	}
	
	if (skip_ready && opp.chance_list.GetCount()) {
		NextBatch();
		return;
	}
	
	ProcessAnalyzeFn(3, THISBACK(OnProcessAveragePayoutEstimation));
}

void LeadSolver::OnProcessAveragePayoutEstimation(String res) {
	LeadData& ld = *p.lead_data;
	LeadOpportunity& opp = ld.opportunities[batch];
	
	/*
	Initial listen: 70%
	- Production quality: 50%
	- Comparison to other submissions: 40%
	- Collaboration potential: 20%
	- Refinement and final review: 15%
	- Top contender selection: 10%
	- Total chance of acceptance: 2.1% (0.7 x 0.5 x 0.4 x 0.2 x 0.15 x 0.1 = 0.0021 = 0.21%). Again, keep in mind that these numbers are theoretical and may vary.
	- Average payout estimation for accepted song: $1,250 x 2.1% = $26.25 or approximately $26.
	*/
	
	if (res.Find("\n2.") >= 0)
		RemoveEmptyLines2(res);
	else
		RemoveEmptyLines3(res);
	
	Vector<String> lines = Split(res, "\n");
	
	double chance = 1.0;
	
	opp.chance_list.Clear();
	for(int i = 0; i < lines.GetCount(); i++) {
		String& line = lines[i];
		line = TrimBoth(line);
		int a0 = line.Find("otal chance of acceptance");
		int a1 = line.Find("verage payout estimation");
		if (a0 >= 0) {
			/*int a = line.Find(":");
			if (a >= 0) {
				a++;
				String value = TrimBoth(line.Mid(a));
				
				a = value.Find("%");
				if (a >= 0) {
					String perc_str = value.Left(a);
					int begin = 0;
					for (int j = perc_str.GetCount()-1; j >= 0; j--) {
						int chr = perc_str[j];
						if (IsDigit(chr) || chr == '.')
							continue;
						else {
							begin = j+1;
							break;
						}
					}
					perc_str = TrimBoth(value.Mid(begin));
					opp.chance_of_acceptance = ScanDouble(perc_str) * 0.01;
				}
			}*/
		}
		/*else if (a1 >= 0) {
			int a = line.Find(":");
			if (a >= 0) {
				a++;
				String value = TrimBoth(line.Mid(a));
				
				a = value.Find("=");
				if (a >= 0) {
					String money_str;
					for(int j = a+1; j < value.GetCount(); j++) {
						int chr = value[j];
						if (IsSpace(chr) || chr == ',' || chr == '$')
							continue;
						if (IsDigit(chr) || chr == '.')
							money_str.Cat(chr);
						else
							break;
					}
					opp.average_payout_estimation = ScanDouble(money_str);
				}
			}
		}*/
		else {
			int a = line.Find(":");
			if (a >= 0) {
				a++;
				String value = TrimBoth(line.Mid(a));
				double factor = ScanDouble(value) * 0.01;
				chance *= factor;
			}
			opp.chance_list << line;
		}
	}
	
	if (chance == 1.0)
		opp.chance_of_acceptance = 0.0001;
	else
		opp.chance_of_acceptance = chance;
	
	// Hotfix taxi.com: have at least 4% chance. Their average is 6%
	if (opp.chance_of_acceptance <= 0.04)
		opp.chance_of_acceptance += 0.04;
	
	opp.average_payout_estimation =
		opp.chance_of_acceptance * (opp.min_compensation + opp.max_compensation) * 0.5;
	
	NextBatch();
	SetWaiting(0);
}

void LeadSolver::ProcessAnalyzeSongTypecast() {
	LeadData& ld = *p.lead_data;
	if (batch >= ld.opportunities.GetCount()) {
		NextPhase();
		return;
	}
	LeadOpportunity& opp = ld.opportunities[batch];
	if ((skip_ready && !(opp.contents.IsEmpty() || opp.typeclasses.IsEmpty())) ||
		SkipLowScoreOpportunity() ||
		opp.weighted_rank >= (double)max_rank ||
		opp.average_payout_estimation <= 0.1) {
		NextBatch();
		return;
	}
	
	ProcessAnalyzeFn(4, THISBACK(OnProcessAnalyzeSongTypecast));
}

void LeadSolver::OnProcessAnalyzeSongTypecast(String res) {
	LeadData& ld = *p.lead_data;
	LeadOpportunity& opp = ld.opportunities[batch];
	
	/*
	1,2
	- 3,4
	- 5,6
	- 7,8
	- 9,10
	*/
	Vector<String> lines = Split(res, "\n");
	
	opp.typeclasses.Clear();
	opp.contents.Clear();
	for(int i = 0; i < lines.GetCount(); i++) {
		String& line = lines[i];
		{
			int a;
			while ((a = line.Find("(")) >= 0) {
				int b = line.Find(")", a);
				if (b < 0) break;
				line = line.Left(a) + line.Mid(b+1);
			}
		}
		int a = line.Find(".");
		if (a >= 0)
			line = line.Mid(a+1);
		line = TrimBoth(line);
		Vector<String> parts = Split(line, ",");
		if (parts.GetCount() < 2)
			continue;
		int tc = ScanInt(TrimLeft(parts[0])) - 1;
		int co = ScanInt(TrimLeft(parts[1])) - 1;
		opp.typeclasses << tc;
		opp.contents << co;
	}
	
	
	NextBatch();
	SetWaiting(0);
}

void LeadSolver::ProcessAnalyzeLyricsIdeas() {
	LeadData& ld = *p.lead_data;
	if (batch >= ld.opportunities.GetCount()) {
		NextPhase();
		return;
	}
	LeadOpportunity& opp = ld.opportunities[batch];
	if ((skip_ready && !opp.lyrics_ideas.IsEmpty()) ||
		SkipLowScoreOpportunity() ||
		opp.weighted_rank >= (double)max_rank ||
		opp.average_payout_estimation <= 0.1) {
		NextBatch();
		return;
	}
	
	ProcessAnalyzeFn(5, THISBACK(OnProcessAnalyzeLyricsIdeas));
}

void LeadSolver::OnProcessAnalyzeLyricsIdeas(String res) {
	LeadData& ld = *p.lead_data;
	LeadOpportunity& opp = ld.opportunities[batch];
	
	Vector<String> lines = Split(res, "\n");
	
	opp.lyrics_ideas.Clear();
	for(int i = 0; i < lines.GetCount(); i++) {
		String& line = lines[i];
		if (line.IsEmpty()) continue;
		
		int a = line.Find(".");
		bool no_beginning = (i == 0 && (a < 0 || a >= 4));
		if (a >= 0 && a < 4)
			line = line.Mid(a+1);
		line = TrimBoth(line);
		
		if (line.IsEmpty()) continue;
		int chr = line[0];
		if (no_beginning && chr >= 'a' && chr <= 'z' && line.Find("he lyrics is about") < 0)
			line = "The lyrics is about " + line;
		
		a = line.Find("** - ");
		if (a >= 0)
			line = line.Mid(a+5);
		
		opp.lyrics_ideas << line;
	}
	
	
	NextBatch();
	SetWaiting(0);
}

void LeadSolver::ProcessAnalyzeMusicStyle() {
	LeadData& ld = *p.lead_data;
	if (batch >= ld.opportunities.GetCount()) {
		NextPhase();
		return;
	}
	LeadOpportunity& opp = ld.opportunities[batch];
	if ((skip_ready && !opp.music_styles.IsEmpty()) ||
		SkipLowScoreOpportunity() ||
		opp.weighted_rank >= (double)max_rank||
		opp.average_payout_estimation <= 0.1) {
		NextBatch();
		return;
	}
	
	ProcessAnalyzeFn(6, THISBACK(OnProcessAnalyzeMusicStyle));
}

void LeadSolver::OnProcessAnalyzeMusicStyle(String res) {
	LeadData& ld = *p.lead_data;
	LeadOpportunity& opp = ld.opportunities[batch];
	
	Vector<String> lines = Split(res, "\n");
	
	opp.music_styles.Clear();
	for(int i = 0; i < lines.GetCount(); i++) {
		String& line = lines[i];
		if (line.IsEmpty()) continue;
		
		int a = line.Find(".");
		bool no_beginning = (i == 0 && (a < 0 || a >= 4));
		if (a >= 0 && a < 4)
			line = line.Mid(a+1);
		line = TrimBoth(line);
		
		if (line.IsEmpty()) continue;
		int chr = line[0];
		if (no_beginning && chr >= 'a' && chr <= 'z' && line.Find("he lyrics is about") < 0)
			line = "The lyrics is about " + line;
		
		a = line.Find("** - ");
		if (a >= 0)
			line = line.Mid(a+5);
		
		opp.music_styles << line;
	}
	
	
	NextBatch();
	SetWaiting(0);
}

void LeadSolver::ProcessTemplateTitleAndText() {
	LeadData& ld = *p.lead_data;
	if (batch >= ld.opportunities.GetCount()) {
		NextPhase();
		return;
	}
	LeadOpportunity& opp = ld.opportunities[batch];
	if (SkipLowScoreOpportunity() ||
		opp.weighted_rank >= (double)max_rank||
		opp.average_payout_estimation <= 0.1) {
		NextBatch();
		return;
	}
	
	ProcessAnalyzeFn(7, THISBACK(OnProcessTemplateTitleAndText));
}

void LeadSolver::OnProcessTemplateTitleAndText(String res) {
	LeadData& ld = *p.lead_data;
	LeadDataTemplate& ldt = *p.lead_tmpl;
	int lng = LNG_ENGLISH;// mdb.GetLanguageIndex();
	LOG("LeadSolver::OnProcessTemplateTitleAndText: warning: TODO: int lng = LNG_ENGLISH");
	LeadOpportunity& opp = ld.opportunities[batch];
	
	/*response example:
		New Music Opportunity Within Specific Genres"
		- Description (multiline):
		
		Are you a talented musician looking for new opportunities? ...
	*/
	
	String title, txt;
	int a = res.Find("\"");
	if (a >= 0) {
		title = TrimBoth(res.Left(a));
	}
	a = res.Find("Description");
	if (a >= 0) {
		a = res.Find("\n", a);
		if (a >= 0) {
			txt = TrimBoth(res.Mid(a+1));
		}
	}
	
	if (title.GetCount() && txt.GetCount()) {
		CombineHash ch;
		ch.Do(title).Do(txt);
		hash_t h = ch;
		bool found = false;
		for (const LeadTemplate& lt : ldt.templates) {
			if (lt.hash == h) {
				found = true;
				break;
			}
		}
		
		if (!found) {
			LeadTemplate& t = ldt.templates.Add();
			t.hash = h;
			t.title = title;
			t.text = txt;
			t.orig_lead_idx = batch;
			t.orig_lead_lng = lng;
			t.submission_price = opp.min_entry_price_cents * 0.01;
		}
	}
	
	NextBatch();
	SetWaiting(0);
}

void LeadSolver::ProcessTemplateAnalyze() {
	LeadDataTemplate& ldt = *p.lead_tmpl;
	if (batch >= ldt.templates.GetCount()) {
		NextPhase();
		return;
	}
	LeadTemplate& lt = ldt.templates[batch];
	if (skip_ready && !lt.author_classes.IsEmpty() &&
		!lt.author_specialities.IsEmpty() &&
		!lt.profit_reasons.IsEmpty() &&
		!lt.organizational_reasons.IsEmpty()) {
		NextBatch();
		return;
	}
	
	ProcessAnalyzeFn(8, THISBACK(OnProcessTemplateAnalyze));
}

void LeadSolver::OnProcessTemplateAnalyze(String res) {
	LeadDataTemplate& ldt = *p.lead_tmpl;
	LeadTemplate& lt = ldt.templates[batch];
	
	/*
	- Speciality of the listing's author in short (music genre speciality, clients speciality): The listing author specializes in sourcing and selecting songs for film soundtracks and has a particular interest in Country, Folk, and Bluegrass genres. They also have a keen understanding of the themes and musical cues most suitable for this specific project.
	- Class of the listing's author (e.g. publisher / A&R / licensing agent etc. ) with 1-3 words: Music Licensing Agent
	- Profit reasons for the author of this listing:
	1. To earn a percentage of the profits from the eventual soundtrack sales.
	2. To establish relationships with talented artists and potentially work with them on future projects.
	3. To showcase their expertise in song selection and music curation within the film industry.
	- Positive organizational reasons for the author of this listing:
	1. To support and promote independent artists by featuring their music in a major film release.
	2. To create a diverse and well-curated soundtrack that enhances the overall viewing experience.
	3. To build a strong relationship with the film producers and establish a reputation for providing quality music choices.
	*/
	int a, b;
	String speciality, classes, profits, orgs;
	
	a = res.Find("- Speciality of the listing's author in short");
	if (a >= 0) {
		a = res.Find(":", a);
		if (a >= 0) {
			a++;
			b = res.Find("\n", a);
			if (b >= 0)
				speciality = TrimBoth(res.Mid(a, b-a));
		}
	}
	
	a = res.Find("- Class of the listing");
	if (a >= 0) {
		a = res.Find(":", a);
		if (a >= 0) {
			a++;
			b = res.Find("\n", a);
			if (b >= 0)
				classes = TrimBoth(res.Mid(a, b-a));
		}
	}
	
	a = res.Find("- Profit reasons");
	if (a >= 0) {
		a = res.Find(":", a);
		if (a >= 0) {
			a++;
			b = res.Find("- Positive organi", a);
			if (b >= 0)
				profits = TrimBoth(res.Mid(a, b-a));
		}
	}
	
	a = res.Find("- Positive organizational");
	if (a >= 0) {
		a = res.Find(":", a);
		if (a >= 0) {
			a++;
			b = res.GetCount();
			if (b >= 0)
				orgs = TrimBoth(res.Mid(a, b-a));
		}
	}
	
	if (speciality.GetCount() && classes.GetCount() && profits.GetCount() && orgs.GetCount()) {
		lt.author_specialities.Clear();
		lt.author_classes.Clear();
		lt.profit_reasons.Clear();
		lt.organizational_reasons.Clear();
		
		lt.author_specialities.FindAdd(ldt.author_specialities.FindAdd(speciality));
		lt.author_classes.FindAdd(ldt.author_classes.FindAdd(classes));
		
		RemoveEmptyLines2(profits);
		RemoveEmptyLines2(orgs);
		Vector<String> profit_reasons = Split(profits, "\n");
		Vector<String> organizational_reasons = Split(orgs, "\n");
		for (String& s : profit_reasons)
			lt.profit_reasons.FindAdd(ldt.profit_reasons.FindAdd(s));
		for (String& s : organizational_reasons)
			lt.organizational_reasons.FindAdd(ldt.organizational_reasons.FindAdd(s));
	}
	
	NextBatch();
	SetWaiting(0);
}


INITIALIZER_COMPONENT_CTRL(LeadData, LeadSourceCtrl)

END_UPP_NAMESPACE
