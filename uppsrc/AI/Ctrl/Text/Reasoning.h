#ifndef _AI_Ctrl_Reasoning_h_
#define _AI_Ctrl_Reasoning_h_

NAMESPACE_UPP


class ScriptReasoningCtrl : public AiComponentCtrl {
	Splitter hsplit;
	ArrayCtrl itemlist;
	
	struct Item : Moveable<Item> {
		typedef enum : int {
			// Same as in DatabaseBrowser
			ATTR_GROUP,
			ATTR_VALUE,
			COLOR,
			ACTION,
			ACTION_ARG,
			ELEMENT,
			TYPECLASS,
			CONTENT,
			
			INSPIRATIONAL_TEXT,
			
			TYPE_COUNT
		} Type;
		
		Type type;
		String new_value, final_value, part;
		Color clr;
		int line = -1;
		
		static inline String TypeString(Type t) {
			switch (t) {
				case ATTR_GROUP:	return "Attribute-Group";
				case ATTR_VALUE:	return "Attribute-Value";
				case COLOR:			return "Metaphorical color";
				case ACTION:		return "Action-Key";
				case ACTION_ARG:	return "Action-Arg";
				case ELEMENT:		return "Element";
				case TYPECLASS:		return "Typeclass";
				case CONTENT:		return "Content";
				
				case INSPIRATIONAL_TEXT:	return "Inspirational text";
				default: return "<error>";
			}
		}
	};
	Vector<Item> items;
	DatabaseBrowser db;
	
	struct Cursor {
		String part;
		int line;
	};
	void AddItem(const Cursor& c, Item::Type type, String new_value, String final_value="", Color clr=White());
	void AddItem(const Cursor& c, DatabaseBrowser::ColumnType type, String new_value, String final_value="", Color clr=White());
	
public:
	typedef ScriptReasoningCtrl CLASSNAME;
	ScriptReasoningCtrl();
	
	void ToolMenu(Bar& bar) override;
	void Data() override;
	void MakeItems();
	bool MakeElementChange(const Cursor& c, const LineElement& cur, const LineElement& el);
	
};

INITIALIZE(ScriptReasoningCtrl)


END_UPP_NAMESPACE

#endif
 
