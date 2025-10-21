#include "Video.h"


NAMESPACE_UPP


VideoStoryboardCtrl::VideoStoryboardCtrl() {
	Add(split.SizePos());
	
	split.Horz() << list << vsplit0 << vsplit1;
	
	list.AddColumn("Lyrics");
	list.AddColumn("Image #1");
	list.AddColumn("Image #2");
	list.AddColumn("Image #3");
	list.AddColumn("Image #4");
	list.WhenCursor << THISBACK(DataLine);
	list.ColumnWidths("4 1 1 1 1");
	list.SetLineCy(48);
	
	vsplit0.Vert() << img[0] << img[1];
	vsplit1.Vert() << img[2] << img[3];
	
	
}

void VideoStoryboardCtrl::Data() {
	DatasetPtrs p; GetDataset(p);
	if (!p.song)
		return;
	String dir = GetFileDirectory(GetFilePath());
	
	
	// Text to storyboard parts
	for(int i = 0; i < p.song->text_storyboard_parts.GetCount(); i++) {
		int part_i = p.song->text_storyboard_parts[i];
		const String& part = p.song->storyboard_parts.GetKey(part_i);
		list.Set(i, 0, p.song->text_storyboard_parts.GetKey(i));
		
		if (i < p.song->text_storyboard_hashes.GetCount()) {
			const auto& hashes = p.song->text_storyboard_hashes[i];
			for(int j = 0; j < 4 && j < hashes.GetCount(); j++) {
				hash_t h = hashes[j];
				String thumb_path = ThumbnailImageFile(dir, h);
				Image img = StreamRaster::LoadFileAny(thumb_path);
				if (img.IsEmpty()) {
					list.Set(i, 1+j, Value());
				}
				else {
					img = CachedRescale(img, Size(48,48));
					list.Set(i, 1+j, img);
				}
			}
		}
		else {
			for(int j = 0; j < 4; j++) {
				list.Set(i, 1+j, Value());
			}
		}
	}
	INHIBIT_CURSOR_(list, c);
	list.SetCount(p.song->text_storyboard_parts.GetCount());
	if (list.GetCount() && !list.IsCursor())
		list.SetCursor(0);
	
	DataLine();
}

void VideoStoryboardCtrl::DataLine() {
	DatasetPtrs p; GetDataset(p);
	if (!p.song || !list.IsCursor())
		return;
	
	const auto& hashes = p.song->text_storyboard_hashes;
	int list_i = list.GetCursor();
	if (list_i >= hashes.GetCount())
		return;
	
	String cache_dir = ConfigFile("");;
	String dir = GetFileDirectory(GetFilePath());
	const auto& prompt_hashes = hashes[list_i];
	
	for(int i = 0; i < 4; i++) {
		if (i < prompt_hashes.GetCount()) {
			hash_t h = prompt_hashes[i];
			String path = FullImageFile(dir, h);
			if (!FileExists(path)) {
				path = CacheImageFile(cache_dir, h);
				if (!FileExists(path)) {
					path = ThumbnailImageFile(dir, h);
					if (!FileExists(path)) {
						img[i].Clear();
						continue;
					}
				}
			}
			Image prompt_img = StreamRaster::LoadFileAny(path);
			img[i].SetImage(prompt_img);
		}
		else {
			img[i].Clear();
		}
	}
}

void VideoStoryboardCtrl::OnValueChange() {
	
}

void VideoStoryboardCtrl::ToolMenu(Bar& bar) {
	bar.Add(t_("Update"), MetaImgs::BlueRing(), THISBACK(Data)).Key(K_CTRL_Q);
	bar.Separator();
	bar.Add(t_("Make video prompts"), MetaImgs::RedRing(), THISBACK1(Do, 0)).Key(K_F5);
}

void VideoStoryboardCtrl::Do(int fn) {
	DatasetPtrs p; GetDataset(p);
	if (!p.song) return;
	
	if (fn == 0) {
		VideoSolver& tm = VideoSolver::Get(*p.song);
		tm.Start();
	}
}


INITIALIZER_COMPONENT_CTRL(VideoStoryboard, VideoStoryboardCtrl)

END_UPP_NAMESPACE
