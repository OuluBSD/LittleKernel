#include "Biography.h"
#define REF(tab, obj) auto& obj = tab.obj;

NAMESPACE_UPP


void BiographyCtrl::Image_Ctor() {
	REF(image, vsplit);
	REF(image, bsplit);
	REF(image, years);
	REF(image, year);
	REF(image, entries);
	REF(image, img);
	
	tabs.Add(vsplit.SizePos(), "Image");
	
	vsplit.Vert() << years << bsplit;
	vsplit.SetPos(3333);
	
	bsplit.Horz() << entries << year << img;
	bsplit.SetPos(1500,0);
	bsplit.SetPos(5000,1);
	
	CtrlLayout(year);
	
	
	years.AddColumn(t_("Year"));
	years.AddColumn(t_("Age"));
	years.AddColumn(t_("Class"));
	years.AddColumn(t_("Images"));
	years.AddColumn(t_(""));
	years.AddIndex("IDX");
	years.ColumnWidths("1 1 1 1 15");
	years.WhenCursor << THISBACK(Image_DataYear);
	
	
	entries.AddColumn(t_("#"));
	entries.AddColumn(t_("Time"));
	entries.AddColumn(t_("Keywords"));
	entries.AddIndex("IDX");
	entries.ColumnWidths("1 2 2");
	entries.WhenCursor << THISBACK(Image_DataEntry);
	entries.WhenBar << THISBACK(Image_EntryListMenu);
	
	year.time <<= THISBACK(Image_OnValueChange);
	year.native_text <<= THISBACK(Image_OnValueChange);
	year.text <<= THISBACK(Image_OnValueChange);
	year.keywords <<= THISBACK(Image_OnValueChange);
	year.image_text <<= THISBACK(Image_OnValueChange);
	year.image_keywords <<= THISBACK(Image_OnValueChange);
	
}

void BiographyCtrl::Image_Data() {
	
	DatasetPtrs mp; GetDataset(mp);
	if (!mp.owner || !mp.biography || !mp.analysis) {
		for(int i = 0; i < categories.GetCount(); i++) {
			categories.Set(i, 0, 0);
			categories.Set(i, 2, 0);
		}
		return;
	}
	Owner& owner = *mp.owner;
	Biography& biography = GetExt<Biography>();
	
	Index<int> req_cats = mp.analysis->GetRequiredCategories();
	for(int i = 0; i < categories.GetCount(); i++) {
		int cat_i = categories.Get(i, "IDX");
		bool req = req_cats.Find(cat_i) >= 0;
		categories.Set(i, 0, req ? "X" : "");
		BiographyCategory& bcat = biography.GetAdd(owner, cat_i);
		int c = bcat.GetFilledImagesCount();
		categories.Set(i, 2, c > 0 ? Value(c) : Value());
	}
	DataCategory();
}

void BiographyCtrl::Image_DataCategory() {
	REF(image, vsplit);
	REF(image, bsplit);
	REF(image, years);
	REF(image, year);
	REF(image, entries);
	
	
	DatasetPtrs mp; GetDataset(mp);
	if (!mp.owner || !mp.biography || !categories.IsCursor()) {
		years.Clear();
		return;
	}
	Owner& owner = *mp.owner;
	Biography& biography = GetExt<Biography>();
	int cat_i = categories.Get("IDX");
	BiographyCategory& bcat = biography.GetAdd(owner, cat_i);
	
	Date today = GetSysDate();
	for(int i = 0; i < bcat.years.GetCount(); i++) {
		const BioYear& by = bcat.years[i];
		Date by_date(by.year, today.month, today.day);
		int age = (by_date - owner.born) / 365;
		int cls = age - 7;
		String cls_str;
		if (cls >= 0) {
			int round = cls / 12;
			cls = cls % 12;
			cls_str.Cat('A' + round);
			cls_str += " " + IntStr(1+cls);
		}
		years.Set(i, 0, by.year);
		years.Set(i, 1, age);
		years.Set(i, 2, cls_str);
		int c = by.images.GetCount();
		years.Set(i, 3, c > 0 ? Value(c) : Value());
		years.Set(i, "IDX", i);
	}
	INHIBIT_CURSOR(years);
	years.SetSortColumn(0, false);
	years.SetCount(bcat.years.GetCount());
	if (years.GetCount() && !years.IsCursor())
		years.SetCursor(0);
	
	Image_DataYear();
}

void BiographyCtrl::Image_DataYear() {
	REF(image, vsplit);
	REF(image, bsplit);
	REF(image, years);
	REF(image, year);
	REF(image, entries);
	DatasetPtrs mp; GetDataset(mp);
	if (!mp.owner || !mp.biography || !categories.IsCursor() || !years.IsCursor())
		return;
	
	Owner& owner = *mp.owner;
	Biography& biography = GetExt<Biography>();
	int cat_i = categories.Get("IDX");
	BiographyCategory& bcat = biography.GetAdd(owner, cat_i);
	int year_i = years.Get("IDX");
	if (year_i >= bcat.years.GetCount()) return;
	BioYear& by = bcat.years[year_i];
	
	for(int i = 0; i < by.images.GetCount(); i++) {
		const BioImage& bimg = by.images[i];
		entries.Set(i, 0, i);
		entries.Set(i, "IDX", i);
		entries.Set(i, 1, bimg.time);
		entries.Set(i, 2, bimg.image_keywords);
	}
	INHIBIT_CURSOR(entries);
	entries.SetSortColumn(0, false);
	entries.SetCount(by.images.GetCount());
	if (entries.GetCount() && !entries.IsCursor())
		entries.SetCursor(0);
	
	
	Image_DataEntry();
}

void BiographyCtrl::Image_DataEntry() {
	REF(image, vsplit);
	REF(image, bsplit);
	REF(image, years);
	REF(image, year);
	REF(image, entries);
	
	DatasetPtrs mp; GetDataset(mp);
	if (!mp.owner || !categories.IsCursor() || !years.IsCursor() || !entries.IsCursor())
		return;
	
	Owner& owner = *mp.owner;
	Biography& biography = GetExt<Biography>();
	int cat_i = categories.Get("IDX");
	BiographyCategory& bcat = biography.GetAdd(owner, cat_i);
	int year_i = years.Get("IDX");
	if (year_i >= bcat.years.GetCount()) return;
	BioYear& by = bcat.years[year_i];
	int entry_i = entries.Get("IDX");
	BioImage& bimg = by.images[entry_i];
	
	
	year.time.SetData(bimg.time);
	year.native_text.SetData(bimg.native_text);
	year.text.SetData(bimg.text);
	year.keywords.SetData(bimg.keywords);
	year.image_text.SetData(bimg.image_text);
	year.image_keywords.SetData(bimg.image_keywords);
	
	if (bimg.image_hash) {
		String dir = GetFileDirectory(GetFilePath());
		String cache_dir = ConfigFile("");
		String path = CacheImageFile(cache_dir, bimg.image_hash);
		if (!FileExists(path))
			path = ThumbnailImageFile(dir, bimg.image_hash);
		Image i = StreamRaster::LoadFileAny(path);
		this->image.img.SetImage(i);
	}
	else {
		this->image.img.Clear();
	}
}

void BiographyCtrl::Image_OnCategoryCursor() {
	REF(image, years);
	REF(image, img);
	
	DataCategory();
	for(int i = years.GetCount()-1; i >= 0; i--) {
		int count = years.Get(i, 3);
		if (count > 0) {
			years.SetCursor(i);
			return;
		}
	}
	img.Clear();
}

void BiographyCtrl::Image_OnValueChange() {
	REF(image, vsplit);
	REF(image, bsplit);
	REF(image, years);
	REF(image, year);
	REF(image, entries);
	
	
	DatasetPtrs mp; GetDataset(mp);
	if (!mp.owner || !mp.biography || !categories.IsCursor() || !years.IsCursor() || !entries.IsCursor())
		return;
	TODO
	#if 0
	if (!mp.editable_biography)
		return;
	Owner& owner = *mp.owner;
	Biography& biography = GetExt<Biography>();
	int cat_i = categories.Get("IDX");
	BiographyCategory& bcat = biography.GetAdd(owner, cat_i);
	int year_i = years.Get("IDX");
	if (year_i >= bcat.years.GetCount()) return;
	mp.snap->last_modified = GetSysTime();
	
	BioYear& by = bcat.years[year_i];
	int entry_i = entries.Get("IDX");
	BioImage& bimg = by.images[entry_i];
	
	bimg.time = year.time.GetData();
	bimg.native_text = year.native_text.GetData();
	bimg.text = year.text.GetData();
	bimg.keywords = year.keywords.GetData();
	bimg.image_keywords = year.image_keywords.GetData();
	bimg.image_text = year.image_text.GetData();
	
	entries.Set(1, bimg.time);
	entries.Set(2, bimg.image_keywords);
	#endif
}

void BiographyCtrl::Image_MakeKeywords(int fn) {
	TODO
	#if 0
	TaskMgr& m = AiTaskManager();
	SocialArgs args;
	if (fn == 0)
		args.text = year.text.GetData();
	else
		args.text = year.image_text.GetData();
	m.GetSocial(args, [this,fn](String s) {PostCallback(THISBACK2(OnKeywords, fn, s));});
	#endif
}

void BiographyCtrl::Image_Translate() {
	REF(image, year);
	TaskMgr& m = AiTaskManager();
	
	String src = year.native_text.GetData();
	
	m.Translate("FI-FI", src, "EN-US", [this](String s) {PostCallback(THISBACK1(Image_OnTranslate, s));});
}

void BiographyCtrl::Image_OnTranslate(String s) {
	REF(image, year);
	year.text.SetData(s);
	Image_OnValueChange();
}

void BiographyCtrl::Image_OnKeywords(int fn, String s) {
	REF(image, year);
	RemoveEmptyLines2(s);
	Vector<String> parts = Split(s, "\n");
	s = Join(parts, ", ");
	if (fn == 0)
		year.keywords.SetData(s);
	else
		year.image_keywords.SetData(s);
	Image_OnValueChange();
}

void BiographyCtrl::Image_ToolMenu(Bar& bar) {
	bar.Add(t_("Start"), MetaImgs::RedRing(), THISBACK1(Image_Do, 0)).Key(K_F5);
	bar.Add(t_("Stop"), MetaImgs::RedRing(), THISBACK1(Image_Do, 1)).Key(K_F6);
	bar.Separator();
	bar.Add(t_("Translate"), MetaImgs::BlueRing(), THISBACK(Image_Translate));
	bar.Add(t_("Make keywords"), MetaImgs::BlueRing(), THISBACK1(Image_MakeKeywords, 0));
	bar.Add(t_("Make keywords (image)"), MetaImgs::BlueRing(), THISBACK1(Image_MakeKeywords, 1));
	bar.Separator();
	bar.Add(t_("Paste Image path"), MetaImgs::BlueRing(), THISBACK(Image_PasteImagePath)).Key(K_CTRL_V);
	bar.Separator();
	bar.Add(t_("Analyse image"), MetaImgs::RedRing(), THISBACK(Image_AnalyseImage)).Key(K_F7);
}

void BiographyCtrl::Image_EntryListMenu(Bar& bar) {
	bar.Add(t_("Add Entry"), MetaImgs::BlueRing(), THISBACK(Image_AddEntry)).Key(K_CTRL_T);
	bar.Add(t_("Remove Entry"), MetaImgs::BlueRing(), THISBACK(Image_RemoveEntry)).Key(K_CTRL|K_SHIFT|K_W);
	
}

void BiographyCtrl::Image_Do(int fn) {
	DatasetPtrs mp; GetDataset(mp);
	if (!mp.profile || !mp.release)
		return;
	if (mp.editable_biography) {
		PromptOK(t_("The latest (and editable) revision can't be processed. Select older than latest revision."));
		return;
	}
	
	ImageBiographyProcess& ss = ImageBiographyProcess::Get(*mp.profile, *mp.snap);
	TODO
	#if 0
	if (fn == 0) {
		ss.Start();
	}
	else if (fn == 1) {
		ss.Stop();
	}
	#endif
}

void BiographyCtrl::Image_AddEntry() {
	REF(image, vsplit);
	REF(image, bsplit);
	REF(image, years);
	REF(image, year);
	REF(image, entries);
	
	DatasetPtrs mp; GetDataset(mp);
	if (!mp.owner || !mp.biography || !categories.IsCursor() || !years.IsCursor())
		return;
	Owner& owner = *mp.owner;
	Biography& biography = GetExt<Biography>();
	int cat_i = categories.Get("IDX");
	BiographyCategory& bcat = biography.GetAdd(owner, cat_i);
	int year_i = years.Get("IDX");
	if (year_i >= bcat.years.GetCount()) return;
	BioYear& by = bcat.years[year_i];
	
	by.images.Add();
	Image_DataYear();
	if (entries.GetCount())
		entries.SetCursor(entries.GetCount()-1);
}

void BiographyCtrl::Image_RemoveEntry() {
	REF(image, vsplit);
	REF(image, bsplit);
	REF(image, years);
	REF(image, year);
	REF(image, entries);
	
	DatasetPtrs mp; GetDataset(mp);
	if (!mp.owner || !mp.biography || !categories.IsCursor() || !years.IsCursor() || !entries.IsCursor())
		return;
	Owner& owner = *mp.owner;
	Biography& biography = GetExt<Biography>();
	int cat_i = categories.Get("IDX");
	BiographyCategory& bcat = biography.GetAdd(owner, cat_i);
	int year_i = years.Get("IDX");
	if (year_i >= bcat.years.GetCount()) return;
	BioYear& by = bcat.years[year_i];
	int entry_i = entries.Get("IDX");
	if (entry_i >= 0 && entry_i < by.images.GetCount())
		by.images.Remove(entry_i);
	Image_DataYear();
}

void BiographyCtrl::Image_PasteImagePath() {
	Image img = ReadClipboardImage();
	if (!img.IsEmpty()) {
		Image_SetCurrentImage(img);
	}
	else {
		String path = ReadClipboardText();
		if (path.Left(7) == "file://") {
			path = path.Mid(7);
			img = StreamRaster::LoadFileAny(path);
			if (!img.IsEmpty())
				Image_SetCurrentImage(img);
		}
	}
}

void BiographyCtrl::Image_SetCurrentImage(Image img) {
	REF(image, vsplit);
	REF(image, bsplit);
	REF(image, years);
	REF(image, year);
	REF(image, entries);
	
	this->image.img.SetImage(img);
	
	DatasetPtrs mp; GetDataset(mp);
	if (!mp.owner || !mp.biography || !categories.IsCursor() || !years.IsCursor() || !entries.IsCursor())
		return;
	Owner& owner = *mp.owner;
	Biography& biography = GetExt<Biography>();
	int cat_i = categories.Get("IDX");
	BiographyCategory& bcat = biography.GetAdd(owner, cat_i);
	int year_i = years.Get("IDX");
	if (year_i >= bcat.years.GetCount()) return;
	BioYear& by = bcat.years[year_i];
	int entry_i = entries.Get("IDX");
	BioImage& bimg = by.images[entry_i];
	
	TODO
	#if 0
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
	
	bimg.image_hash = h;
	#endif
}

void BiographyCtrl::Image_AnalyseImage() {
	REF(image, vsplit);
	REF(image, bsplit);
	REF(image, years);
	REF(image, year);
	REF(image, entries);
	DatasetPtrs mp; GetDataset(mp);
	if (!mp.owner || !mp.biography || !categories.IsCursor() || !years.IsCursor() || !entries.IsCursor())
		return;
	
	Owner& owner = *mp.owner;
	Biography& biography = GetExt<Biography>();
	int cat_i = categories.Get("IDX");
	BiographyCategory& bcat = biography.GetAdd(owner, cat_i);
	int year_i = years.Get("IDX");
	if (year_i >= bcat.years.GetCount()) return;
	BioYear& by = bcat.years[year_i];
	int entry_i = entries.Get("IDX");
	BioImage& bimg = by.images[entry_i];
	
	if (!bimg.image_hash)
		return;
	
	TODO
	#if 0
	String path = CacheImageFile(bimg.image_hash);
	if (!FileExists(path))
		path = ThumbnailImageFile(bimg.image_hash);
	/*Image i = StreamRaster::LoadFileAny(path);
	if (i.IsEmpty())
		return;*/
	String jpeg = LoadFile(path);
	if (jpeg.IsEmpty())
		return;
	
	TaskMgr& m = AiTaskManager();
	
	VisionArgs args;
	args.fn = 0;
	//args.img = i;
	
	m.GetVision(jpeg, args, [this](String s_) {
		PostCallback([this,s_](){
			String s = TrimBoth(s_);
			if (s.Left(1) == "\"") s = s.Mid(1);
			if (s.Right(1) == "\"") s = s.Left(s.GetCount()-1);
			year.image_text.SetData(s);
			OnValueChange();
		});
	});
	#endif
}





END_UPP_NAMESPACE
#undef REF
