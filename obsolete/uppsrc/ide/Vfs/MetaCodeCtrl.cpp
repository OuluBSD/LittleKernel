#include "Vfs.h"
#include <ide/ide.h>

#ifndef flagV1
#include <AI/Core/Core.h>
#endif

NAMESPACE_UPP

MetaCodeCtrl::MetaCodeCtrl()
{
	Add(hsplit.SizePos());
	hsplit.Horz() << editor << tabs;
	
	tabs.Add(rsplit.SizePos(), "Code");
	tabs.Add(process.SizePos(), "Process");
	tabs.WhenSet = THISBACK(OnTab);
	
	editor.Highlight("cpp");
	editor.LineNumbers(true);
	editor.SetReadOnly();
	editor.WhenBar << THISBACK(ContextMenu);
	editor.WhenSel << THISBACK(CheckEditorCursor);

	rsplit.Vert() << cursorinfo << depthfirst;
	
	cursorinfo.AddColumn("Kind");
	cursorinfo.AddColumn("Id");
	cursorinfo.AddColumn("Type");
	cursorinfo.AddColumn("Pos");
	cursorinfo.AddColumn("Ref-pos");
	cursorinfo.ColumnWidths("2 4 4 1 1");
	
	depthfirst.AddColumn("File");
	depthfirst.AddColumn("Pos");
	depthfirst.AddColumn("Kind");
	depthfirst.AddColumn("Id");
	depthfirst.AddColumn("Type");
	depthfirst.AddColumn("Ref-pos");
	depthfirst.ColumnWidths("1 1 2 4 4 1");
	
}

void MetaCodeCtrl::SetFont(Font fnt) { editor.SetFont(fnt); }

void MetaCodeCtrl::Load(const String& includes, String filename, Stream& str, byte charset)
{
	this->filepath = NormalizePath(filename);
	this->includes = includes;
	
	IdeMetaEnv().Load(includes, filepath);

	this->content = str.Get((int)str.GetSize());
	this->charset = charset;

	UpdateEditor();
}

void MetaCodeCtrl::UpdateEditor()
{
	auto& ienv = IdeMetaEnv();
	VfsSrcFile& file = ienv.ResolveFile(this->includes, this->filepath);
	VfsSrcPkg& pkg = *file.pkg;
	hash_t pkg_hash = pkg.GetPackageHash();
	hash_t file_hash = pkg.GetFileHash(this->filepath);
	ASSERT(pkg_hash >= 0 && file_hash >= 0);
	VfsValueSubset sub;
	ienv.SplitValueHash(ienv.env.root, sub, pkg_hash, file_hash);
	
	gen.Process(sub);
	gen_file = gen.GetResultFile(pkg_hash, file_hash);
	
	String code;
	if (gen_file) {
		code = gen_file ? gen_file->code : String();
		
		editor_to_line <<= gen_file->editor_to_line;
		comment_to_node <<= gen_file->comment_to_node;
	}

	Point scroll_pos = editor.GetScrollPos();
	int cursor = editor.GetCursor();
	
	editor.Set(code, charset);
	editor.SetScrollPos(scroll_pos);
	editor.SetCursor(cursor);
}

void MetaCodeCtrl::Save(Stream& str, byte charset)
{
	str.Put(this->content);
	// VfsSrcPkg& aion = VfsSrcPkgs().GetAdd(aion_path);
	// aion.Save();
}

void MetaCodeCtrl::SetEditPos(LineEdit::EditPos pos) {}

void MetaCodeCtrl::SetPickUndoData(LineEdit::UndoData pos) {}

LineEdit::UndoData MetaCodeCtrl::PickUndoData() { return LineEdit::UndoData(); }

LineEdit::EditPos MetaCodeCtrl::GetEditPos() { return LineEdit::EditPos(); }

void MetaCodeCtrl::ContextMenu(Bar& bar)
{
	bar.Separator();
	bar.Add("Add comment", THISBACK(AddComment));
	bar.Add("Remove comment", THISBACK(RemoveComment));
	bar.Separator();
	//bar.Sub("AI", [&](Bar& bar) {
	#ifndef flagV1
	bar.Add("Create AI comments for this scope", THISBACK(MakeAiComments));
	#endif
	bar.Add("Run base analysis for this scope", THISBACK1(RunTask, MetaProcess::FN_BASE_ANALYSIS));
	//});
}

void MetaCodeCtrl::AddComment()
{
	SetSelectedLineFromEditor();
	if(!sel_node)
		return;
	const AstValue* a = *sel_node;
	if (!a) {
		PromptOK("Error: not an AstValue");
		return;
	}
	int sel_line = editor.GetCursorLine();
	int origl = editor_to_line[sel_line];
	int l = origl - a->begin.y;
	String txt;
	if(!EditText(txt, "Add comment", ""))
		return;
	VfsValue& cn = sel_node->Add();
	AstValue& ca = cn;
	ca.kind = METAKIND_COMMENT;
	ca.end = Point(0,origl);
	ca.begin = Point(0,origl);
	cn.id = txt;
	cn.file_hash = sel_node->file_hash;
	cn.pkg_hash = sel_node->pkg_hash;
	//sel_ann->Sort();
	StoreMetaFile();
	UpdateEditor();
}

void MetaCodeCtrl::RemoveComment()
{
	SetSelectedLineFromEditor();
	int sel_line = editor.GetCursorLine();
	VfsValue* c = sel_line < comment_to_node.GetCount() ? comment_to_node[sel_line] : 0;
	if (c) {
		c->Destroy();
		StoreMetaFile();
		UpdateEditor();
	}
}

Vector<String> MetaCodeCtrl::GetAnnotationAreaCode() {
	const AstValue* a = *sel_node;
	ASSERT(a);
	if (!a) return Vector<String>();
	return GetStringArea(this->content, a->begin, a->end);
}

#ifndef flagV1
void MetaCodeCtrl::MakeAiComments()
{
	SetSelectedLineFromEditor();
	if(!sel_node) {
		PromptOK(DeQtf("No line selected"));
		return;
	}
	
	TaskMgr& m = AiTaskManager();
	args.Clear();
	args.fn = CodeArgs::SCOPE_COMMENTS;
	args.lang = "C++";
	args.code = GetAnnotationAreaCode();

	auto cur_sel_node = sel_node;
	const AstValue* ast = *cur_sel_node;

	m.GetCode(args, [&, cur_sel_node, ast](String result) {
		Vector<String> lines = Split(result, "\n");
		VectorMap<int, String> comments;
		for(String& l : lines) {
			int a = l.Find("line #");
			if(a < 0)
				continue;
			l = l.Mid(a + 6);
			int line = ScanInt(l); // Get the number of the line
			a = l.Find(":");
			if(a < 0)
				continue;
			l = TrimBoth(l.Mid(a + 1)); // Get the answer only
			if(l.IsEmpty())
				continue;
			if(l[0] == '\"') // Remove possible quotes
				l = l.Mid(1, l.GetCount() - 2);
			a = l.Find("//");
			if(a >= 0)
				l = TrimLeft(l.Mid(a + 2));
			comments.GetAdd(line) = l;
		}
		// DUMPM(comments);
		cur_sel_node->AstRemoveAllDeep(METAKIND_COMMENT);
		for(auto c : ~comments) {
			Point pt = ast->begin;
			pt.y += c.key;
			//VfsValue* closest = cur_sel_node->FindClosest(pt);
			VfsValue& cn = cur_sel_node->Add();
			cn.id = c.value;
			cn.file_hash = cur_sel_node->file_hash;
			cn.pkg_hash = cur_sel_node->pkg_hash;
			AstValue& ast = cn;
			ast.kind = METAKIND_COMMENT;
			ast.begin = ast.end = pt;
		}
		//cur_sel_node->Sort();
		
		Ptr<Ctrl> p = this; //static_cast<Pte<MetaCodeCtrl>*>(this);
		PostCallback([p] {
			if (p) {
				MetaCodeCtrl* c = dynamic_cast<MetaCodeCtrl*>(&*p);
				if (c) {
					c->StoreMetaFile();
					c->UpdateEditor();
				}
			}
		});
	});
	
}
#endif

void MetaCodeCtrl::RunTask(MetaProcess::FnType t) {
	SetSelectedLineFromEditor();
	if(!sel_node) {
		PromptOK(DeQtf("No node selected"));
		return;
	}
	auto code = GetAnnotationAreaCode();
	auto& codeidx = CodeIndex();
	int i = codeidx.Find(filepath);
	if (!sel_node || i < 0) {
		PromptOK(DeQtf("Error: no pointers found"));
		return;
	}
	FileAnnotation& fa = codeidx[i];
	
	process.RunTask(this->filepath, *sel_node, pick(code), MetaProcess::FN_BASE_ANALYSIS);
	
	tabs.Set(1);
}

void MetaCodeCtrl::OnTab() {
	int tab = tabs.Get();
	
	if (tab == 1) {
		tc.Set(-500, [this]{
			process.Data();
		});
	}
	else {
		tc.Kill();
	}
}

void MetaCodeCtrl::StoreMetaFile()
{
	auto& ienv = IdeMetaEnv();
	VfsSrcFile& file = ienv.ResolveFile(this->includes, this->filepath);
	if (file.managed_file) {
		file.MakeTempFromEnv(false);
		file.Store(true);
	}
	VfsSrcFile& mfile = file.pkg->GetMetaFile();
	ASSERT(mfile.managed_file);
	mfile.MakeTempFromEnv(true);
	mfile.Store(true);
}

void MetaCodeCtrl::SetSelectedLineFromEditor()
{
	int64 pos = editor.GetCursor64();
	Point pt;
	int sel_line = editor.GetCursorLine();
	int origl = editor_to_line[sel_line];
	pt.y = origl;
	pt.x = (int)(pos - editor.GetPos(sel_line));
	for (auto n : ~gen_file->code_nodes) {
		if (n.key.Contains(pt)) {
			VfsValue& sel = *n.value;
			ASSERT(!sel.only_temporary);
			this->sel_node = &sel;
			return;
		}
	}
	this->sel_node = 0;
}

void MetaCodeCtrl::CheckEditorCursor() {
	int cur = editor.GetCursor();
	if (cur != prev_editor_cursor) {
		prev_editor_cursor = cur;
		OnEditorCursor();
	}
}

void MetaCodeCtrl::OnEditorCursor() {
	if (!gen_file)
		return;
	SetSelectedLineFromEditor();
	AnnotationData();
}

void MetaCodeCtrl::VisitCursorInfo(VfsValue& n, int& row) {
	const AstValue* a = n;
	cursorinfo.Set(row, 0, a ? VfsValue::AstGetKindString(a->kind) : String());
	cursorinfo.Set(row, 1, n.id);
	cursorinfo.Set(row, 2, n.GetTypeString());
	cursorinfo.Set(row, 3, a ? a->begin : Null);
	VfsValue* decl = a->is_ref ? IdeMetaEnv().FindDeclaration(n) : 0;
	if (decl) {
		const AstValue* decl_a = *decl;
		cursorinfo.Set(row, 4, decl_a->begin);
	}
	else if (a && a->is_ref)
		cursorinfo.Set(row, 4, "<decl not found>");
	else
		cursorinfo.Set(row, 4, Value());
	row++;
	
	for (auto& s : n.sub)
		VisitCursorInfo(s, row);
}

void MetaCodeCtrl::AnnotationData() {
	auto& codeidx = CodeIndex();
	int i = codeidx.Find(filepath);
	
	if (!sel_node || i < 0) {
		cursorinfo.Clear();
		return;
	}
	
	int row = 0;
	VisitCursorInfo(*sel_node, row);
	cursorinfo.SetCount(row);
	
	CodeVisitor vis;
	vis.SetLimit(1000);
	vis.Begin();
	vis.Visit(filepath, *sel_node);
	
	row = 0;
	for(const auto& it : vis.export_items) {
		depthfirst.Set(row, 0, it.file);
		depthfirst.Set(row, 1, it.pos);
		if (it.node) {
			VfsValue& n = *it.node;
			depthfirst.Set(row, 2, n.AstGetKindString());
			depthfirst.Set(row, 3, n.id);
			depthfirst.Set(row, 4, n.GetTypeString());
			if (it.link_node) {
				const AstValue* a1 = *it.link_node;
				depthfirst.Set(row, 5, a1 ? a1->begin.ToString() : String());
			}
			else
				depthfirst.Set(row, 5, Value());
			depthfirst.Set(row, 6, it.error);
			row++;
		}
		else {
			depthfirst.Set(row, 2, Value());
			depthfirst.Set(row, 3, Value());
			depthfirst.Set(row, 4, Value());
			depthfirst.Set(row, 5, Value());
			depthfirst.Set(row, 6, Value());
			depthfirst.Set(row, 7, it.error);
			row++;
		}
	}
	depthfirst.SetCount(row);
}

END_UPP_NAMESPACE

