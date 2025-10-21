#include "Biography.h"
#define REF(tab, obj) auto& obj = tab.obj;

NAMESPACE_UPP


void BiographyCtrl::El_Ctor() {
	REF(el, vsplit);
	REF(el, elements);
	REF(el, block);
	REF(el, sort_column);
	
	tabs.Add(vsplit.SizePos(), "Elements");
	
	vsplit.Vert() << elements << block;
	vsplit.SetPos(3333);
	
	CtrlLayout(block);
	
	elements.AddColumn(t_("Year"));
	elements.AddColumn(t_("Element"));
	elements.AddColumn(t_("Description"));
	for(int i = 0; i < SCORE_COUNT; i++)
		elements.AddColumn(ScoreTitles[i]);
	elements.AddColumn(t_("Score"));
	elements.AddIndex("CAT");
	elements.AddIndex("YEAR");
	elements.ColumnWidths("1 3 8 1 1 1 1 1 1 1 1 1 1 1");
	elements <<= THISBACK(El_DataElement);
	sort_column = 3 + SCORE_COUNT;
	
	block.keywords <<= THISBACK(El_OnValueChange);
	block.native_text <<= THISBACK(El_OnValueChange);
	block.text <<= THISBACK(El_OnValueChange);
	
	block.elements.AddColumn("Key");
	block.elements.AddColumn("Value");
	block.elements.AddColumn("Score");
	block.elements.ColumnWidths("2 12 1");
	
	
}

void BiographyCtrl::El_DataCategory() {
	REF(el, vsplit);
	REF(el, elements);
	REF(el, block);
	REF(el, sort_column);
	
	DatasetPtrs mp; GetDataset(mp);
	if (!mp.owner || !mp.biography || !categories.IsCursor()) {
		elements.Clear();
		return;
	}
	Owner& owner = *mp.owner;
	Biography& biography = GetExt<Biography>();
	int cat_i = categories.Get("IDX");
	int row = 0;
	
	// Dump elements
	if (0) {
		LOG("BEGIN");
		Index<String> elements;
		for (const auto& a : biography.AllCategories()) {
			for (const auto& by : a.years) {
				for (const auto& el : by.elements) {
					elements.FindAdd(el.key);
				}
			}
		}
		SortIndex(elements, StdLess<String>());
		for(int i = 0; i < elements.GetCount(); i++) {
			LOG("v.Add(\"" + elements[i] + "\");");
		}
	}
	
	for (int cat_iter = 0; cat_iter < BIOCATEGORY_COUNT; cat_iter++) {
		if (cat_i >= 0 && cat_iter != cat_i)
			continue;
		
		BiographyCategory& bcat = biography.GetAdd(owner, cat_iter);
		
		for(int i = 0; i < bcat.years.GetCount(); i++) {
			const BioYear& by = bcat.years[i];
			
			for(int j = 0; j < by.elements.GetCount(); j++) {
				const BioYear::Element& el = by.elements[j];
				
				elements.Set(row, 0, by.year);
				elements.Set(row, 1, el.key);
				elements.Set(row, 2, el.value);
				elements.Set(row, "CAT", cat_iter);
				elements.Set(row, "YEAR", i);
				
				double sum = 0;
				for(int k = 0; k < SCORE_COUNT; k++) {
					elements.Set(row, 3+k, el.scores[k]);
					sum += el.scores[k];
				}
				if (sum == 0.0)
					continue;
				double av = sum / SCORE_COUNT;
				if (av > 10.0)
					av = 0;
				
				elements.Set(row, 3+SCORE_COUNT, av);
				row++;
			}
		}
	}
	
	INHIBIT_CURSOR(elements);
	elements.SetCount(row);
	elements.SetSortColumn(sort_column, true);
	if (elements.GetCount() && !elements.IsCursor())
		elements.SetCursor(0);
	
	El_DataElement();
}

void BiographyCtrl::El_DataElement() {
	REF(el, vsplit);
	REF(el, elements);
	REF(el, block);
	REF(el, sort_column);
	
	DatasetPtrs mp; GetDataset(mp);
	if (!mp.owner || !mp.biography || !elements.IsCursor())
		return;
	int cat_i = elements.Get("CAT");
	int year_i = elements.Get("YEAR");
	
	Biography& biography = GetExt<Biography>();
	BiographyCategory& bcat = biography.GetAdd(*mp.owner, cat_i);
	const BioYear& by = bcat.years[year_i];
	
	block.year.SetData(by.year);
	block.keywords.SetData(by.keywords);
	block.native_text.SetData(by.native_text);
	block.text.SetData(by.text);
	
	for(int i = 0; i < by.elements.GetCount(); i++) {
		block.elements.Set(i, 0, Capitalize(by.elements[i].key));
		block.elements.Set(i, 1, by.elements[i].value);
		block.elements.Set(i, 2, by.elements[i].GetAverageScore());
	}
	block.elements.SetCount(by.elements.GetCount());
	block.elements.SetSortColumn(2, true);
	
}

void BiographyCtrl::El_OnValueChange() {
	
}

void BiographyCtrl::El_ToolMenu(Bar& bar) {
	
}


END_UPP_NAMESPACE
#undef REF
