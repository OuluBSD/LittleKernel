#include "Content.h"
#include <AI/Core/Marketing/Marketing.h>


NAMESPACE_UPP


ImageBiographySummaryProcess::ImageBiographySummaryProcess() {
	
}

int ImageBiographySummaryProcess::GetPhaseCount() const {
	return PHASE_COUNT;
}

int ImageBiographySummaryProcess::GetBatchCount(int phase) const {
	switch (phase) {
		case PHASE_SUMMARIZE_IMAGE_CATEGORY_YEAR:	return max(1, imgsum_tasks.GetCount());
		case PHASE_SUMMARIZE_IMAGE_BIOGRAPHY:		return BIOCATEGORY_COUNT;
		default: return 1;
	}
}

int ImageBiographySummaryProcess::GetSubBatchCount(int phase, int batch) const {
	TODO
	#if 0
	if (phase == PHASE_SUMMARIZE_IMAGE_BIOGRAPHY) {
		BiographyCategory& bcat = p.snap->data.GetAdd(*p.owner, batch);
		return bcat.summaries.GetCount();
	}
	#endif
	return 1;
}

void ImageBiographySummaryProcess::DoPhase() {
	switch (phase) {
		case PHASE_SUMMARIZE_IMAGE_CATEGORY_YEAR:	ProcessSummarizeImageCategoryYear(); return;
		case PHASE_SUMMARIZE_IMAGE_BIOGRAPHY:		ProcessSummarizeImageBiography(); return;
		default: return;
	}
}

ImageBiographySummaryProcess& ImageBiographySummaryProcess::Get(Profile& prof, BiographyPerspectives& snap) {
	static ArrayMap<String, ImageBiographySummaryProcess> arr;
	
	TODO
	#if 0
	String key = "PROFILE(" + p.name + "), REVISION(" + IntStr(snap.revision) + ")";
	ImageBiographySummaryProcess& ts = arr.GetAdd(key);
	ts.owner = prof.owner;
	ts.profile = &p;
	ts.snap = &snap;
	ASSERT(ts.owner);
	return ts;
	#endif
	return Single<ImageBiographySummaryProcess>();
}

void ImageBiographySummaryProcess::TraverseImageSummaryTasks() {
	TODO
	#if 0
	Biography& biography = p.snap->data;
	for(int i = 0; i < BIOCATEGORY_COUNT; i++) {
		BiographyCategory& bcat = biography.GetAdd(*p.owner, i);
		int bcat_i = i;
		for(int j = 0; j < bcat.years.GetCount(); j++) {
			BioYear& by = bcat.years[j];
			by.RealizeImageSummaries();
			for(int k = 0; k < by.image_summaries.GetCount(); k++) {
				const BioRange& range = by.image_summaries.GetKey(k);
				BioImage& bimg = by.image_summaries[k];
				if (phase == PHASE_SUMMARIZE_IMAGE_CATEGORY_YEAR && bimg.image_text.IsEmpty()) {
					ImageSummaryTask& t = imgsum_tasks.Add();
					t.bcat = &bcat;
					t.by = &by;
					t.range = range;
					t.summary = &bimg;
					t.bcat_i = bcat_i;
				}
			}
		}
	}
	#endif
}

void ImageBiographySummaryProcess::ProcessSummarizeImageCategoryYear() {
	
	if (batch == 0) {
		imgsum_tasks.Clear();
		TraverseImageSummaryTasks();
	}
	
	if (batch >= imgsum_tasks.GetCount()) {
		NextPhase();
		return;
	}
	
	const ImageSummaryTask& t = imgsum_tasks[batch];
	const BioRange& range = t.range;
	const BiographyCategory& bcat = *t.bcat;
	BioYear& by = *t.by;
	
	SocialArgs args;
	args.fn = 10;
	
	Date today = GetSysDate();
	if (range.len == 2) {
		int begin = range.off;
		int end = range.off + range.len;
		ASSERT(begin < end && end - begin < 100);
		for(int i = begin; i < end && i < by.images.GetCount(); i++) {
			BioImage& bi = by.images[i];
			Date by_date(by.year, today.month, today.day);
			String title = GetBiographyCategoryKey(t.bcat_i) +
				", year " + IntStr(by.year) +
				", age " + IntStr((by_date - p.owner->born) / 365) +
				", image #" + IntStr(i);
			if (bi.image_text.GetCount())
				args.parts.Add(title, bi.image_text);
		}
		if (args.parts.IsEmpty()) {
			NextBatch();
			return;
		}
		else if (args.parts.GetCount() == 1) {
			OnProcessSummarizeImageCategoryYear("(" + args.parts.GetKey(0) + ") " + args.parts[0]);
			return;
		}
	}
	else {
		int step = range.len / 2;
		int begin = range.off;
		int end = range.off + range.len;
		for(int i = begin; i < end; i+=step) {
			BioRange sub_range;
			sub_range.off = i;
			sub_range.len = range.len >> 1;
			int j = by.image_summaries.Find(sub_range);
			ASSERT(j >= 0);
			BioImage& bi = by.image_summaries[j];
			Date by_date(by.year, today.month, today.day);
			int from = sub_range.off;
			int to = sub_range.off + sub_range.len - 1;
			String title =
				GetBiographyCategoryKey(t.bcat_i) +
				", year " + IntStr(by.year) +
				", age " + IntStr((by_date - p.owner->born) / 365) +
				", images from #" + IntStr(from) + " to #" + IntStr(to);
				;
			if (bi.image_text.GetCount())
				args.parts.Add(title, bi.image_text);
		}
		if (args.parts.IsEmpty()) {
			NextBatch();
			return;
		}
		else if (args.parts.GetCount() == 1) {
			OnProcessSummarizeImageCategoryYear("(" + args.parts.GetKey(0) + ") " + args.parts[0]);
			return;
		}
	}
	
	SetWaiting(1);
	TaskMgr& m = AiTaskManager();
	m.GetSocial(args, THISBACK(OnProcessSummarizeImageCategoryYear));
}

void ImageBiographySummaryProcess::OnProcessSummarizeImageCategoryYear(String res) {
	const ImageSummaryTask& t = imgsum_tasks[batch];
	
	int j = t.by->image_summaries.Find(t.range);
	ASSERT(j >= 0);
	BioImage& bi = t.by->image_summaries[j];
	String& s = bi.image_text;
	
	s = TrimBoth(res);
	if (s.Left(1) == "\"") s = s.Mid(1);
	if (s.Right(1) == "\"") s = s.Left(s.GetCount()-1);
	
	if (j == t.by->image_summaries.GetCount()-1)
		t.by->image_text = s;
	
	NextBatch();
	SetWaiting(0);
}

void ImageBiographySummaryProcess::ProcessSummarizeImageBiography() {
	TODO
	#if 0
	Biography& biography = p.snap->data;
	
	if (batch >= BIOCATEGORY_COUNT) {
		NextPhase();
		return;
	}
	
	int bcat_i = batch;
	BiographyCategory& bcat = biography.GetAdd(*p.owner, bcat_i);
	bcat.RealizeSummaries();
	if (sub_batch >= bcat.summaries.GetCount()) {
		NextBatch();
		return;
	}
	
	const BioRange& range = bcat.summaries.GetKey(sub_batch);
	BioYear& sum = bcat.summaries[sub_batch];
	
	if (skip_ready && sum.text.GetCount()) {
		NextSubBatch();
		return;
	}
	
	SocialArgs args;
	args.fn = 10;
	
	if (range.len == 2) {
		int begin = range.off;
		int end = range.off + range.len;
		ASSERT(begin < end && end - begin < 100);
		for(int i = begin; i < end; i++) {
			BioYear& by = bcat.GetAdd(i);
			String title = GetBiographyCategoryKey(bcat_i) +
				", year " + IntStr(by.year) +
				", age " + IntStr(by.year - p.owner->year_of_birth);
			if (by.image_text.GetCount())
				args.parts.Add(title, by.image_text);
		}
		if (args.parts.IsEmpty()) {
			NextSubBatch();
			return;
		}
		else if (args.parts.GetCount() == 1) {
			TODO //OnProcessSummarize("(" + args.parts.GetKey(0) + ") " + args.parts[0]);
			return;
		}
	}
	else {
		int step = range.len / 2;
		int begin = range.off;
		int end = range.off + range.len;
		for(int i = begin; i < end; i+=step) {
			BioRange sub_range;
			sub_range.off = i;
			sub_range.len = range.len >> 1;
			int j = bcat.summaries.Find(sub_range);
			ASSERT(j >= 0);
			BioYear& by = bcat.summaries[j];
			int from = sub_range.off;
			int to = sub_range.off + sub_range.len - 1;
			String title =
				GetBiographyCategoryKey(bcat_i) +
				", from year " + IntStr(from) +
				" to year " + IntStr(to) +
				", age " + IntStr(from - p.owner->year_of_birth) + " - " + IntStr(to - p.owner->year_of_birth)
				;
			if (by.image_text.GetCount())
				args.parts.Add(title, by.image_text);
		}
		if (args.parts.IsEmpty()) {
			NextSubBatch();
			return;
		}
		else if (args.parts.GetCount() == 1) {
			TODO //OnProcessSummarize("(" + args.parts.GetKey(0) + ") " + args.parts[0]);
			return;
		}
	}
	
	SetWaiting(1);
	TaskMgr& m = AiTaskManager();
	m.GetSocial(args, THISBACK(OnProcessSummarizeImageBiography));
	#endif
}

void ImageBiographySummaryProcess::OnProcessSummarizeImageBiography(String res) {
	TODO
	#if 0
	Biography& biography = p.snap->data;
	int bcat_i = batch;
	BiographyCategory& bcat = biography.GetAdd(*p.owner, bcat_i);
	const BioRange& range = bcat.summaries.GetKey(sub_batch);
	BioYear& sum = bcat.summaries[sub_batch];
	
	sum.image_text = TrimBoth(res);
	
	
	NextSubBatch();
	SetWaiting(0);
	#endif
}


END_UPP_NAMESPACE
