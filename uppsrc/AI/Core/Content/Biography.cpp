#include "Content.h"
#include <AI/Core/Marketing/Marketing.h>

NAMESPACE_UPP

void BioYear::RealizeImageSummaries() {
	if (images.IsEmpty()) return;
	int begin = 0;
	int last = images.GetCount()-1;
	
	for(int i = 1; i <= 5; i++) {
		int len = 1 << i;
		for(int j = 0; j < images.GetCount(); j += len) {
			int range_last = j + len - 1;
			if (i > 1 && range_last > last) // skip empty tail in largest range
				continue;
			BioImage& bi = GetAddImageSummary(j, len);
		}
		if (len >= images.GetCount())
			break;
	}
	SortByKey(image_summaries, BioRange());
}

BioImage& BioYear::GetAddImageSummary(int off, int len) {
	BioRange r;
	r.off = off;
	r.len = len;
	int i = image_summaries.Find(r);
	if (i >= 0)
		return image_summaries[i];
	BioImage& bi = image_summaries.Add(r);
	return bi;
}

int BioYear::FindElement(const String& key) const {
	int i = 0;
	for (const auto& e : elements) {
		if (e.key == key)
			return i;
		i++;
	}
	return -1;
}

double BioYear::Element::GetAverageScore() const {
	double score_sum = 0;
	int score_count = 0;
	for(int i = 0; i < SCORE_COUNT; i++) {
		int score = scores[i];
		score_sum += score;
		score_count++;
	}
	double score = score_sum / score_count;
	return score;
}

String BioYear::JoinElementMap(String delim0, String delim1) {
	String s;
	for (const BioYear::Element& e : elements) {
		s << e.key << delim0 << e.value << delim1;
	}
	return s;
}












void BiographyCategory::RealizeSummaries() {
	if (years.IsEmpty()) return;
	int begin_year = years[0].year;
	int last_year = years.Top().year;
	
	for(int i = 1; i <= 5; i++) {
		int len = 1 << i;
		for(int j = 0; j < years.GetCount(); j += len) {
			int range_last_year = years[j].year + len - 1;
			// When exceeding last year, allow only lowest range
			if (i > 1 && range_last_year > last_year)
				continue;
			BioYear& by = GetAddSummary(years[j].year, len);
		}
	}
	SortByKey(summaries, BioRange());
}

BioYear& BiographyCategory::GetAddSummary(int begin_year, int years) {
	BioRange r;
	r.off = begin_year;
	r.len = years;
	int i = summaries.Find(r);
	if (i >= 0)
		return summaries[i];
	BioYear& by = summaries.Add(r);
	by.year = begin_year + years - 1;
	return by;
}

int BiographyCategory::GetFilledCount() const {
	int c = 0;
	for (const BioYear& by : years)
		if (by.text.GetCount() || by.keywords.GetCount())
			c++;
	return c;
}

int BiographyCategory::GetFilledImagesCount() const {
	int c = 0;
	for (const BioYear& by : years)
		c += by.images.GetCount();
	return c;
}

BioYear& BiographyCategory::GetAdd(int year) {
	ASSERT(year >= -2000 && year < 3000);
	for (BioYear& by : years)
		if (by.year == year)
			return by;
	BioYear& by = years.Add();
	by.year = year;
	return by;
}












BiographyCategory& Biography::GetAdd(Owner& o, int enum_) {
	String s = GetBiographyCategoryEnum(enum_);
	BiographyCategory& bc = categories.GetAdd(s);
	Date today = GetSysDate();
	for(int i = o.born.year; i <= today.year; i++) {
		BioYear& by = bc.GetAdd(i);
	}
	return bc;
}

BiographyCategory* Biography::Find(Owner& o, int enum_) {
	String s = GetBiographyCategoryEnum(enum_);
	int i = categories.Find(s);
	if (i < 0) return 0;
	return &categories[i];
}

const BiographyCategory* Biography::Find(Owner& o, int enum_) const {
	String s = GetBiographyCategoryEnum(enum_);
	int i = categories.Find(s);
	if (i < 0) return 0;
	return &categories[i];
}

void Biography::ClearSummary() {
	for (BiographyCategory& bc : categories) {
		bc.summaries.Clear();
		bc.RealizeSummaries();
	}
}




String PhotoPrompt::GetFilePath(String dir, int i) const {
	dir = AppendFileName(dir, "images" DIR_SEPS "prompts");
	String filename = IntStr64(prompt.GetHashValue()) + "_" + IntStr(i) + ".jpg";
	String path = AppendFileName(dir, filename);
	return path;
}













INITIALIZER_COMPONENT(Biography, "ecs.private.biography", "Ecs|Private")
INITIALIZER_COMPONENT(BiographyPerspectives, "ecs.private.biography.perspectives", "Ecs|Private")

END_UPP_NAMESPACE
