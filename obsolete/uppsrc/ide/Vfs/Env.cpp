#include "Vfs.h"

NAMESPACE_UPP

EnvEditorCtrl::EnvEditorCtrl() {
	Add(hsplit.SizePos());
	
	hsplit.Horz() << ctxlist << edit;
	hsplit.SetPos(1500);
	
	CtrlLayout(edit);
	edit.name.WhenAction << THISBACK(OnValueChange);
	
	ctxlist.AddColumn("Context");
	ctxlist.AddColumn("Items");
	ctxlist.WhenBar << [this](Bar& b) {
		b.Add("Add Context", THISBACK(AddContext));
		if (ctxlist.IsCursor())
			b.Add("Remove Context", THISBACK(RemoveContext));
	};
	ctxlist.WhenCursor << THISBACK(DataItem);
	
	edit.items.AddColumn("Enabled");
	edit.items.AddColumn("Item");
	edit.items.ColumnWidths("1 8");
	
}

void EnvEditorCtrl::RefreshDatabases() {
	dbs.SetCount(0);
	
	IdeMetaEnvironment& env = IdeMetaEnv();
	
	for(VfsSrcPkg& pkg : env.GetPackages()) {
		
		for(int j = 0; j < pkg.src_files.GetCount(); j++) {
			VfsSrcFile& file = pkg.src_files[j];
			
			if (file.IsExt(".db-src")) {
				hash_t pkg_hash = pkg.GetPackageHash();
				hash_t file_hash = file.GetFileHash();
				dbs << &env.RealizeFileNodeHash(pkg_hash, file_hash, AsTypeHash<SrcTxtHeader>());
			}
		}
	}
}

String EnvEditorCtrl::MakeIdString(const Vector<VfsValue*>& v) {
	String s;
	for (auto& np : v) {
		if (!s.IsEmpty()) s.Cat(", ");
		s << np->id;
	}
	return s;
}

void EnvEditorCtrl::Data() {
	
	RealizeFileRoot();
	RefreshDatabases();
	
	if (!file_root) {
		ctxlist.Clear();
		edit.items.Clear();
		return;
	}
	
	ctxs = this->file_root->FindAllShallow(AsTypeHash<Context>());
	ctx_dbs.SetCount(ctxs.GetCount());
	int row = 0;
	for(int i = 0; i < ctxs.GetCount(); i++) {
		auto& ep = ctxs[i];
		if (!ep) {ctx_dbs[i].Clear(); continue;}
		auto& e = *ep;
		ctxlist.Set(row, 0, e.id);
		
		// Make string of items
		auto& db_refs = ctx_dbs[i];
		db_refs = e.FindAllShallow(AsTypeHash<DbRef>());
		ctxlist.Set(row, 1, MakeIdString(db_refs));
		row++;
	}
	ctxlist.SetCount(row);
	
	for(int i = edit.items.GetCount(); i < dbs.GetCount(); i++) {
		VfsValue& db = *dbs[i];
		ASSERT(!db.id.IsEmpty());
		Option* o = new Option;
		o->WhenAction << THISBACK2(OnOption, o, &db);
		edit.items.SetCtrl(i, 0, o);
		edit.items.Set(i, 1, db.id);
	}
	edit.items.SetCount(dbs.GetCount());
	
	if (!ctxlist.IsCursor() && ctxlist.GetCount())
		ctxlist.SetCursor(0);
	else
		DataItem();
}

void EnvEditorCtrl::DataItem() {
	if (!ctxlist.IsCursor()) {
		edit.items.Clear();
		return;
	}
	int ctx_i = ctxlist.GetCursor();
	auto& ctx = *ctxs[ctx_i];
	edit.name = ctx.id;
	
	auto& db_refs = ctx_dbs[ctx_i];
	Index<String> enabled;
	for (auto& db_ref : db_refs) {
		ASSERT(!db_ref->id.IsEmpty());
		enabled.FindAdd(db_ref->id);
	}
	
	int row = 0;
	for(int i = 0; i < dbs.GetCount(); i++) {
		VfsValue& db = *dbs[i];
		ASSERT(!db.id.IsEmpty());
		bool db_enabled = enabled.Find(db.id) >= 0;
		Option* o = dynamic_cast<Option*>(edit.items.GetCtrl(row, 0));
		o->Set(db_enabled);
		row++;
	}
	edit.items.SetCount(row);
	
}

void EnvEditorCtrl::OnOption(Option* opt, VfsValue* db) {
	String db_id = db->id;
	if (!ctxlist.IsCursor()) {
		edit.items.Clear();
		return;
	}
	int ctx_i = ctxlist.GetCursor();
	auto& ctx = *ctxs[ctx_i];
	auto& db_refs = ctx_dbs[ctx_i];
	bool enabled = opt->Get();
	int sub_i = ctx.FindPos<DbRef>(db_id);
	if (enabled) {
		if (sub_i >= 0)
			return;
		ctx.Add<DbRef>(db_id);
	}
	else {
		if (sub_i < 0)
			return;
		ctx.sub.Remove(sub_i);
	}
	
	db_refs = ctx.FindAllShallow(AsTypeHash<DbRef>());
	ctxlist.Set(1, MakeIdString(db_refs));
	
	PostCallback(THISBACK(DataItem));
}

void EnvEditorCtrl::ToolMenu(Bar& bar) {
	
}

void EnvEditorCtrl::Visit(Vis& v) {
	if (v.IsLoading()) {
		VfsValue* n = 0;
		IdeMetaEnv().LoadFileRootVisit(GetFileIncludes(), GetFilePath(), v, true, n);
		if (n)
			SetFileNode(n);
	}
	else {
		VfsSrcFile& file = RealizeFileRoot();
		file.MakeTempFromEnv(false);
		file.Visit(v);
		file.ClearTemp(1);
	}
}

VfsSrcFile& EnvEditorCtrl::RealizeFileRoot() {
	IdeMetaEnvironment& env = IdeMetaEnv();
	String path = this->GetFilePath();
	VfsSrcFile& file = env.ResolveFile("", path);
	VfsSrcPkg& pkg = *file.pkg;
	hash_t pkg_hash = pkg.GetPackageHash();
	hash_t file_hash = pkg.GetFileHash(path);
	ASSERT(file_hash >= 0);
	VfsValue& n = env.RealizeFileNodeHash(pkg_hash, file_hash, AsTypeHash<PkgEnv>());
	this->file_root = &n;
	if (!this->file_root->type_hash) {
		this->file_root->CreateExt<PkgEnv>();
	}
	ASSERT(this->file_root->type_hash == AsTypeHash<PkgEnv>());
	return file;
}

void EnvEditorCtrl::SetFont(Font fnt) {
	
}

void EnvEditorCtrl::OnValueChange() {
	if (!ctxlist.IsCursor()) {
		edit.items.Clear();
		return;
	}
	int ctx_i = ctxlist.GetCursor();
	auto& ctx = *ctxs[ctx_i];
	ctx.id = edit.name.GetData();
	ctxlist.Set(0, ctx.id);
}

void EnvEditorCtrl::AddContext() {
	RealizeFileRoot();
	VfsValue& n = *file_root;
	VfsValue& e = n.Add<Context>().val;
	e.id = "Unnamed";
	PostCallback(THISBACK(Data));
}

void EnvEditorCtrl::RemoveContext() {
	if (!ctxlist.IsCursor()) return;
	VfsValue& n = *file_root;
	int ent_i = ctxlist.GetCursor();
	if (ent_i >= 0 && ent_i < n.sub.GetCount())
		n.sub.Remove(ent_i);
	PostCallback(THISBACK(Data));
}

END_UPP_NAMESPACE
