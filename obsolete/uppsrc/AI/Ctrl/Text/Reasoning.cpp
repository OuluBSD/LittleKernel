#include "Text.h"

NAMESPACE_UPP


ScriptReasoningCtrl::ScriptReasoningCtrl() {
	Add(itemlist.SizePos());
	
	itemlist.AddColumn("Part");
	itemlist.AddColumn("Line");
	itemlist.AddColumn("Type");
	itemlist.AddColumn("New value");
	itemlist.AddColumn("Final value");
	
}

void ScriptReasoningCtrl::ToolMenu(Bar& bar) {
	
}

void ScriptReasoningCtrl::Data() {
	MakeItems();
	
	for(int i = 0; i < items.GetCount(); i++) {
		Item& it = items[i];
		
		itemlist.Set(i, 0, it.part);
		itemlist.Set(i, 1, it.line);
		itemlist.Set(i, 2, Item::TypeString(it.type));
		itemlist.Set(i, 3, AttrText(it.new_value).NormalPaper(it.clr));
		itemlist.Set(i, 4, AttrText(it.final_value).NormalPaper(it.clr));
	}
	itemlist.SetCount(items.GetCount());
	
}

void ScriptReasoningCtrl::MakeItems() {
	items.Clear();
	DatasetPtrs p; GetDataset(p);
	if (!p.script)
		return;
	Script& s = *p.script;
	
	TODO
	#if 0
	Cursor cursor;
	for(int i = 0; i < s.parts.GetCount(); i++) {
		const DynPart& dp = s.parts[i];
		cursor.part = dp.GetName();
		cursor.line = 0;
		
		LineElement previous;
		previous.element = "All";
		previous.attr.group = "All";
		previous.attr.value = "All";
		previous.act.action = "All";
		previous.act.arg = "All";
		previous.clr_i = 0;
		previous.typeclass_i = 0;
		previous.con_i = 0;
		
		for(int j = 0; j < dp.sub.GetCount(); j++) {
			const DynSub& ds = dp.sub[j];
			
			for(int k = 0; k < ds.lines.GetCount(); k++) {
				const DynLine& dl = ds.lines[k];
				
				LineElement current = dp.el;
				current.Overlay(ds.el);
				current.Overlay(dl.el);
				
				if (MakeElementChange(cursor, previous, current)) {
					
				}
				
				AddItem(cursor, Item::INSPIRATIONAL_TEXT, dl.text, dl.user_text);
				
				Swap(previous, current);
				cursor.line++;
			}
		}
	}
	#endif
}

bool ScriptReasoningCtrl::MakeElementChange(const Cursor& cursor, const LineElement& cur, const LineElement& el) {
	int mode = max(0, DatabaseBrowser::FindMode(el.sorter));
	DatasetPtrs p;
	GetDataset(p);
	db.SetMode(p, mode);
	
	int prev_items_count = items.GetCount();
	
	for(int i = 0; i < DatabaseBrowser::TYPE_COUNT; i++) {
		DatabaseBrowser::ColumnType t = db.GetOrder(i);
		
		switch (t) {
			case DatabaseBrowser::TYPE_COUNT:
			case DatabaseBrowser::INVALID: break;
			
			case DatabaseBrowser::ELEMENT:
			if (cur.element != el.element) {
				AddItem(cursor, t, el.element);
			}
			break;
			
			case DatabaseBrowser::ATTR_GROUP:
			if (cur.attr.group != el.attr.group) {
				AddItem(cursor, t, el.attr.group);
			}
			break;
			
			case DatabaseBrowser::ATTR_VALUE:
			if (cur.attr.value != el.attr.value) {
				AddItem(cursor, t, el.attr.value);
			}
			break;
			
			case DatabaseBrowser::ACTION:
			if (cur.act.action != el.act.action) {
				AddItem(cursor, t, el.act.action);
			}
			break;
			
			case DatabaseBrowser::ACTION_ARG:
			if (cur.act.arg != el.act.arg) {
				AddItem(cursor, t, el.act.arg);
			}
			break;
			
			case DatabaseBrowser::COLOR:
			if (cur.clr_i != el.clr_i) {
				AddItem(cursor, t, GetColorString(el.clr_i), "", GetGroupColor(el.clr_i));
			}
			break;
			
			case DatabaseBrowser::TYPECLASS:
			TODO
			#if 0
			if (cur.typeclass_i != el.typeclass_i) {
				String tc  = el.typeclass_i >= 0 ? GetTypeclasses()[el.typeclass_i] : String();
				AddItem(cursor, t, tc);
			}
			#endif
			break;
			
			case DatabaseBrowser::CONTENT:
			TODO
			#if 0
			if (cur.con_i != el.con_i) {
				int con_i = el.con_i / 3;
				int con_mod = el.con_i % 3;
				String con = el.con_i >= 0 ?
					GetContents()[con_i].key + ": " + GetContents()[con_i].parts[con_mod] : String();
				AddItem(cursor, t, con);
			}
			#endif
			break;
		};
	}
	
	return prev_items_count < items.GetCount();
}

void ScriptReasoningCtrl::AddItem(const Cursor& c, Item::Type type, String new_value, String final_value, Color clr) {
	Item& it = items.Add();
	it.type = type;
	it.new_value = new_value;
	it.final_value = final_value;
	it.clr = clr;
	it.part = c.part;
	it.line = c.line;
}

void ScriptReasoningCtrl::AddItem(const Cursor& c, DatabaseBrowser::ColumnType type, String new_value, String final_value, Color clr) {
	Item& it = items.Add();
	it.type = (Item::Type)type; // same as in DatabaseBrowser
	it.new_value = new_value;
	it.final_value = final_value;
	it.clr = clr;
	it.part = c.part;
	it.line = c.line;
}




INITIALIZER_COMPONENT_CTRL(ScriptReasoning, ScriptReasoningCtrl)

END_UPP_NAMESPACE
