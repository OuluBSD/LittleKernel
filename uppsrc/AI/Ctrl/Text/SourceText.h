#ifndef _AI_TextCtrl_SourceText_h_
#define _AI_TextCtrl_SourceText_h_

NAMESPACE_UPP

class AuthorDataCtrl : public ParentCtrl {
public:
	typedef enum {
		TEXT,
		STRUCTURED,
		MIXED,
	} Type;
	
private:
	DatasetProvider& o;
	Splitter vsplit, hsplit;
	ArrayCtrl entities, components; //, active_components;
	DocEdit scripts, analysis;
	Type data_type = TEXT;
	
public:
	typedef AuthorDataCtrl CLASSNAME;
	AuthorDataCtrl(DatasetProvider& o);

	void Data();
	void DataEntity();
	void DataExtension();
	void SetFont(Font fnt);
	void SetDataType(Type i) {data_type = i;}
	void SetStructured() {data_type = STRUCTURED;}
	void SetMixed() {data_type = MIXED;}
};


class ScriptTextDebuggerPage : public ParentCtrl {
	DatasetProvider& o;
	TabCtrl tabs;
	ArrayCtrl tokens, word_classes, words;
	ArrayCtrl token_texts, virtual_phrases, virtual_phrase_parts, virtual_phrase_structs;
	ArrayCtrl phrase_parts, struct_part_types, struct_types, simple_attrs;
	ArrayCtrl element_keys, attrs, actions;
	ArrayCtrl parallel, trans, action_phrases, wordnets;
public:
	typedef ScriptTextDebuggerPage CLASSNAME;
	ScriptTextDebuggerPage(DatasetProvider& o);
	
	void Data();
};

// TODO rename
class TokensPage : public ParentCtrl {
	DatasetProvider& o;
	Splitter hsplit;
	ArrayCtrl tokens;
	
public:
	typedef TokensPage CLASSNAME;
	TokensPage(DatasetProvider& o);
	
	void Data();
};


class TextElements : public ParentCtrl {
	DatasetProvider& o;
	ArrayCtrl list;
	
public:
	typedef TextElements CLASSNAME;
	TextElements(DatasetProvider& o);
	
	void Data();
};

// TODO rename
class TextDataWords : public ParentCtrl {
	DatasetProvider& o;
	Splitter hsplit, vsplit;
	ArrayCtrl colors, words;
	bool disabled = false;
	bool batch = false;
	
public:
	typedef TextDataWords CLASSNAME;
	TextDataWords(DatasetProvider& o);
	
	void EnableAll();
	void DisableAll();
	void Data();
	void DataColor();
	void DumpWordGroups();
	void DumpPhoneticChars();
};

// TODO rename
class TokenPhrases : public ParentCtrl {
	DatasetProvider& o;
	Splitter hsplit;
	ArrayCtrl texts;
	
public:
	typedef TokenPhrases CLASSNAME;
	TokenPhrases(DatasetProvider& o);
	
	void Data();
};

String GetTypePhraseString(const Vector<int>& word_classes, const SrcTextData& da);

// TODO rename
class VirtualPhrases : public ParentCtrl {
	DatasetProvider& o;
	Splitter vsplit, hsplit;
	ArrayCtrl texts, parts;
	
public:
	typedef VirtualPhrases CLASSNAME;
	VirtualPhrases(DatasetProvider& o);
	
	void Data();
};

// TODO rename
class VirtualPhraseParts : public ParentCtrl {
	DatasetProvider& o;
	Splitter vsplit, hsplit;
	ArrayCtrl texts, parts;
	
public:
	typedef VirtualPhraseParts CLASSNAME;
	VirtualPhraseParts(DatasetProvider& o);
	
	void Data();
};

// TODO rename
class VirtualPhraseStructs : public ParentCtrl {
	DatasetProvider& o;
	Splitter vsplit, hsplit;
	ArrayCtrl texts, parts;
	
public:
	typedef VirtualPhraseStructs CLASSNAME;
	VirtualPhraseStructs(DatasetProvider& o);
	
	void Data();
};

struct ScoreDisplay : public Display {
	virtual void Paint(Draw& w, const Rect& r, const Value& q,
	                   Color ink, Color paper, dword style) const;
};

// TODO rename
class PhrasePartAnalysis : public ParentCtrl {
	DatasetProvider& o;
	Splitter vsplit, hsplit;
	ArrayCtrl attrs, colors, actions, action_args, parts;
	
	VectorMap<String, VectorMap<String, int>> uniq_acts;
	
public:
	typedef PhrasePartAnalysis CLASSNAME;
	PhrasePartAnalysis(DatasetProvider& o);
	
	void Data();
	void DataMain();
	void DataAttribute();
	void DataColor();
	void DataAction();
	void DataActionHeader();
	void UpdateCounts();
};

// TODO rename
class PhrasePartAnalysis2 : public ParentCtrl {
	DatasetProvider& o;
	Splitter vsplit, hsplit;
	ArrayCtrl elements, typecasts, contrasts, parts,  colors;
	
public:
	typedef PhrasePartAnalysis2 CLASSNAME;
	PhrasePartAnalysis2(DatasetProvider& o);
	
	void Data();
	void DataMain();
	void DataElement();
	void DataTypeclass();
	void DataContrast();
	void DataColor();
	void UpdateCounts();
	void ClearAll();
};

// TODO rename
class ActionAttrsPage : public ParentCtrl {
	DatasetProvider& o;
	Splitter vsplit, hsplit;
	ArrayCtrl attrs, colors, actions;
	
public:
	typedef ActionAttrsPage CLASSNAME;
	ActionAttrsPage(DatasetProvider& o);
	
	void Data();
	void DataAttribute();
	void DataColor();
	void UpdateFromCache();
};

// TODO rename
class Attributes : public ParentCtrl {
	DatasetProvider& o;
	Splitter hsplit, vsplit;
	ArrayCtrl groups, values, pos_values, neg_values;
	
	VectorMap<String,Index<String>> uniq_attrs;
	VectorMap<String,Index<int>> uniq_attrs_i;
	
	void RealizeTemp();
	
public:
	typedef Attributes CLASSNAME;
	Attributes(DatasetProvider& o);
	
	void Data();
	void DataGroup();
};

// TODO rename
class TextDataDiagnostics : public ParentCtrl {
	DatasetProvider& o;
	Splitter hsplit;
	ArrayCtrl values;
	
public:
	typedef TextDataDiagnostics CLASSNAME;
	TextDataDiagnostics(DatasetProvider& o);
	
	void Data();
};

class SourceTextMergerCtrl : public TabCtrl {
	DatasetProvider& o;
	WithMergerLayout<Ctrl> conf;
	
public:
	typedef SourceTextMergerCtrl CLASSNAME;
	SourceTextMergerCtrl(DatasetProvider& o);
	
	void Data();
	void Do(int fn);
};

class TextDataWordnet : public ParentCtrl {
	DatasetProvider& o;
	Splitter vsplit, hsplit;
	ArrayCtrl attrs, colors, wordnets;
	Mutex lock;
	String tmp_first_line;
	
public:
	typedef TextDataWordnet CLASSNAME;
	TextDataWordnet(DatasetProvider& o);
	
	void EnableAll();
	void DisableAll();
	void Data();
	void DataMain();
	void DataAttribute();
	void DataColor();
	void ToolMenu(Bar& bar);
	
	void ToggleGettingColorAlternatives();
	void GetColorAlternatives(int batch_i);
	void OnColorAlternatives(String res, int batch_i);
	
	void DoWordnet(int fn);
	
};

// TODO rename
class PhraseParts : public ParentCtrl {
	DatasetProvider& o;
	Splitter vsplit, hsplit;
	ArrayCtrl texts, parts;
	
public:
	typedef PhraseParts CLASSNAME;
	PhraseParts(DatasetProvider& o);
	
	void Data();
	void ToolMenu(Bar& bar);
	void DoWords(int fn);
	void DoWordsUsingExisting(int fn);
	
};

class ActionParallelsPage : public ParentCtrl {
	DatasetProvider& o;
	Splitter vsplit, hsplit;
	ArrayCtrl actions, action_args, parallels;
	Mutex lock;
	
	VectorMap<String, VectorMap<String, int>> uniq_acts;
	
public:
	typedef ActionParallelsPage CLASSNAME;
	ActionParallelsPage(DatasetProvider& o);
	
	void Data();
	void ToolMenu(Bar& bar);
	void DataMain();
	void DataAction();
	void DataActionHeader();
	void UpdateParallels();
	
};

class ActionTransitionsPage : public ParentCtrl {
	DatasetProvider& o;
	Splitter vsplit, hsplit;
	ArrayCtrl actions, action_args, transitions;
	Mutex lock;
	
	VectorMap<String, VectorMap<String, int>> uniq_acts;
	
public:
	typedef ActionTransitionsPage CLASSNAME;
	ActionTransitionsPage(DatasetProvider& o);
	
	void Data();
	void ToolMenu(Bar& bar);
	void DataMain();
	void DataAction();
	void DataActionHeader();
	void UpdateTransitions();
	
};

class SourceTextCtrl : public ToolAppCtrl {
	DropList				data_type;
	AuthorDataCtrl			src;
	TokensPage				tk;
	VirtualPhrases			vp;
	VirtualPhraseParts		vpp;
	VirtualPhraseStructs	vps;
	PhrasePartAnalysis		vpa;
	PhrasePartAnalysis2		vpa2;
	ActionAttrsPage			aap;
	Attributes				att;
	TextDataDiagnostics		diag;
	TextDataWordnet			wn;
	PhraseParts				pp;
	ActionParallelsPage		apar;
	ActionTransitionsPage	atra;
	SourceTextMergerCtrl	merger;
	
	void Load();
public:
	typedef SourceTextCtrl CLASSNAME;
	SourceTextCtrl();
	
	void SetFont(Font fnt);
	void Data() override;
	void ToolMenu(Bar& bar) override;
	void Visit(Vis& v) override;
	bool Load(const String& includes, const String& filename, Stream& in, byte charset) override;
	void Do(int fn);
	void SetDataCtrl();
	
	static bool AcceptsExt(String e) { return e == ".db-src"; }
	static String GetID() { return "SourceText"; }
	
	Event<> WhenSaveEditPos;
};

END_UPP_NAMESPACE

#endif
 
