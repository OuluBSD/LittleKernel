#include "Vfs.h"


MetaEnvTree::MetaEnvTree() {
	AddFrame(menu);
	Add(split.SizePos());
	
	split.Horz() << lsplit << code;
	ltsplit.Horz() << pkgs << files;
	lsplit.Vert() << ltsplit << stmts << focus;
	lsplit.SetPos(2000,0);
	lsplit.SetPos(6000,1);
	
	code.Highlight("cpp");
	
	pkgs.AddColumn("#");
	pkgs.AddColumn("Package");
	pkgs.AddColumn("Directory");
	pkgs.AddIndex("HASH");
	pkgs.ColumnWidths("1 3 5");
	files.AddColumn("File");
	files.AddColumn("Count");
	files.AddIndex("HASH");
	
	pkgs.WhenCursor << THISBACK(DataPkg);
	files.WhenCursor << THISBACK(DataFile);
	stmts.WhenCursor << THISBACK(DataTreeSelection);
	focus.WhenCursor << THISBACK(DataFocusSelection);
	
	menu.Set([this](Bar& b) {
		b.Add("Update", THISBACK(Data)).Key(K_Q);
	});
	PostCallback(THISBACK(Data));
}

void MetaEnvTree::RefreshFiles() {
	IdeMetaEnvironment& env = IdeMetaEnv();
	
	// Get all pkg and file hashs (and counts of them)
	env.env.root.FindFiles(pkgfiles);
	
	// Sort values
	for (auto it : ~pkgfiles) {
		SortByValue(it.value, StdGreater<int>());
	}
	struct Sorter {
		bool operator()(const VectorMap<hash_t,int>& a, const VectorMap<hash_t,int>& b) const {
			int as = 0, bs = 0;
			for (int i : a.GetValues()) as += i;
			for (int i : b.GetValues()) bs += i;
			return as > bs;
		}
	};
	Sort(pkgfiles, Sorter());
}

void MetaEnvTree::Data() {
	IdeMetaEnvironment& env = IdeMetaEnv();
	
	if (pkgfiles.IsEmpty())
		RefreshFiles();
	
	int row = 0;
	pkgs.Set(row, 0, "<global>");
	pkgs.Set(row, "HASH", 0);
	row++;
	for(int i = 0; i < pkgfiles.GetCount(); i++) {
		hash_t h = pkgfiles.GetKey(i);
		if (h != 0) {
			VfsSrcPkg& pkg = env.GetAddPackage(h);
			AttrText dir(pkg.dir);
			dir.NormalInk(GrayColor());
			pkgs.Set(row, 1, pkg.GetTitle());
			pkgs.Set(row, 2, dir);
		}
		else {
			pkgs.Set(row, 1, "");
			pkgs.Set(row, 2, "");
		}
		pkgs.Set(row, 0, i);
		pkgs.Set(row, "HASH", (int64)h);
		row++;
	}
	pkgs.SetCount(row);
	
	if (!pkgs.IsCursor() && pkgs.GetCount())
		pkgs.SetCursor(0);
	else
		DataPkg();
}

void MetaEnvTree::DataPkg() {
	IdeMetaEnvironment& env = IdeMetaEnv();
	
	if (!pkgs.IsCursor()) {
		files.Clear();
		stmts.Clear();
		focus.Clear();
		code.Clear();
		return;
	}
	int row = 0;
	files.Set(row,0,"<global>");
	files.Set(row,"HASH",0);
	row++;
	hash_t pkg_hash = (int64)pkgs.Get("HASH");
	if (pkg_hash != 0) {
		const auto& files_of_pkg = pkgfiles.Get(pkg_hash);
		VfsSrcPkg& pkg = env.GetAddPackage(pkg_hash);
		for(auto it : ~files_of_pkg) {
			hash_t file_hash = it.key;
			const String* seen_path = MetaEnv().seen_path_names.Find(file_hash);
			String filepath;
			if (seen_path)
				filepath = *seen_path;
			else
				filepath = Format("%X", (int64)file_hash);
			// TODO
			files.Set(row, 0, filepath);
			files.Set(row, 1, it.value);
			files.Set(row, "HASH", (int64)file_hash);
			row++;
		}
	}
	files.SetCount(row);
	
	if (!files.IsCursor() && files.GetCount())
		files.SetCursor(0);
	else
		DataFile();
}

void MetaEnvTree::DataFile() {
	IdeMetaEnvironment& env = IdeMetaEnv();
	
	if (!pkgs.IsCursor() || !files.IsCursor()) {
		stmts.Clear();
		focus.Clear();
		code.Clear();
		return;
	}
	size_t pkg_hash = (int64)pkgs.Get("HASH");
	size_t file_hash = (int64)files.Get("HASH");
	
	int prev = stmts.IsCursor() ? stmts.GetCursor() : -1;
	Point scroll = stmts.GetScroll();
	
	int count = 0;
	stmts.Clear();
	stmt_ptrs.SetCount(0);
	if (pkg_hash != 0) {
		AddStmtNodes(0, env.env.root, 0, count);
	}
	else if (pkg_hash != 0 && file_hash != 0) {
		subset.Clear();
		stmt_ptrs.Clear();
		env.SplitValueHash(env.env.root, subset, pkg_hash);
		AddStmtNodes(0, *subset.n, &subset, count);
	}
	else {
		subset.Clear();
		stmt_ptrs.Clear();
		env.SplitValueHash(env.env.root, subset, pkg_hash, file_hash);
		AddStmtNodes(0, *subset.n, &subset, count);
	}
	stmts.OpenDeep(0);
	
	if (prev >= 0 && prev < stmt_ptrs.GetCount()) {
		stmts.SetCursor(prev);
		stmts.ScrollTo(scroll);
	}
	else if (!stmts.IsCursor())
		stmts.SetCursor(0);
	else
		DataTreeSelection();
}

void MetaEnvTree::DataTreeSelection() {
	focus.Clear();
	if (!stmts.IsCursor())
		return;
	focus_ptrs.SetCount(0);
	int sel = stmts.GetCursor();
	int count = 0;
	if (sel >= stmt_ptrs.GetCount()) return;
	VfsValue& n = *stmt_ptrs[sel];
	AddFocusNodes(0, n, 0, count);
	focus.OpenDeep(0);
	focus.SetCursor(0);
}

void MetaEnvTree::DataFocusSelection() {
	IdeMetaEnvironment& env = IdeMetaEnv();
	code.Clear();
	if (!focus.IsCursor())
		return;
	int sel = focus.GetCursor();
	const VfsValue& n = *focus_ptrs[sel];
	const AstValue* a = n;
	if (n.pkg_hash == 0 || n.file_hash == 0 || !a)
		return;
	VfsSrcPkg& pkg = env.GetAddPackage(n.pkg_hash);
	String full_path = pkg.GetFullPath(n.file_hash);
	String content = LoadFile(full_path);
	String node_content = GetStringRange(content, a->begin, a->end);
	code.SetData(node_content);
}

bool MetaEnvTree::Key(dword key, int count) {
	if (key == K_Q) {
		Data();
		return true;
	}
	return false;
}

void MetaEnvTree::AddStmtNodes(int tree_idx, VfsValue& n, VfsValueSubset* ns, int& count) {
	if (count >= tree_limit) return;
	
	if (tree_idx <= stmt_ptrs.GetCount())
		stmt_ptrs.SetCount(tree_idx+1,0);
	stmt_ptrs[tree_idx] = &n;
	
	String s;
	bool skip_default = false;
	const AstValue* a = n;
	const auto& env = MetaEnv();
	if (a) {
		String kind_str = VfsValue::AstGetKindString(a->kind);
		s = kind_str + ": " + n.id;
		if (a->type.GetCount()) s += " (" + a->type + ")";
		const String* pkg_path = env.seen_path_names.Find(n.pkg_hash);
		const String* file_path = env.seen_path_names.Find(n.file_hash);
		s += " (";
		if (pkg_path)
			s += " " + *pkg_path;
		else
			s += " " + Format("%X", (int64)n.pkg_hash);
		if (file_path)
			s += " : " + *file_path;
		else
			s += " " + Format(":%X", (int64)n.file_hash);
		s += ")";
		switch (a->kind) {
		case CXCursor_CXXMethod:
		case CXCursor_Constructor:
		case CXCursor_Destructor:
		case CXCursor_CompoundStmt:
		case CXCursor_FunctionDecl:
		case CXCursor_FieldDecl:
		case CXCursor_CXXBaseSpecifier:
			skip_default = true;
			break;
		default:
			break;
		}
	}
	else {
		s = n.id;
		if (n.ext)
			s += " (" + n.ext->GetTypeCls().GetName() + ")";
	}
	
	stmts.Set(tree_idx, s);
	
	if (!skip_default) {
		if (ns) {
			for (VfsValueSubset& s : ns->sub) {
				int idx = stmts.Add(tree_idx);
				count++;
				AddStmtNodes(idx, *s.n, &s, count);
				if (count >= tree_limit) return;
			}
		}
		else {
			for (VfsValue& sub : n.sub) {
				int idx = stmts.Add(tree_idx);
				count++;
				AddStmtNodes(idx, sub, 0, count);
				if (count >= tree_limit) return;
			}
		}
	}
}

void MetaEnvTree::AddFocusNodes(int tree_idx, VfsValue& n, VfsValueSubset* ns, int& count) {
	if (count >= tree_limit) return;
	
	if (tree_idx <= focus_ptrs.GetCount())
		focus_ptrs.SetCount(tree_idx+1,0);
	focus_ptrs[tree_idx] = &n;
	
	String s;
	const AstValue* a = n;
	if (a) {
		String kind_str = n.AstGetKindString();
		s = kind_str + ": " + n.id;
		if (a->type.GetCount()) s += " (" + a->type + ")";
	}
	else {
		s = n.id;
		if (n.ext)
			s += " (" + n.ext->GetTypeCls().GetName() + ")";
	}
	focus.Set(tree_idx, s);
	
	if (ns) {
		for (VfsValueSubset& s : ns->sub) {
			int idx = focus.Add(tree_idx);
			count++;
			AddFocusNodes(idx, *s.n, &s, count);
			if (count >= tree_limit) break;
		}
	}
	else {
		for (VfsValue& s : n.sub) {
			int idx = focus.Add(tree_idx);
			count++;
			AddFocusNodes(idx, s, 0, count);
			if (count >= tree_limit) break;
		}
	}
}

