#include "Content.h"



NAMESPACE_UPP



ImageBiographyProcess::ImageBiographyProcess() {
	
}

int ImageBiographyProcess::GetPhaseCount() const {
	return PHASE_COUNT;
}

int ImageBiographyProcess::GetBatchCount(int phase) const {
	switch (phase) {
		case PHASE_ANALYZE_IMAGE_BIOGRAPHY:			return max(1, vision_tasks.GetCount());
		default: return 1;
	}
}

int ImageBiographyProcess::GetSubBatchCount(int phase, int batch) const {
	return 1;
}

void ImageBiographyProcess::DoPhase() {
	switch (phase) {
		case PHASE_ANALYZE_IMAGE_BIOGRAPHY:			ProcessAnalyzeImageBiography(); return;
		default: return;
	}
}

ImageBiographyProcess& ImageBiographyProcess::Get(Profile& p, BiographyPerspectives& snap) {
	static ArrayMap<String, ImageBiographyProcess> arr;
	
	TODO
	#if 0
	String key = "PROFILE(" + p.name + "), REVISION(" + IntStr(snap.revision) + ")";
	ImageBiographyProcess& ts = arr.GetAdd(key);
	ts.owner = p.owner;
	ts.profile = &p;
	ts.snap = &snap;
	ASSERT(ts.owner);
	return ts;
	#endif
	return Single<ImageBiographyProcess>();
}

void ImageBiographyProcess::TraverseVisionTasks() {
	TODO
	#if 0
	Biography& biography = snap->data;
	for(int i = 0; i < BIOCATEGORY_COUNT; i++) {
		BiographyCategory& bcat = biography.GetAdd(*owner, i);
		for(int j = 0; j < bcat.years.GetCount(); j++) {
			BioYear& by = bcat.years[j];
			
			for(int k = 0; k < by.images.GetCount(); k++) {
				BioImage& bimg = by.images[k];
				if (phase == PHASE_ANALYZE_IMAGE_BIOGRAPHY && bimg.image_text.IsEmpty() && bimg.image_hash != 0) {
					String path = CacheImageFile(bimg.image_hash);
					if (!FileExists(path))
						path = ThumbnailImageFile(bimg.image_hash);
					String jpeg = LoadFile(path);
					if (!jpeg.IsEmpty()) {
						VisionTask& t = vision_tasks.Add();
						t.bimg = &bimg;
						t.jpeg = jpeg;
					}
				}
			}
		}
	}
	#endif
}

void ImageBiographyProcess::ProcessAnalyzeImageBiography() {
	TODO
	#if 0
	
	if (batch == 0) {
		vision_tasks.Clear();
		TraverseVisionTasks();
	}
	
	if (batch >= vision_tasks.GetCount()) {
		NextPhase();
		return;
	}
	
	const VisionTask& t = vision_tasks[batch];
	
	VisionArgs args;
	args.fn = 0;
	
	SetWaiting(1);
	AiTaskManager();
	m.GetVision(t.jpeg, args, THISBACK(OnProcessAnalyzeImageBiography));
	
	#endif
}

void ImageBiographyProcess::OnProcessAnalyzeImageBiography(String res) {
	const VisionTask& t = vision_tasks[batch];
	
	String& s = t.bimg->image_text;
	s = TrimBoth(res);
	if (s.Left(1) == "\"") s = s.Mid(1);
	if (s.Right(1) == "\"") s = s.Left(s.GetCount()-1);
	
	TODO
	#if 0
	NextBatch();
	SetWaiting(0);
	#endif
}


END_UPP_NAMESPACE
