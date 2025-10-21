#include "Social.h"

NAMESPACE_UPP


MarketplaceProcess::MarketplaceProcess() {
	
}

int MarketplaceProcess::GetPhaseCount() const {
	return PHASE_COUNT;
}

int MarketplaceProcess::GetBatchCount(int phase) const {
	const auto& s = GetMarketplaceSections();
	switch (phase) {
		case PHASE_DESCRIPTION:	return p.analysis->market_items.GetCount();
		default: return 1;
	}
}

int MarketplaceProcess::GetSubBatchCount(int phase, int batch) const {
	switch (phase) {
		case PHASE_DESCRIPTION:	return 1;
		default: return 1;
	}
}

void MarketplaceProcess::DoPhase() {
	switch (phase) {
		case PHASE_DESCRIPTION:	ProcessDescription(); return;
		default: NextPhase(); break;
	}
}

MarketplaceProcess& MarketplaceProcess::Get(DatasetPtrs p) {
	String t = p.owner->val.GetPath();
	hash_t h = t.GetHashValue();
	static ArrayMap<hash_t, MarketplaceProcess> map;
	int i = map.Find(h);
	if (i >= 0)
		return map[i];
	
	MarketplaceProcess& ls = map.Add(h);
	ls.p = p;
	return ls;
}

void MarketplaceProcess::MakeArgs(MarketplaceArgs& args) {
	MarketplaceItem& it = p.analysis->market_items[batch];
	args.map.Add("generic", it.generic);
	args.map.Add("brand", it.brand);
	args.map.Add("model", it.model);
	args.map.Add("price", DblStr(it.price) + "€");
	args.map.Add("faults", it.faults);
	args.map.Add("works", it.works);
	args.map.Add("condition", it.broken ? "broken" : (it.good ? "good" : "fair"));
	args.map.Add("title", it.title);
	args.map.Add("description", it.description);
	args.map.Add("other", it.other);
	if (it.year_of_manufacturing > 0)
		args.map.Add("year of manufacturing", IntStr(it.year_of_manufacturing));
	
	const auto& sects = GetMarketplaceSections();
	args.map.Add("category", sects.GetKey(it.category));
	args.map.Add("subcategory", sects[it.category][it.subcategory]);
}

void MarketplaceProcess::ProcessDescription() {
	if (batch >= p.analysis->market_items.GetCount()) {
		NextPhase();
		return;
	}
	
	MarketplaceArgs args;
	args.fn = 0;
	MakeArgs(args);
	args.map.RemoveKey("title");
	args.map.RemoveKey("description");
	MarketplaceItem& it = p.analysis->market_items[batch];
	int64 hash = args.map.GetHashValue();
	/*if (skip_ready && hash == it.input_hash && it.description.GetCount()) {
		NextBatch();
		return;
	}*/
	
	SetWaiting(1);
	TaskMgr& m = AiTaskManager();
	m.GetMarketplace(args, [this,hash](String res) {
		MarketplaceItem& it = p.analysis->market_items[batch];
		res = TrimBoth(res);
		RemoveQuotes(res);
		it.description = res;
		it.input_hash = hash;
		it.title.Clear();
		
		SetWaiting(0);
		NextBatch();
	});
}


END_UPP_NAMESPACE
