#include <AI/Ctrl/Ctrl.h>
#include <plugin/jpg/jpg.h>

NAMESPACE_UPP


void BiographyPlatformCtrl::Platforms::Marketplace::Ctor() {
	this->p.tabs.Add(hsplit.SizePos(), "Marketplace");
	
	hsplit.Horz() << items << tabs << imgsplit;
	hsplit.SetPos(1500, 0);
	hsplit.SetPos(1500+4000, 1);
	
	imgsplit.Vert() << images << img;
	imgsplit.SetPos(2500);
	
	tabs.Add(form.SizePos(), "Form");
	tabs.Add(viewer.SizePos(), "Viewer");
	tabs.WhenSet << THISBACK(DataItem);
	CtrlLayout(form);
	CtrlLayout(viewer);
	
	items.AddColumn(t_("Category"));
	items.AddColumn(t_("Title"));
	items.ColumnWidths("2 4");
	items.AddIndex("IDX");
	items.WhenBar << [this](Bar& bar) {
		bar.Add(t_("Add item"), THISBACK1(Do, 0));
		bar.Add(t_("Remove item"), THISBACK1(Do, 1));
		bar.Separator();
		bar.Add(t_("Show all"), THISBACK1(ShowItems, -1));
		for(int i = 0; i < MARKETPRIORITY_COUNT; i++)
			bar.Add(t_("Show ") + GetMarketPriorityKey(i), THISBACK1(ShowItems, i));
	};
	items.WhenCursor << THISBACK(DataItem);
	
	images.AddColumn(t_("#"));
	images.WhenCursor << THISBACK(DataImage);
	images.WhenBar << [this](Bar& bar) {
		bar.Add(t_("Add image from clipboard"), THISBACK1(Do, 2));
		bar.Add(t_("Set image from clipboard"), THISBACK1(Do, 3));
		bar.Add(t_("Remove image"), THISBACK1(Do, 4));
		bar.Add(t_("Move up"), THISBACK1(Do, 5));
		bar.Add(t_("Move down"), THISBACK1(Do, 6));
	};
	for(int i = 0; i < MARKETPRIORITY_COUNT; i++)
		form.priority.Add(GetMarketPriorityKey(i));
	
	form.priority	<<= THISBACK(OnValueChange);
	form.added		<<= THISBACK(OnValueChange);
	form.generic	<<= THISBACK(OnValueChange);
	form.brand		<<= THISBACK(OnValueChange);
	form.model		<<= THISBACK(OnValueChange);
	form.price		<<= THISBACK(OnValueChange);
	form.width		<<= THISBACK(OnDimensionChange);
	form.height		<<= THISBACK(OnDimensionChange);
	form.depth		<<= THISBACK(OnDimensionChange);
	form.weight		<<= THISBACK(OnDimensionChange);
	form.faults		<<= THISBACK(OnValueChange);
	form.works		<<= THISBACK(OnValueChange);
	form.broken		<<= THISBACK(OnValueChange);
	form.good		<<= THISBACK(OnValueChange);
	form.year		<<= THISBACK(OnValueChange);
	form.other		<<= THISBACK(OnValueChange);
	viewer.title		<<= THISBACK(OnValueChange);
	viewer.description	<<= THISBACK(OnValueChange);
	form.category		<<= THISBACK(OnCategory);
	form.subcategory	<<= THISBACK(OnSubCategory);
	form.set_book		<<= THISBACK1(SetCategoryShorcut, 1);
	
	viewer.make_images <<= THISBACK(MakeTempImages);
	
}

void BiographyPlatformCtrl::Platforms::Marketplace::DataPlatform() {
	DatasetPtrs p; o.GetDataset(p);
	
	if (!p.owner) {
		items.Clear();
		images.Clear();
		ClearForm();
		return;
	}
	
	if (form.category.GetCount() == 0) {
		const auto& map = GetMarketplaceSections();
		for(int i = 0; i < map.GetCount(); i++) {
			String key = map.GetKey(i);
			form.category.Add(key);
		}
		form.category.SetIndex(0);
	}
	
	int row = 0;
	for(int i = 0; i < p.analysis->market_items.GetCount(); i++) {
		MarketplaceItem& mi = p.analysis->market_items[i];
		
		if (filter_priority >= 0 && mi.priority != filter_priority)
			continue;
		
		const auto& sects = GetMarketplaceSections();
		
		String cat = sects.GetKey(mi.category) + ": " + sects[mi.category][mi.subcategory];
		items.Set(row, 0, cat);
		items.Set(row, 1, mi.GetTitle());
		items.Set(row, "IDX", i);
		row++;
	}
	INHIBIT_CURSOR(items);
	items.SetCount(row);
	items.SetSortColumn(1);
	items.SetSortColumn(0);
	if (!items.IsCursor() && items.GetCount())
		items.SetCursor(0);
	
	DataItem();
}

void BiographyPlatformCtrl::Platforms::Marketplace::ToolMenu(Bar& bar) {
	bar.Add(t_("Add item"), MetaImgs::BlueRing(), THISBACK1(Do, 0)).Key(K_CTRL_T);
	bar.Add(t_("Remove item"), MetaImgs::BlueRing(), THISBACK1(Do, 1)).Key(K_CTRL|K_W|K_SHIFT);
	bar.Separator();
	bar.Add(t_("Rotate image clockwise"), MetaImgs::BlueRing(), THISBACK1(Do, 9)).Key(K_CTRL_F);
	bar.Add(t_("Rotate image counter-clockwise"), MetaImgs::BlueRing(), THISBACK1(Do, 10)).Key(K_CTRL_G);
	bar.Separator();
	bar.Add(t_("Set recursive image directory from clipboard"), MetaImgs::BlueRing(), THISBACK1(Do, 8)).Key(K_F3);
	bar.Add(t_("Set image directory from clipboard"), MetaImgs::BlueRing(), THISBACK1(Do, 7)).Key(K_F4);
	bar.Add(t_("Add image from clipboard"), MetaImgs::BlueRing(), THISBACK1(Do, 2)).Key(K_F5);
	bar.Add(t_("Set image from clipboard"), MetaImgs::BlueRing(), THISBACK1(Do, 3)).Key(K_F6);
	bar.Add(t_("Remove image"), MetaImgs::BlueRing(), THISBACK1(Do, 4)).Key(K_F7);
	bar.Add(t_("Move up"), MetaImgs::BlueRing(), THISBACK1(Do, 5)).Key(K_F8);
	bar.Add(t_("Move down"), MetaImgs::BlueRing(), THISBACK1(Do, 6)).Key(K_F9);
	bar.Separator();
	bar.Add(t_("Start"), MetaImgs::RedRing(), THISBACK1(Do, 11));
	bar.Add(t_("Stop"), MetaImgs::RedRing(), THISBACK1(Do, 12));
	
}

void BiographyPlatformCtrl::Platforms::Marketplace::ClearForm() {
	form.added.Clear();
	form.generic.Clear();
	form.brand.Clear();
	form.model.Clear();
	form.price.Clear();
	form.width.Clear();
	form.height.Clear();
	form.depth.Clear();
	form.weight.Clear();
	form.faults.Clear();
	form.works.Clear();
	form.broken.Set(0);
	form.good.Set(0);
}

void BiographyPlatformCtrl::Platforms::Marketplace::DataItem() {
	if (!items.IsCursor()) {
		images.Clear();
		ClearForm();
		img.Clear();
		return;
	}
	
	DatasetPtrs p; o.GetDataset(p);
	int item_i = items.Get("IDX");
	TODO
	#if 0
	if (item_i < 0 || item_i >= p.owner->marketplace.items.GetCount()) {
		PostCallback(THISBACK(DataPlatform));
		return;
	}
	MarketplaceItem& mi = p.owner->marketplace.items[item_i];
	
	form.priority.SetIndex(mi.priority);
	form.added.SetData(mi.added);
	form.generic.SetData(mi.generic);
	form.brand.SetData(mi.brand);
	form.model.SetData(mi.model);
	form.price.SetData(mi.price);
	form.width.SetData(mi.cx);
	form.height.SetData(mi.cy);
	form.depth.SetData(mi.cz);
	form.weight.SetData(mi.weight);
	form.faults.SetData(mi.faults);
	form.works.SetData(mi.works);
	form.broken.SetData(mi.broken);
	form.good.SetData(mi.good);
	form.year.SetData(mi.year_of_manufacturing);
	form.other.SetData(mi.other);
	String pkg_str = GetPackageString(mi.cx, mi.cy, mi.cz, mi.weight);
	form.package.SetData(pkg_str);
	viewer.package.SetData(pkg_str);
	
	viewer.title.SetData(mi.title);
	viewer.description.SetData(mi.description);
	
	for(int i = 0; i < mi.images.GetCount(); i++) {
		hash_t h = mi.images[i];
		images.Set(i, 0, i);
	}
	images.SetCount(mi.images.GetCount());
	INHIBIT_CURSOR(images);
	if (images.GetCount() && !images.IsCursor())
		images.SetCursor(0);
	
	int tab = tabs.Get();
	if (tab == 0) {
		DataCategory();
	}
	else if (tab == 1) {
		const auto& sects = GetMarketplaceSections();
		viewer.category.SetData(sects.GetKey(mi.category));
		viewer.subcategory.SetData(sects[mi.category][mi.subcategory]);
		viewer.condition.SetData(mi.broken ? "Broken" : (mi.good ? "Good" : "Fair"));
		viewer.price.SetData(Format("%.2!n€", mi.price));
	}
	#endif
	
	DataImage();
}

void BiographyPlatformCtrl::Platforms::Marketplace::DataCategory() {
	DatasetPtrs p; o.GetDataset(p);
	if (!p.owner || !items.IsCursor())
		return;
	
	int item_i = items.Get("IDX");
	MarketplaceItem& mi = p.analysis->market_items[item_i];
	
	// Normalize values (ugly, cos in gui)
	const auto& sects = GetMarketplaceSections();
	if (mi.category < 0 || mi.category >= sects.GetCount())
		mi.category = 0;
	
	// Update droplist values based on category
	form.category.SetIndex(mi.category);
	
	DataSubCategory();
}

void BiographyPlatformCtrl::Platforms::Marketplace::DataSubCategory() {
	DatasetPtrs p; o.GetDataset(p);
	if (!p.analysis || !items.IsCursor())
		return;
	
	int item_i = items.Get("IDX");
	MarketplaceItem& mi = p.analysis->market_items[item_i];
	
	const auto& sects = GetMarketplaceSections();
	const auto& sub_sects = sects[mi.category];
	if (mi.subcategory < 0 || mi.subcategory >= sub_sects.GetCount())
		mi.subcategory = 0;
	
	form.subcategory.Clear();
	for(int i = 0; i < sub_sects.GetCount(); i++)
		form.subcategory.Add(sub_sects[i]);
	form.subcategory.SetIndex(mi.subcategory);
}

void BiographyPlatformCtrl::Platforms::Marketplace::DataImage() {
	if (!images.IsCursor()) {
		img.Clear();
		return;
	}
	DatasetPtrs p; o.GetDataset(p);
	int item_i = items.Get("IDX");
	
	if (item_i < 0 || item_i >= p.analysis->market_items.GetCount()) {
		o.PostCallback(THISBACK(DataPlatform));
		return;
	}
	MarketplaceItem& mi = p.analysis->market_items[item_i];
	int img_i = images.GetCursor();
	hash_t h = mi.images[img_i];
	String cache_dir = ConfigFile("");
	
	if (h) {
		String path = CacheImageFile(cache_dir, h);
		Image i = StreamRaster::LoadFileAny(path);
		img.SetImage(i);
	}
	else {
		img.Clear();
	}
}

void BiographyPlatformCtrl::Platforms::Marketplace::Do(int fn) {
	DatasetPtrs p; o.GetDataset(p);
	
	if (fn == 0) {
		if (!p.owner) return;
		MarketplaceItem& mi = p.analysis->market_items.Add();
		mi.added = GetSysTime();
		o.PostCallback(THISBACK(DataPlatform));
		o.PostCallback([this]() {
			if (items.GetCount())
				items.SetCursor(items.GetCount()-1);
		});
	}
	else if (fn == 1) {
		if (!p.owner || !items.IsCursor()) return;
		int item_i = items.Get("IDX");
		if (item_i < 0 || item_i >= p.analysis->market_items.GetCount()) {
			o.PostCallback(THISBACK(DataPlatform));
			return;
		}
		p.analysis->market_items.Remove(item_i);
		o.PostCallback(THISBACK(DataPlatform));
	}
	else if (fn == 2) {
		if (!p.owner || !items.IsCursor()) return;
		int item_i = items.Get("IDX");
		if (item_i < 0 || item_i >= p.analysis->market_items.GetCount()) {
			o.PostCallback(THISBACK(DataPlatform));
			return;
		}
		MarketplaceItem& mi = p.analysis->market_items[item_i];
		int img_i = mi.images.GetCount();
		mi.images.Add(0);
		images.Set(img_i,0,img_i);
		images.SetCursor(img_i);
		PasteImagePath();
		o.PostCallback(THISBACK(DataItem));
	}
	else if (fn == 3) {
		PasteImagePath();
	}
	else if (fn == 4) {
		if (!p.owner || !items.IsCursor() || !images.IsCursor()) return;
		int item_i = items.Get("IDX");
		if (item_i < 0 || item_i >= p.analysis->market_items.GetCount()) {
			o.PostCallback(THISBACK(DataPlatform));
			return;
		}
		int img_i = images.GetCursor();
		MarketplaceItem& mi = p.analysis->market_items[item_i];
		mi.images.Remove(img_i);
		o.PostCallback(THISBACK(DataItem));
	}
	else if (fn == 5) {
		if (!p.owner || !items.IsCursor() || !images.IsCursor()) return;
		int item_i = items.Get("IDX");
		if (item_i < 0 || item_i >= p.analysis->market_items.GetCount()) {
			o.PostCallback(THISBACK(DataPlatform));
			return;
		}
		MarketplaceItem& mi = p.analysis->market_items[item_i];
		int img_i = images.GetCursor();
		if (img_i == 0) return;
		hash_t h = mi.images[img_i];
		mi.images.Remove(img_i);
		mi.images.Insert(img_i-1, h);
		images.SetCursor(img_i-1);
		o.PostCallback(THISBACK(DataItem));
	}
	else if (fn == 6) {
		if (!p.owner || !items.IsCursor() || !images.IsCursor()) return;
		int item_i = items.Get("IDX");
		if (item_i < 0 || item_i >= p.analysis->market_items.GetCount()) {
			o.PostCallback(THISBACK(DataPlatform));
			return;
		}
		MarketplaceItem& mi = p.analysis->market_items[item_i];
		int img_i = images.GetCursor();
		if (img_i == mi.images.GetCount()-1) return;
		hash_t h = mi.images[img_i];
		mi.images.Remove(img_i);
		mi.images.Insert(img_i+1, h);
		images.SetCursor(img_i+1);
		o.PostCallback(THISBACK(DataItem));
	}
	else if (fn == 7) {
		String dir = ReadClipboardText();
		if (!DirectoryExists(dir)) return;
		int item_i = items.Get("IDX");
		if (item_i < 0 || item_i >= p.analysis->market_items.GetCount()) {
			o.PostCallback(THISBACK(DataPlatform));
			return;
		}
		String fname = GetFileName(dir);
		MarketplaceItem& mi = p.analysis->market_items[item_i];
		if (mi.generic.IsEmpty())
			mi.generic = fname;
		FindFile ff(AppendFileName(dir, "*.jpg"));
		do {
			String fname = ff.GetName();
			if (fname == "." || fname == "..") continue;
			int img_i = mi.images.GetCount();
			mi.images.Add(0);
			images.Set(img_i,0,img_i);
			images.SetCursor(img_i);
			LoadImagePath(ff.GetPath());
		}
		while (ff.Next());
		o.PostCallback(THISBACK(DataItem));
	}
	else if (fn == 8) {
		String dir = ReadClipboardText();
		if (!DirectoryExists(dir)) return;
		FindFile dirs(AppendFileName(dir, "*"));
		do {
			if (!dirs.IsDirectory()) continue;
			String dir_name = dirs.GetName();
			if (dir_name == "." || dir_name == "..") continue;
			if (dir_name.Left(2) == "__") continue;
			
			bool found = false;
			for (MarketplaceItem& mi : p.analysis->market_items) {
				if (mi.generic == dir_name) {
					found = true;
					break;
				}
			}
			if (found) continue;
			
			String dir = dirs.GetPath();
			int item_i = p.analysis->market_items.GetCount();
			INHIBIT_CURSOR(items);
			items.Set(item_i,"IDX",item_i);
			items.SetCursor(item_i);
			MarketplaceItem& mi = p.analysis->market_items.Add();
			if (mi.generic.IsEmpty())
				mi.generic = dir_name;
			FindFile ff(AppendFileName(dir, "*.jpg"));
			do {
				String fname = ff.GetName();
				if (fname == "." || fname == "..") continue;
				int img_i = mi.images.GetCount();
				mi.images.Add(0);
				images.Set(img_i,0,img_i);
				INHIBIT_CURSOR(images);
				images.SetCursor(img_i);
				LoadImagePath(ff.GetPath());
			}
			while (ff.Next());
		}
		while (dirs.Next());
		o.PostCallback(THISBACK(DataPlatform));
	}
	else if (fn == 9) {
		if (!images.IsCursor())
			return;
		int item_i = items.Get("IDX");
		if (item_i < 0 || item_i >= p.analysis->market_items.GetCount()) {
			o.PostCallback(THISBACK(DataPlatform));
			return;
		}
		MarketplaceItem& mi = p.analysis->market_items[item_i];
		int img_i = images.GetCursor();
		hash_t h = mi.images[img_i];
		
		if (h) {
			String cache_dir = ConfigFile("");
			String path = CacheImageFile(cache_dir, h);
			Image i = StreamRaster::LoadFileAny(path);
			i = RotateClockwise(i);
			JPGEncoder enc(100);
			enc.SaveFile(path, i);
			this->img.SetImage(i);
		}
	}
	else if (fn == 10) {
		if (!images.IsCursor())
			return;
		int item_i = items.Get("IDX");
		if (item_i < 0 || item_i >= p.analysis->market_items.GetCount()) {
			o.PostCallback(THISBACK(DataPlatform));
			return;
		}
		MarketplaceItem& mi = p.analysis->market_items[item_i];
		int img_i = images.GetCursor();
		hash_t h = mi.images[img_i];
		
		if (h) {
			String cache_dir = ConfigFile("");
			String path = CacheImageFile(cache_dir, h);
			Image i = StreamRaster::LoadFileAny(path);
			i = RotateAntiClockwise(i);
			JPGEncoder enc(100);
			enc.SaveFile(path, i);
			this->img.SetImage(i);
		}
	}
	else if (fn == 11 || fn == 12) {
		MarketplaceProcess& ss = MarketplaceProcess::Get(p);
		if (fn == 11)
			ss.Start();
		else if (fn == 12)
			ss.Stop();
	}
}

void BiographyPlatformCtrl::Platforms::Marketplace::OnDimensionChange() {
	DatasetPtrs p; o.GetDataset(p);
	int item_i = items.Get("IDX");
	
	MarketplaceItem& mi = p.analysis->market_items[item_i];
	OnValueChange();
	String pkg_str = GetPackageString((int)mi.cx, (int)mi.cy, (int)mi.cz, mi.weight);
	form.package.SetData(pkg_str);
	viewer.package.SetData(pkg_str);
}

void BiographyPlatformCtrl::Platforms::Marketplace::OnValueChange() {
	if (!items.IsCursor())
		return;
	
	DatasetPtrs p; o.GetDataset(p);
	int item_i = items.Get("IDX");
	
	if (item_i < 0 || item_i >= p.analysis->market_items.GetCount()) {
		o.PostCallback(THISBACK(DataPlatform));
		return;
	}
	MarketplaceItem& mi = p.analysis->market_items[item_i];
	
	mi.priority = form.priority.GetIndex();
	mi.added = form.added.GetData();
	mi.generic = form.generic.GetData();
	mi.brand = form.brand.GetData();
	mi.model = form.model.GetData();
	mi.price = form.price.GetData();
	mi.cx = form.width.GetData();
	mi.cy = form.height.GetData();
	mi.cz = form.depth.GetData();
	mi.weight = form.weight.GetData();
	mi.faults = form.faults.GetData();
	mi.works = form.works.GetData();
	mi.broken = form.broken.GetData();
	mi.good = form.good.GetData();
	mi.year_of_manufacturing = form.year.GetData();
	mi.other = form.other.GetData();
	
	mi.description = viewer.description.GetData();
	mi.title = viewer.title.GetData();
	
	const auto& sects = GetMarketplaceSections();
	String cat = sects.GetKey(mi.category) + ": " + sects[mi.category][mi.subcategory];
	items.Set(0, cat);
	items.Set(1, mi.GetTitle());
}

void BiographyPlatformCtrl::Platforms::Marketplace::PasteImagePath() {
	Image img = ReadClipboardImage();
	if (!img.IsEmpty()) {
		SetCurrentImage(img);
	}
	else {
		String path = ReadClipboardText();
		if (path.Left(7) == "file://") {
			path = path.Mid(7);
			img = StreamRaster::LoadFileAny(path);
			if (!img.IsEmpty())
				SetCurrentImage(img);
		}
	}
}

void BiographyPlatformCtrl::Platforms::Marketplace::LoadImagePath(String path) {
	Image img = StreamRaster::LoadFileAny(path);
	if (!img.IsEmpty())
		SetCurrentImage(img);
}

void BiographyPlatformCtrl::Platforms::Marketplace::SetCurrentImage(Image img) {
	this->img.SetImage(img);
	
	DatasetPtrs mp; o.GetDataset(mp);
	if (!mp.owner || !items.IsCursor() || !images.IsCursor())
		return;
	
	int item_i = items.Get("IDX");
	if (item_i < 0 || item_i >= mp.analysis->market_items.GetCount()) {
		o.PostCallback(THISBACK(DataPlatform));
		return;
	}
	int img_i = images.GetCursor();
	MarketplaceItem& mi = mp.analysis->market_items[item_i];
	
	hash_t h = img.GetHashValue();
	String cache_dir = ConfigFile("");
	String cache_path = CacheImageFile(cache_dir, h);
	RealizeDirectory(GetFileDirectory(cache_path));
	
	if (!FileExists(cache_path)) {
		Image small_img = RescaleToFit(img, 1024);
		JPGEncoder enc(98);
		enc.SaveFile(cache_path, small_img);
	}
	
	if (img_i >= mi.images.GetCount())
		mi.images.SetCount(img_i+1,0);
	mi.images[img_i] = h;
	
	o.PostCallback(THISBACK(DataImage));
}

void BiographyPlatformCtrl::Platforms::Marketplace::MakeTempImages() {
	#ifdef flagPOSIX
	String dir = GetHomeDirFile(".tmp_images");
	#else
	String dir = GetHomeDirFile("Temp Images");
	#endif
	RealizeDirectory(dir);
	
	DatasetPtrs mp; o.GetDataset(mp);
	if (!mp.owner || !items.IsCursor())
		return;
	
	int item_i = items.Get("IDX");
	MarketplaceItem& mi = mp.analysis->market_items[item_i];
	
	FindFile ff(AppendFileName(dir, "*.jpg"));
	do {
		DeleteFile(ff.GetPath());
	}
	while (ff.Next());
	
	String cache_dir = ConfigFile("");
	for(int i = 0; i < mi.images.GetCount(); i++) {
		String src = CacheImageFile(cache_dir, mi.images[i]);
		String dst = AppendFileName(dir, IntStr(i) + ".jpg");
		FileIn in(src);
		FileOut out(dst);
		CopyStream(out, in);
	}
}

void BiographyPlatformCtrl::Platforms::Marketplace::OnCategory() {
	DatasetPtrs mp; o.GetDataset(mp);
	if (!mp.owner || !items.IsCursor())
		return;
	
	int item_i = items.Get("IDX");
	MarketplaceItem& mi = mp.analysis->market_items[item_i];
	
	mi.category = form.category.GetIndex();
	mi.subcategory = 0;
	
	DataSubCategory();
}

void BiographyPlatformCtrl::Platforms::Marketplace::OnSubCategory() {
	DatasetPtrs mp; o.GetDataset(mp);
	if (!mp.owner || !items.IsCursor())
		return;
	
	int item_i = items.Get("IDX");
	MarketplaceItem& mi = mp.analysis->market_items[item_i];
	
	mi.subcategory = form.subcategory.GetIndex();
}

void BiographyPlatformCtrl::Platforms::Marketplace::SetCategoryShorcut(int i) {
	DatasetPtrs mp; o.GetDataset(mp);
	if (!mp.owner || !items.IsCursor())
		return;
	
	int item_i = items.Get("IDX");
	
	MarketplaceItem& mi = mp.analysis->market_items[item_i];
	mi.category = 10;
	mi.subcategory = 2;
	
	DataCategory();
}

String BiographyPlatformCtrl::Platforms::Marketplace::GetPackageString(int w, int h, int d, double weight) {
	if (!w || !h || !d) return String();
	
	Vector<int> lengths;
	lengths << w << h << d;
	Sort(lengths, StdLess<int>());
	
	static const int PKG_TYPE_COUNT = 4;
	int limits[PKG_TYPE_COUNT][4] = {
		{3,25,35,2},
		{15,32,40,4},
		{26,32,40,25},
		{60,60,160,25}
	};
	
	for(int i = 0; i < PKG_TYPE_COUNT; i++) {
		bool fits = true;
		for(int j = 0; j < 3; j++)
			if (limits[i][j] < lengths[j])
				fits = false;
		if (weight != 0.0 && weight > limits[i][3])
			fits = false;
		if (fits) {
			String s;
			switch (i) {
				case 0: s << "Pikkupaketti"; break;
				case 1: s << "Peruspaketti"; break;
				case 2: s << "Iso paketti"; break;
				case 3: s << "Jättipaketti"; break;
				default: break;
			}
			s << Format(", %d cm x %d cm x %d cm, Max %d kg", limits[i][0], limits[i][1], limits[i][2], limits[i][3]);
			return s;
		}
	}
	return "";
}


void BiographyPlatformCtrl::Platforms::Marketplace::ShowItems(int priority) {
	filter_priority = priority;
	o.PostCallback(THISBACK(DataPlatform));
}


END_UPP_NAMESPACE
