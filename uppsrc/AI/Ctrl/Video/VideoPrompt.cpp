#include "Video.h"


NAMESPACE_UPP


VideoPromptMakerCtrl::VideoPromptMakerCtrl() {
	Add(vsplit.SizePos());
	
	vsplit.Vert() <<	storyboard_parts <<
						storyboard_prompt_split <<
						text_storyboard_parts;
	vsplit.SetPos(2500,0);
	vsplit.SetPos(5000,1);
	
	storyboard_parts.AddColumn(t_(""));
	storyboard_parts.AddColumn(t_(""));
	
	storyboard_prompt_split.Horz() << storyboard_prompt_keys << storyboard_prompt_values;
	storyboard_prompt_keys.AddColumn(t_(""));
	storyboard_prompt_keys.WhenCursor << THISBACK(DataPrompt);
	storyboard_prompt_values.AddColumn(t_(""));
	
	text_storyboard_parts.AddColumn(t_(""));
	text_storyboard_parts.AddColumn(t_(""));
	text_storyboard_parts.AddColumn(t_("Search"));
	text_storyboard_parts.AddColumn(t_("Prompt"));
	text_storyboard_parts.ColumnWidths("2 1 2 4");
	
	
	
}

void VideoPromptMakerCtrl::Data() {
	DatasetPtrs p; GetDataset(p);
	if (!p.song)
		return;
	
	// Storyboard parts
	for(int i = 0; i < p.song->storyboard_parts.GetCount(); i++) {
		String key = p.song->storyboard_parts.GetKey(i);
		String value = p.song->storyboard_parts[i];
		storyboard_parts.Set(i, 0, key);
		storyboard_parts.Set(i, 1, value);
	}
	INHIBIT_CURSOR_(storyboard_parts, a);
	storyboard_parts.SetCount(p.song->storyboard_parts.GetCount());
	
	// Storyboard prompts
	for(int i = 0; i < p.song->storyboard_prompts.GetCount(); i++) {
		storyboard_prompt_keys.Set(i, 0, p.song->storyboard_prompts.GetKey(i));
	}
	INHIBIT_CURSOR_(storyboard_prompt_keys, b);
	storyboard_prompt_keys.SetCount(p.song->storyboard_prompts.GetCount());
	if (!storyboard_prompt_keys.IsCursor() && storyboard_prompt_keys.GetCount())
		storyboard_prompt_keys.SetCursor(0);
	
	// Text to storyboard parts
	for(int i = 0; i < p.song->text_storyboard_parts.GetCount(); i++) {
		int part_i = p.song->text_storyboard_parts[i];
		const String& part = p.song->storyboard_parts.GetKey(part_i);
		text_storyboard_parts.Set(i, 0, p.song->text_storyboard_parts.GetKey(i));
		text_storyboard_parts.Set(i, 1, part);
		
		if (i < p.song->text_storyboard_searches.GetCount())
			text_storyboard_parts.Set(i, 2, p.song->text_storyboard_searches[i]);
		else
			text_storyboard_parts.Set(i, 2, "");
		
		if (i < p.song->text_storyboard_prompts.GetCount())
			text_storyboard_parts.Set(i, 3, p.song->text_storyboard_prompts[i]);
		else
			text_storyboard_parts.Set(i, 3, "");
	}
	INHIBIT_CURSOR_(text_storyboard_parts, c);
	text_storyboard_parts.SetCount(p.song->text_storyboard_parts.GetCount());
	
	
	DataPrompt();
}

void VideoPromptMakerCtrl::DataPrompt() {
	DatasetPtrs p; GetDataset(p);
	if (!p.song || !storyboard_prompt_keys.IsCursor()) {
		storyboard_prompt_values.Clear();
		return;
	}
	
	int cur = storyboard_prompt_keys.GetCursor();
	const auto& values = p.song->storyboard_prompts[cur];
	for(int j = 0; j < values.GetCount(); j++) {
		storyboard_prompt_values.Set(j, 0, values[j]);
	}
	INHIBIT_CURSOR(storyboard_prompt_values);
	storyboard_prompt_values.SetCount(values.GetCount());
}

void VideoPromptMakerCtrl::OnValueChange() {
	
}

void VideoPromptMakerCtrl::ToolMenu(Bar& bar) {
	bar.Add(t_("Update"), MetaImgs::BlueRing(), THISBACK(Data)).Key(K_CTRL_Q);
	bar.Add(t_("Copy prompt"), MetaImgs::BlueRing(), THISBACK1(Do, 1)).Key(K_CTRL_C);
	bar.Separator();
	bar.Add(t_("Make video prompts"), MetaImgs::RedRing(), THISBACK1(Do, 0)).Key(K_F5);
}

void VideoPromptMakerCtrl::Do(int fn) {
	DatasetPtrs p; GetDataset(p);
	if (!p.song) return;
	
	TODO
	#if 0
	if (fn == 0) {
		VideoSolver& tm = VideoSolver::Get(*p.song);
		tm.Start();
	}
	if (fn == 1) {
		if (!text_storyboard_parts.IsCursor())
			return;
		String text = text_storyboard_parts.Get(2);
		WriteClipboardText(text);
	}
	#endif
}

VideoSolver::VideoSolver() {
	
}

int VideoSolver::GetPhaseCount() const {
	return PHASE_COUNT;
}

void VideoSolver::DoPhase() {
	ASSERT(snap);
	
	if (phase >= PHASE_COUNT)
		return;
	
	TODO
	#if 0
	if (phase == PHASE_MAKE_STORYBOARD) {
		int script_i = snap->entity->FindScript(comp->scripts_file_title);
		if (script_i < 0) {
			NextPhase();
			LOG("error: no script found for " << comp->scripts_file_title);
			SetNotRunning();
			return;
		}
		
		if (skip_ready && comp->storyboard_parts.GetCount()) {
			NextPhase();
			return;
		}
		
		Script& script = snap->entity->scripts[script_i];
		
		VideoSolverArgs args;
		args.fn = 0;
		args.text = script.GetText(appmode);
		
		SetWaiting(1);
		AiTaskManager();
		m.GetVideoSolver(args, [this](String res) {
			comp->storyboard_parts.Clear();
			
			if (res.Left(1) != "-")
				res = "- " + res;
			
			RemoveEmptyLines3(res);
			Vector<String> lines = Split(res, "\n");
			int part_i = 0;
			for (String& l : lines) {
				int a = l.Find(":");
				if (a < 0)
					comp->storyboard_parts.Add(IntStr(part_i++), l);
				else {
					String key = l.Left(a);
					String value = TrimBoth(l.Mid(a+1));
					if (value == "None" || value == "none")
						continue;
					
					comp->storyboard_parts.Add(key, value);
				}
			}
			
			SetWaiting(0);
			NextPhase();
		});
	}
	else if (phase == PHASE_STORYBOARD_TO_PARTS_AND_DALLE_PROMPTS) {
		int script_i = snap->entity->FindScript(comp->scripts_file_title);
		ASSERT(script_i >= 0);
		Script& script = snap->entity->scripts[script_i];
		
		if (batch >= comp->storyboard_parts.GetCount()) {
			NextPhase();
			return;
		}
		
		String key = comp->storyboard_parts.GetKey(batch);
		auto& prompts = comp->storyboard_prompts.GetAdd(key);
		if (skip_ready && prompts.GetCount()) {
			NextBatch();
			return;
		}
		
		VideoSolverArgs args;
		args.fn = 1;
		args.text = script.GetText(appmode);
		args.parts.Add(key);
		
		SetWaiting(1);
		AiTaskManager();
		m.GetVideoSolver(args, [this](String res) {
			String key = comp->storyboard_parts.GetKey(batch);
			auto& prompts = comp->storyboard_prompts.GetAdd(key);
			prompts.Clear();
			
			if (res.Left(1) != "-")
				res = "- " + res;
			
			RemoveEmptyLines3(res);
			Vector<String> lines = Split(res, "\n");
			for (String& l : lines) {
				l = TrimBoth(l);
				RemoveQuotes(l);
				if (l.Find("List of long descriptions") == 0)
					break;
				
				prompts.Add(l);
			}
			
			SetWaiting(0);
			NextBatch();
		});
	}
	else if (phase == PHASE_TEXT_TO_PARTS) {
		int script_i = snap->entity->FindScript(comp->scripts_file_title);
		ASSERT(script_i >= 0);
		Script& script = snap->entity->scripts[script_i];
		
		VideoSolverArgs args;
		args.fn = 2;
		args.parts <<= comp->storyboard_parts;
		
		String txt = script.GetText(appmode);
		txt.Replace("\r", "");
		tmp_lines = Split(txt, "\n");
		for(int i = 0; i < tmp_lines.GetCount(); i++) {
			String& l = tmp_lines[i];
			if (l.Left(1) == "[" || l.IsEmpty())
				tmp_lines.Remove(i--);
		}
		args.text = Join(tmp_lines, "\n");
		
		SetWaiting(1);
		AiTaskManager();
		m.GetVideoSolver(args, [this](String res) {
			comp->text_storyboard_parts.Clear();
			Vector<String> lines = Split(res, "\n");
			for(int i = 0; i < lines.GetCount(); i++) {
				String& l = lines[i];
				l = TrimBoth(l);
				RemoveQuotes(l);
				
				int la = l.Find("line #");
				int pa = l.Find("part #");
				if (pa < 0)
					continue;
				if (la < 0)
					la = -6;
				
				int line_i = ScanInt(l.Mid(la+6));
				int part_i = ScanInt(l.Mid(pa+6));
				if (line_i < 0 || line_i >= tmp_lines.GetCount())
					continue;
				
				String& tl = tmp_lines[line_i];
				comp->text_storyboard_parts.Add(tl, part_i);
			}
			
			SetWaiting(0);
			NextPhase();
		});
	}
	else if (phase == PHASE_IMAGE_SEARCH_PROMPT) {
		if (batch == 0) comp->text_storyboard_searches.Clear();
		
		if (batch >= comp->text_storyboard_parts.GetCount()) {
			NextPhase();
			return;
		}
		if (skip_ready && comp->text_storyboard_searches.GetCount() > batch) {
			NextBatch();
			return;
		}
		
		comp->text_storyboard_searches.SetCount(batch);
		
		String prompts_so_far = Join(comp->text_storyboard_searches, "\n");
		
		const String& line = comp->text_storyboard_parts.GetKey(batch);
		int part_i = comp->text_storyboard_parts[batch];
		String key = comp->storyboard_parts.GetKey(part_i);
		const auto& prompts = comp->storyboard_prompts.GetAdd(key);
		if (prompts.IsEmpty()) {
			NextBatch();
			return;
		}
		
		VideoSolverArgs args;
		args.fn = 6;
		args.text = prompts_so_far;
		args.line = line;
		args.prompts <<= prompts;
		
		
		SetWaiting(1);
		AiTaskManager();
		m.GetVideoSolver(args, [this](String res) {
			res = TrimBoth(res);
			RemoveEmptyLines3(res);
			
			res.Replace("\"", "");
			res.Replace("- ", "");
			
			Vector<String> items = Split(res, "\n");
			String line = Join(items, ", ");
			
			comp->text_storyboard_searches.Add(line);
			
			SetWaiting(0);
			NextBatch();
		});
	}
	else if (phase == PHASE_FILL_STORY_PROMPTS) {
		if (batch >= comp->text_storyboard_parts.GetCount()) {
			NextPhase();
			return;
		}
		if (skip_ready && comp->text_storyboard_prompts.GetCount() > batch) {
			NextBatch();
			return;
		}
		
		comp->text_storyboard_prompts.SetCount(batch);
		
		String prompts_so_far = Join(comp->text_storyboard_prompts, "\n");
		
		const String& line = comp->text_storyboard_parts.GetKey(batch);
		int part_i = comp->text_storyboard_parts[batch];
		String key = comp->storyboard_parts.GetKey(part_i);
		const auto& prompts = comp->storyboard_prompts.GetAdd(key);
		if (prompts.IsEmpty()) {
			NextBatch();
			return;
		}
		
		VideoSolverArgs args;
		args.fn = 3;
		args.text = prompts_so_far;
		args.line = line;
		args.prompts <<= prompts;
		
		
		SetWaiting(1);
		AiTaskManager();
		m.GetVideoSolver(args, [this](String res) {
			res = TrimBoth(res);
			RemoveQuotes(res);
			
			comp->text_storyboard_prompts.Add(res);
			
			SetWaiting(0);
			NextBatch();
		});
	}
	else if (phase == PHASE_GET_IMAGES ||
			 phase == PHASE_GET_SAFE_IMAGES) {
		int c = comp->text_storyboard_prompts.GetCount();
		comp->text_storyboard_prompts_safe.SetCount(c);
		comp->text_storyboard_prompts_runway.SetCount(c);
		
		if (batch >= c) {
			NextPhase();
			return;
		}
		
		auto& hashes = comp->text_storyboard_hashes;
		if (skip_ready &&
			batch < hashes.GetCount() &&
			hashes[batch].GetCount() >= arg_image_count) {
			NextBatch();
			return;
		}
		
		if (batch >= hashes.GetCount())
			hashes.SetCount(batch+1);
		
		Vector<int64>& prompt_hashes = hashes[batch];
		
		prompt_hashes.SetCount(0);
		
		String prompt;
		
		if (comp->text_storyboard_prompts_safe[batch].GetCount() ||
			phase == PHASE_GET_SAFE_IMAGES)
			prompt = comp->text_storyboard_prompts_safe[batch];
		else
			prompt = comp->text_storyboard_prompts[batch];
		
		if (prompt.GetCount() > 1000)
			prompt = prompt.Left(1000);
		
		
		SetWaiting(1);
		AiTaskManager();
		m.CreateImage(prompt, arg_image_count, [this](Array<Image>& imgs) {
			auto& hashes = comp->text_storyboard_hashes;
			Vector<int64>& prompt_hashes = hashes[batch];
			
			prompt_hashes.SetCount(imgs.GetCount(), 0);
			
			for(int i = 0; i < imgs.GetCount(); i++) {
				Image& img = imgs[i];
				hash_t h = img.GetHashValue();
				
				String cache_path = CacheImageFile(h);
				String thumb_path = ThumbnailImageFile(h);
				String full_path = FullImageFile(h);
				RealizeDirectory(GetFileDirectory(cache_path));
				RealizeDirectory(GetFileDirectory(thumb_path));
				RealizeDirectory(GetFileDirectory(full_path));
				
				if (!FileExists(cache_path)) {
					Image small_img = RescaleToFit(img, 1024);
					JPGEncoder enc(98);
					enc.SaveFile(cache_path, small_img);
				}
				
				if (!FileExists(thumb_path)) {
					Image thumb_img = RescaleToFit(img, 128);
					JPGEncoder enc(98);
					enc.SaveFile(thumb_path, thumb_img);
				}
				
				if (!FileExists(full_path)) {
					JPGEncoder enc(100);
					enc.SaveFile(full_path, img);
				}
				
				prompt_hashes[i] = h;
			}
			
			
			SetWaiting(0);
			NextBatch();
		});
	}
	else if (phase == PHASE_SAFE_PROMPTS) {
		
		if (batch >= comp->text_storyboard_prompts.GetCount()) {
			NextPhase();
			return;
		}
		
		auto& hashes = comp->text_storyboard_hashes;
		if (skip_ready &&
			batch < hashes.GetCount() &&
			hashes[batch].GetCount() >= arg_image_count) {
			NextBatch();
			return;
		}
		
		String prompt = comp->text_storyboard_prompts[batch];
		
		VideoSolverArgs args;
		args.fn = 4;
		args.text = comp->text_storyboard_prompts[batch];
		
		
		SetWaiting(1);
		AiTaskManager();
		m.GetVideoSolver(args, [this](String res) {
			res = TrimBoth(res);
			RemoveQuotes(res);
			
			comp->text_storyboard_prompts_safe[batch] = res;
			
			SetWaiting(0);
			NextBatch();
		});
	}
	else if (phase == PHASE_GET_RUNWAY_STORYBOARD) {
		
		if (batch >= comp->text_storyboard_prompts_runway.GetCount()) {
			NextPhase();
			return;
		}
		
		if (skip_ready && comp->text_storyboard_prompts_runway[batch].GetCount()) {
			NextBatch();
			return;
		}
		
		String prompt = comp->text_storyboard_prompts[batch];
		
		VideoSolverArgs args;
		args.fn = 5;
		args.text = comp->text_storyboard_prompts[batch];
		
		
		SetWaiting(1);
		AiTaskManager();
		m.GetVideoSolver(args, [this](String res) {
			res = TrimBoth(res);
			RemoveQuotes(res);
			
			comp->text_storyboard_prompts_runway[batch] = res;
			
			SetWaiting(0);
			NextBatch();
		});
	}
	else TODO
	#endif
}

ArrayMap<hash_t, VideoSolver>& __VideoSolvers() {
	static ArrayMap<hash_t, VideoSolver> map;
	return map;
}

VideoSolver& VideoSolver::Get(Song& c) {
	TODO
	#if 0
	Entity& e = *c.snapshot->entity;
	Profile& p = *e.profile;
	Release& n = *c.snapshot;
	String t = p.owner->name + ": " + e.file_title + ": " + n.file_title + ": " + c.file_title + " (" + IntStr(appmode) + ")";
	hash_t h = t.GetHashValue();
	ArrayMap<hash_t, VideoSolver>& map = __VideoSolvers();
	int i = map.Find(h);
	if (i >= 0)
		return map[i];
	
	VideoSolver& ls = map.Add(h);
	ASSERT(c.snapshot);
	ASSERT(c.snapshot->entity);
	ls.comp = &c;
	ls.snap = c.snapshot;
	ls.entity = c.snapshot->entity;
	ls.appmode = appmode;
	return ls;
	#endif
	return Single<VideoSolver>();
}


INITIALIZER_COMPONENT_CTRL(VideoPromptMaker, VideoPromptMakerCtrl)

END_UPP_NAMESPACE
