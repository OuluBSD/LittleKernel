#include "Video.h"


NAMESPACE_UPP


ProtoVideoTask1Ctrl::ProtoVideoTask1Ctrl() {
	Add(pages.SizePos());
	
	CtrlLayout(input);
	CtrlLayout(sora_preset);
	CtrlLayout(slideshow_prompts);
	CtrlLayout(descriptions);
	CtrlLayout(ad_message);
	CtrlLayout(cover_image);
	CtrlLayout(english_lyrics);
	CtrlLayout(video_lyrics);
	
	pages.Add(input, "Input");
	pages.Add(sora_preset, "Sora preset");
	pages.Add(slideshow_prompts, "Slideshow prompts");
	pages.Add(descriptions, "Descriptions");
	pages.Add(ad_message, "Ad messages");
	pages.Add(cover_image, "Cover image");
	pages.Add(english_lyrics, "English lyrics");
	pages.Add(video_lyrics, "Video lyrics");
	
	input.artist.WhenAction = THISBACK(OnChange);
	input.title.WhenAction = THISBACK(OnChange);
	input.authors_arrangement.WhenAction = THISBACK(OnChange);
	input.authors_composition.WhenAction = THISBACK(OnChange);
	input.authors_lyrics.WhenAction = THISBACK(OnChange);
	input.extra.WhenAction = THISBACK(OnChange);
	input.lyrics.WhenAction = THISBACK(OnChange);
	input.musicstyle.WhenAction = THISBACK(OnChange);
	input.num.WhenAction = THISBACK(OnChange);
	input.original_arrangement.WhenAction = THISBACK(OnChange);
	input.original_composition.WhenAction = THISBACK(OnChange);
	input.original_lyrics.WhenAction = THISBACK(OnChange);
	input.selfmade_arrangement.WhenAction = THISBACK(OnChange);
	input.selfmade_composition.WhenAction = THISBACK(OnChange);
	input.selfmade_lyrics.WhenAction = THISBACK(OnChange);
	input.videoidea.WhenAction = THISBACK(OnChange);
	input.videolyrics.WhenAction = THISBACK(OnChange);
	input.whenmade_arrangement.WhenAction = THISBACK(OnChange);
	input.whenmade_composition.WhenAction = THISBACK(OnChange);
	input.whenmade_lyrics.WhenAction = THISBACK(OnChange);
	input.whenmade_lyrics_today.WhenAction = THISBACK(OnChange);
	
	sora_preset.refresh.WhenAction = THISBACK(DoSora);
	slideshow_prompts.refresh.WhenAction = THISBACK(DoSlideshowPrompts);
	descriptions.refresh.WhenAction = THISBACK(DoDescriptions);
	ad_message.refresh.WhenAction = THISBACK(DoAdMessages);
	cover_image.refresh.WhenAction = THISBACK(DoCoverImage);
	english_lyrics.refresh.WhenAction = THISBACK(DoEnglishLyrics);
	video_lyrics.refresh.WhenAction = THISBACK(DoVideoLyrics);
	video_lyrics.browse.WhenAction = [this]{
		String dir = SelectDirectory();
		if (DirectoryExists(dir)) {
			ProtoVideoTask1& comp = this->GetExt<ProtoVideoTask1>();
			if (!comp.val.value.Is<ValueMap>())
				comp.val.value = ValueMap();
			ValueMap map = comp.val.value;
			map.Set("videolyrics_path", dir);
			video_lyrics.path.SetData(dir);
			comp.val.value = map;
		}
	};
	video_lyrics.path.WhenAction = [this] {
		String dir = video_lyrics.path.GetData();
		ProtoVideoTask1& comp = this->GetExt<ProtoVideoTask1>();
		if (!comp.val.value.Is<ValueMap>())
			comp.val.value = ValueMap();
		ValueMap map = comp.val.value;
		map.Set("videolyrics_path", dir);
		video_lyrics.path.SetData(dir);
		comp.val.value = map;
	};
	
	for(int i = 0; i < Font::GetFaceCount(); i++) {
		String n = Font::GetFaceName(i);
		Font fnt(i, 11);
		video_lyrics.fontlist.Add(AttrText(n).SetFont(fnt));
	}
}

ProtoVideoTask1Ctrl::~ProtoVideoTask1Ctrl() {
	Save();
}

void ProtoVideoTask1Ctrl::Initialize(Value args) {
	// ??
}

void ProtoVideoTask1Ctrl::OnChange() {
	tc.Set(500, THISBACK(Save));
}

void ProtoVideoTask1Ctrl::Load() {
	ProtoVideoTask1& comp = this->GetExt<ProtoVideoTask1>();
	
	if (!comp.val.value.Is<ValueMap>())
		comp.val.value = ValueMap();

	ValueMap map = comp.val.value;
	#define LOAD(x) input.x.SetData(map.Get(#x, Value()));
	LOAD(artist)
	LOAD(title)
	LOAD(authors_arrangement)
	LOAD(authors_composition)
	LOAD(authors_lyrics)
	LOAD(extra)
	LOAD(lyrics)
	LOAD(musicstyle)
	LOAD(num)
	LOAD(original_arrangement)
	LOAD(original_composition)
	LOAD(original_lyrics)
	LOAD(selfmade_arrangement)
	LOAD(selfmade_composition)
	LOAD(selfmade_lyrics)
	LOAD(videoidea)
	LOAD(videolyrics)
	LOAD(whenmade_arrangement)
	LOAD(whenmade_composition)
	LOAD(whenmade_lyrics)
	LOAD(whenmade_lyrics_today)
	#undef LOAD
	
	sora_preset.txt.SetData(map.Get("sora_preset", Value()));
	slideshow_prompts.txt.SetData(map.Get("slideshow_prompts", Value()));
	descriptions.txt.SetData(map.Get("descriptions", Value()));
	ad_message.txt.SetData(map.Get("ad_message", Value()));
	cover_image.txt.SetData(map.Get("cover_image", Value()));
	english_lyrics.txt.SetData(map.Get("english_lyrics", Value()));
	video_lyrics.path.SetData(map.Get("videolyrics_path", Value()));
	
	// Load font
	String videolyrics_fontname = map.Get("videolyrics_fontname", Value());
	if (videolyrics_fontname.IsEmpty())
		videolyrics_fontname = SansSerif().GetFaceName();
	int videolyrics_fontidx = 0;
	for(int i = 0; i < Font::GetFaceCount(); i++) {
		if (Font::GetFaceName(i) == videolyrics_fontname) {
			videolyrics_fontidx = i;
			break;
		}
	}
	if (videolyrics_fontidx >= 0 && videolyrics_fontidx < video_lyrics.fontlist.GetCount())
		video_lyrics.fontlist.SetIndex(videolyrics_fontidx);
	video_lyrics.height.SetData(map.Get("videolyrics_fontheight", 36));
	
	int bordersize = map.Get("videolyrics_bordersize", 4);
	video_lyrics.bordersize.SetData(bordersize);
	
	Color foreground = map.Get("videolyrics_foreground", White());
	Color background = map.Get("videolyrics_background", Black());
	video_lyrics.foreground.SetData(foreground);
	video_lyrics.background.SetData(background);
	
}

void ProtoVideoTask1Ctrl::Save() {
	ProtoVideoTask1& comp = this->GetExt<ProtoVideoTask1>();
	
	if (!comp.val.value.Is<ValueMap>())
		comp.val.value = ValueMap();
	ValueMap map = comp.val.value;
	#define SAVE(x) map.Set(#x, input.x.GetData());
	SAVE(artist)
	SAVE(title)
	SAVE(authors_arrangement)
	SAVE(authors_composition)
	SAVE(authors_lyrics)
	SAVE(extra)
	SAVE(lyrics)
	SAVE(musicstyle)
	SAVE(num)
	SAVE(original_arrangement)
	SAVE(original_composition)
	SAVE(original_lyrics)
	SAVE(selfmade_arrangement)
	SAVE(selfmade_composition)
	SAVE(selfmade_lyrics)
	SAVE(videoidea)
	SAVE(videolyrics)
	SAVE(whenmade_arrangement)
	SAVE(whenmade_composition)
	SAVE(whenmade_lyrics)
	SAVE(whenmade_lyrics_today)
	#undef SAVE
	
	map.Set("sora_preset", sora_preset.txt.GetData());
	map.Set("slideshow_prompts", slideshow_prompts.txt.GetData());
	map.Set("descriptions", descriptions.txt.GetData());
	map.Set("ad_message", ad_message.txt.GetData());
	map.Set("cover_image", cover_image.txt.GetData());
	map.Set("english_lyrics", english_lyrics.txt.GetData());
	
	int font_idx = video_lyrics.fontlist.GetIndex();
	map.Set("videolyrics_fontname", Font::GetFaceName(font_idx));
	map.Set("videolyrics_fontheight", video_lyrics.height.GetData());
	map.Set("videolyrics_bordersize", video_lyrics.bordersize.GetData());
	map.Set("videolyrics_foreground", video_lyrics.foreground.GetData());
	map.Set("videolyrics_background", video_lyrics.background.GetData());
	
	
	comp.val.value = map;
}

void ProtoVideoTask1Ctrl::Data() {
	Load();
}

void ProtoVideoTask1Ctrl::ToolMenu(Bar& bar) {
	
}

void ProtoVideoTask1Ctrl::UpdateData() {
	
}

void ProtoVideoTask1Ctrl::DoSora() {
	TaskMgr& m = AiTaskManager();
	
	if (flag.IsRunning()) {
		PromptOK("Task already running");
		return;
	}
	
	Save();
	ProtoVideoTask1& comp = this->GetExt<ProtoVideoTask1>();
	ValueMap map = comp.val.value;
	
	flag.Start();
	
	Vector<String> keys;
	keys << /*"artist" <<*/ "title" << "extra" << "musicstyle" << "lyrics" << "videoidea";
	
	String input;
	for (String key : keys)
		if (map.Find(key) >= 0)
			input += (String)map.Get(key, "") + "\n\n";
	
	if (input.IsEmpty()) {
		PromptOK("Error: empty output");
		flag.Stop();
		return;
	}
	
	input += "\n\nAdd a mention to the preset that people's ethnicity is \"Finnish\" and that women have heavy makeup\n";
	
	TaskArgs args;
	args.fn = FN_SORA_PRESETS;
	args.params("input") = input;
	
	Ptr<Ctrl> c = this;
	m.GetJson(args, [this,c](String s) {
		if (c) c->PostCallback([this,s,c]{
			if (!c) return;
			ProtoVideoTask1& comp = this->GetExt<ProtoVideoTask1>();
			ValueMap map = comp.val.value;
			map.Set("sora_preset", s);
			comp.val.value = map;
			
			sora_preset.txt.SetData(s);
			flag.SetStopped();
		});
	}, "Sora preset");
}

void ProtoVideoTask1Ctrl::DoSlideshowPrompts() {
	TaskMgr& m = AiTaskManager();
	
	if (flag.IsRunning()) {
		PromptOK("Task already running");
		return;
	}
	
	Save();
	ProtoVideoTask1& comp = this->GetExt<ProtoVideoTask1>();
	ValueMap map = comp.val.value;
	
	flag.Start();
	
	Vector<String> keys;
	keys << "sora_preset" /*<< "artist" <<*/ "title" << "extra" << "musicstyle" << "lyrics" << "videoidea";
	
	String input;
	for (String key : keys)
		if (map.Find(key) >= 0)
			input += (String)map.Get(key, "") + "\n\n";
	
	if (input.IsEmpty()) {
		PromptOK("Error: empty output");
		flag.Stop();
		return;
	}
	
	TaskArgs args;
	args.fn = FN_SLIDESHOW_PROMPTS;
	args.params("input") = input;
	
	Ptr<Ctrl> c = this;
	m.GetJson(args, [this,c](String s) {
		if (c) c->PostCallback([this,s,c]{
			if (!c) return;
			ProtoVideoTask1& comp = this->GetExt<ProtoVideoTask1>();
			ValueMap map = comp.val.value;
			map.Set("slideshow_prompts", s);
			comp.val.value = map;
			
			slideshow_prompts.txt.SetData(s);
			flag.SetStopped();
		});
	}, "Sora preset");
}

void ProtoVideoTask1Ctrl::DoNews(int i) {
	TaskMgr& m = AiTaskManager();
	
	if (flag.IsRunning()) {
		PromptOK("Task already running");
		return;
	}
	
	Save();
	ProtoVideoTask1& comp = this->GetExt<ProtoVideoTask1>();
	ValueMap map = comp.val.value;
	
	flag.Start();
	
	Vector<String> keys;
	String input;
	
	if (i == 3) {
		keys << "lyrics";
	}
	else {
		keys << "artist" << "extra" << "musicstyle" << "lyrics";
		
		
		int year = map.Get("whenmade_composition", 0);
		int born = 1989;
		int age = max(0, min(100, year - born));
		
		input += "I'm releasing my old songs one a day.\n";
		input += Format("This is the song number %d.", map.Get("num", Value(0)));
		input += Format("I'm releasing a composition from %d, when I was %d years old.\n", year, age);
		
		if (map.Get("whenmade_lyrics_today", false))
			input += "I made the lyrics today.\n";
		
		if (map.Get("original_composition", false))
			input += "The composition of the song is made by hand without any use of AI.\n";
		
		if (map.Get("original_lyrics", false))
			input += "The lyrics of the song is made by hand without any use of AI.\n";
		
		if (map.Get("original_arrangement", false))
			input += "The arrangement of the song is made by hand without any use of AI.\n";
		
		input += "The video is generated by Sora AI.\n";
		input += "I edited the video myself by hand.\n";
		input += "\n\n";
		
	}
	input += "Title: \"" + map.Get("title", Value()).ToString() + "\"\n\n";
	
	for (String key : keys)
		if (map.Find(key) >= 0)
			input += (String)map.Get(key, "") + "\n\n";
	
	if (input.IsEmpty()) {
		PromptOK("Error: empty output");
		flag.Stop();
		return;
	}
	
	
	TaskArgs args;
	switch (i) {
		case 0: args.fn = FN_VIDEO_WEBSITE_DESCRIPTIONS; break;
		case 1: args.fn = FN_VIDEO_ADS; break;
		case 2: args.fn = FN_VIDEO_COVER_IMAGE;
				input += "\n\nLocation is Finland and people's ethnicity is Finnish. Women have heavy makeup\n"; break;
		case 3: args.fn = FN_ENGLISH_LYRICS; break;
		default: break;
	}
	args.params("input") = input;
	
	Ptr<Ctrl> c = this;
	m.GetJson(args, [this,c,i](String s) {
		if (c) c->PostCallback([this,s,c,i]{
			if (!c) return;
			ProtoVideoTask1& comp = this->GetExt<ProtoVideoTask1>();
			ValueMap map = comp.val.value;
			if (i == 0) {
				map.Set("descriptions", s);
				descriptions.txt.SetData(s);
			}
			else if (i == 1) {
				map.Set("ad_message", s);
				ad_message.txt.SetData(s);
			}
			else if (i == 2) {
				map.Set("cover_image", s);
				cover_image.txt.SetData(s);
			}
			else if (i == 3) {
				map.Set("english_lyrics", s);
				english_lyrics.txt.SetData(s);
			}
			comp.val.value = map;
			
			flag.SetStopped();
		});
	}, "Sora preset");
}

void ProtoVideoTask1Ctrl::DoDescriptions() {
	DoNews(0);
}

void ProtoVideoTask1Ctrl::DoAdMessages() {
	DoNews(1);
}

void ProtoVideoTask1Ctrl::DoCoverImage() {
	DoNews(2);
}

void ProtoVideoTask1Ctrl::DoEnglishLyrics() {
	DoNews(3);
}

void ProtoVideoTask1Ctrl::DoVideoLyrics() {
	ProtoVideoTask1& comp = this->GetExt<ProtoVideoTask1>();
	if (!comp.val.value.Is<ValueMap>())
		return;
	ValueMap map = comp.val.value;
	String dir = map.Get("videolyrics_path", "");
	
	String s = map.Get("videolyrics","");
	s.Replace("\r", "");
	Vector<String> lines = Split(s, "\n");
	for(int i = 0; i < lines.GetCount(); i++) {
		String& l = lines[i];
		l = TrimBoth(l);
		if (l.IsEmpty() || l.Left(1) == "[")
			lines.Remove(i--);
	}
	int fnt_idx = video_lyrics.fontlist.GetIndex();
	int fnt_h = video_lyrics.height.GetData();
	Font fnt(fnt_idx, fnt_h);
	
	int bordersize = video_lyrics.bordersize.GetData();
	Color foreground = video_lyrics.foreground.GetData();
	Color background = video_lyrics.background.GetData();
	
	int off = 5 + bordersize;
	
	PNGEncoder enc;
	
	String out_text;
	int idx = 0;
	for (String& l : lines) {
		String filename = Format("txt-%03d.png", idx++);
		Size sz = GetTextSize(l, fnt);
		if (sz.IsEmpty())
			continue;
		sz.cx += off * 2;
		sz.cy += off * 2;
		
		String filepath = AppendFileName(dir, filename);
		out_text += "\"" + l + "\": " + filepath + "\n";
		
		ImageDraw id(sz);
		id.Alpha().DrawRect(sz, Black());
		
		for(int i = 0; i < 36; i++) {
			double angle = i / 36.0 * M_2PI;
			int yoff = sin(angle) * bordersize;
			int xoff = cos(angle) * bordersize;
			id.DrawText(off+xoff,off+yoff, l, fnt, background);
			id.Alpha().DrawText(off+xoff,off+yoff, l, fnt, White());
		}
		id.DrawText(off,off, l, fnt, foreground);
		id.Alpha().DrawText(off,off, l, fnt, White());
		
		Image img = id;
		enc.Bpp(32);
		enc.SaveFile(filepath, img);
	}
	video_lyrics.txt.SetData(out_text);
}



INITIALIZER_COMPONENT_CTRL(ProtoVideoTask1, ProtoVideoTask1Ctrl)


END_UPP_NAMESPACE
