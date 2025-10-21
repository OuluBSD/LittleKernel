#ifndef _ide_Vfs_MetaCodeCtrl_h_
#define _ide_Vfs_MetaCodeCtrl_h_

struct MetaCodeCtrl : ParentCtrl {
	Splitter			hsplit, rsplit;
	TabCtrl				tabs;
	CodeEditor			editor;
	ArrayCtrl			depthfirst;
	MetaProcessCtrl		process;
	ArrayCtrl			cursorinfo;

	String				filepath;
	String				includes;
	VfsSrcPkg*			pkg = 0;
	int					lineh = 24;
	Font				fnt;
	String				content;
	//int					sel_line = -1;
	//SourceRange*		sel_ann_f = 0;
	//AiAnnotationItem*	sel_ann = 0;
	//VfsSrcFile*		sel_f = 0;
	Color				clr_sel;
	Color				clr_ann;
	byte				charset = 0;
	CodeArgs			args;
	int					prev_editor_cursor = -1;
	TimeCallback		tc;
	MetaCodeGenerator	gen;
	const MetaCodeGenerator::File* gen_file = 0;
	Ptr<VfsValue>		sel_node = 0;
	Vector<int>			editor_to_line;
	Vector<VfsValue*>	comment_to_node;
	
	typedef MetaCodeCtrl CLASSNAME;
	MetaCodeCtrl();
	void OnTab();
	void SetFont(Font fnt);
	void Load(const String& includes, String filename, Stream& str, byte charset);
	void Save(Stream& str, byte charset);
	void SetEditPos(LineEdit::EditPos pos);
	void SetPickUndoData(LineEdit::UndoData pos);
	LineEdit::UndoData PickUndoData();
	LineEdit::EditPos GetEditPos();
	void StoreMetaFile();
	void SetSelectedLineFromEditor();
	void ContextMenu(Bar& bar);
	void AddComment();
	void RemoveComment();
	void UpdateEditor();
	Vector<String> GetAnnotationAreaCode();
	void MakeAiComments();
	void RunTask(MetaProcess::FnType t);
	void CheckEditorCursor();
	void OnEditorCursor();
	void AnnotationData();
	void VisitCursorInfo(VfsValue& n, int& row);
};

#endif
