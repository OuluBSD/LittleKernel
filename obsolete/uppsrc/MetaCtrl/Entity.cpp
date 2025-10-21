#include "MetaCtrl.h"
#include <ide/Vfs/Vfs.h>

NAMESPACE_UPP


void ComponentCtrl::GetDataset(DatasetPtrs& p) const {
	if (!ext)
		return;
	Component& comp = dynamic_cast<Component&>(*ext);
	comp.GetDataset(p);
}




VirtualFSComponentCtrl::VirtualFSComponentCtrl() {
	
}

void VirtualFSComponentCtrl::Data() {
	if (!data_iter++)
		Init();
	
	// Get current VirtualNode
	VirtualNode vnode = Find(vnode_path);
	while (!vnode && vnode_path.GetPartCount() > 0) {
		vnode_path.RemoveLast();
		vnode = Find(vnode_path);
	}
	if (!vnode) {
		PromptOK("vnode error");
		return;
	}
	hash_t vnode_type_hash = vnode.GetTypeHash();
	
	// Check if vnode_ctrl is correct
	bool create_new = !vnode_ctrl || vnode_ctrl->type_hash != vnode_type_hash;
	if (create_new) {
		if (vnode_ctrl) {
			RemoveChild(&*vnode_ctrl);
			vnode_ctrl.Clear();
		}
		// Make new ctrl if needed
		vnode_ctrl = CreateCtrl(vnode);
		if (vnode_ctrl) {
			vnode_ctrl->type_hash = vnode_type_hash;
			Add(vnode_ctrl->SizePos());
		}
	}
	
	if (vnode_ctrl)
		vnode_ctrl->Data();
}

void VirtualFSComponentCtrl::OnTreeCursor(TreeCtrl* tree) {
	if (!tree->IsCursor()) return;
	Vector<int> path;
	int cur = tree->GetCursor();
	path << cur;
	while (cur) {
		cur = tree->GetParent(cur);
		path << cur;
	}
	Reverse(path);
	Vector<Value> parts;
	for(int i = 1; i < path.GetCount(); i++) {
		Value val = tree->Get(path[i]);
		parts << val;
	}
	//DUMPC(parts);
	VfsPath vfspart(parts);
	this->vnode_path = vfspart;
	PostCallback(THISBACK(Data));
}

VfsPath VirtualFSComponentCtrl::GetCursorPath() const {
	VfsPath full_path(GetValue().GetPath());
	full_path.Append(vnode_path);
	return full_path;
}

VfsPath VirtualFSComponentCtrl::GetCursorRelativePath() const {
	return vnode_path;
}

void VirtualFSComponentCtrl::EditPos(JsonIO& json) {
	if (vnode_ctrl)
		vnode_ctrl->EditPos(json);
}

void VirtualFSComponentCtrl::DataTree(TreeCtrl& tree) {
	// Fill tree with virtual-node data: visit root
	tree.WhenCursor.Clear(); // prevent useless calls
	tree.Clear();
	String name = GetExt().GetName();
	if (name.IsEmpty())
		name = GetTitle();
	if (name.IsEmpty())
		name = "This component";
	tree.SetDisplay(QTFDisplay());
	tree.SetRoot(MetaImgs::RedRing(), name);
	try {
		Visit(tree, 0, Root());
	}
	catch (...) {
		LOG("VirtualFSComponentCtrl::DataTree: error visiting");
	}
	tree.OpenDeep(0);
	tree.SetCursor(0);
	tree.WhenCursor = THISBACK1(OnTreeCursor, &tree);
}

bool VirtualFSComponentCtrl::Visit(TreeCtrl& tree, int id, VirtualNode n) {
	auto sub = n.GetAll();
	for (VirtualNode& s : sub) {
		Value name = s.GetName();
		//DLOG("Visit " << name.GetTypeName() << ": " << name.ToString());
		hash_t type_hash = s.GetTypeHash();
		String qtf;
		if (!TreeItemString(s, name, qtf))
			qtf = DeQtf(name.ToString() + " (" + TypeStringHasherIndex::ToString(type_hash) + ")");
		int sub_id = tree.Add(id, MetaImgs::BlueRing(), name, qtf);
		if (!Visit(tree, sub_id, s))
			return false;
	}
	return true;
}

VirtualNode VirtualFSComponentCtrl::Find(const VfsPath& rel_path) {
	VirtualNode p = Root();
	for(int i = 0; i < rel_path.GetPartCount(); i++) {
		Value item = rel_path.Parts()[i];
		//DLOG(i << ": " << item.GetTypeName() << ": " << item.ToString());
		p = p.Find(item);
		if (!p)
			break;
	}
	return p;
}

#if 0
VirtualNode VirtualFSComponentCtrl::GetAdd(const VfsPath& rel_path, hash_t type_hash) {
	VirtualNode n = Root();
	TODO;
	return n;
}
#endif



ValueVFSComponentCtrl::ValueVFSComponentCtrl() {
	
}

VirtualNode ValueVFSComponentCtrl::Root() {
	if (!root) {
		VfsPath root_path; // empty
		VfsValue& val = this->ext->val;
		if (val.value.Is<AstValue>()) {
			LOG("ValueVFSComponentCtrl::Root: warning: resetting AstValue to Value");
			val.value = Value();
		}
		auto& data = root.CreateValue(root_path, &val.value);
		data.key = "Root";
		data.vfs_value = &val;
	}
	return root;
}

Value* ValueVFSComponentCtrl::GetPolyValue() {
	VfsValue& val = this->ext->val;
	return &val.value;
}

void ValueVFSComponentCtrl::Set(Value key, Value value) {
	Value* db = this->GetPolyValue();
	ASSERT(db);
	if (db) {
		ValueMap map = *db;
		map.Set(key, value);
		*db = map;
	}
}

Value ValueVFSComponentCtrl::Get(Value key) {
	Value* db = this->GetPolyValue();
	ASSERT(db);
	if (db) {
		ValueMap map = *db;
		int i = map.Find(key);
		if (i >= 0)
			return map.GetValue(i);
	}
	return Value();
}






VNodeComponentCtrl::VNodeComponentCtrl(ValueVFSComponentCtrl& o, const VirtualNode& vnode) : owner(o), vnode(vnode) {
	ASSERT(vnode);
}

DatasetPtrs VNodeComponentCtrl::RealizeEntityVfsObject(const VirtualNode& vnode, hash_t type_hash) {
	DatasetPtrs p;
	owner.GetDataset(p);
	TODO
	#if 0
	if (!p.entity)
		return p;
	
	//DUMP(vnode.data->path);
	
	VfsPath path = vnode.data->path;
	ASSERT(vnode.data->vfs_value);
	if (!vnode.data->vfs_value)
		Panic("internal error: todo?");
	
	// needed? vnode.data->vfs_value->CreateExt(type_hash);
	
	path.Add(vnode.data->vfs_value->ext->GetTypeName());
	int i = p.entity->objs.Find(path);
	if (i >= 0) {
		EntityData& data = p.entity->objs[i];
		VfsValueExtFactory::SetEntityData(p, type_hash, data);
	}
	else {
		EntityData* data = it.create_ed_fn();
		ASSERT(data); // ???
		if (data) {
			VfsValueExtFactory::SetEntityData(p, type_hash, *data);
			p.entity->objs.Add(path, data);
		}
	}
	
	#endif
	return p;
}

void VNodeComponentCtrl::GetDataset(DatasetPtrs& p) const {
	
	// Get entity-vfs-objects
	if (vnode.IsValue() && p.entity) {
		TODO // seems to be type_hash logic instead of AstValue::kind logic
		#if 0
		const VfsPath& path = vnode.data->path;
		for(auto ed : ~p.entity->objs) {
			if (ed.key.IsLeft(path) && ed.key.GetPartCount() == path.GetPartCount()+1) {
				EntityData& data = ed.value;
				int data_kind = data.GetKind();
				VfsValueExtFactory::Set(p, data_kind, data);
			}
		}
		#endif
	}
}
















EntityInfoCtrl::EntityInfoCtrl() {
	CtrlLayout(info);
	Add(info.SizePos());
	info.split.Horz() << data << value;
	info.split.SetPos(7500);
	
	data.AddColumn("Key");
	data.AddColumn("Value");
	data.AddIndex("IDX");
	data.ColumnWidths("1 4");
	data.WhenCursor = THISBACK(DataCursor);
	
	info.name.WhenAction = THISBACK(OnEdit);
	info.show_hidden_values.WhenAction = THISBACK(Data);
	value.WhenAction = THISBACK(OnEditValue);
}

void EntityInfoCtrl::Data() {
	Entity& ent = GetExt<Entity>();
	VfsValue& enode = ent.val;
	
	info.name = enode.id;
	
	// Realize some default fields
	ent.Data("description");
	ent.Data("gender");
	
	Vector<VfsValue*> envs = IdeMetaEnv().FindAllEnvs();
	
	// Get all contexts
	all_ctxs.Clear();
	for (VfsValue* env : envs) {
		Vector<VfsValue*> ctxs = env->FindAllShallow(AsTypeHash<Context>());
		for (VfsValue* ctx : ctxs) {
			String key = /*env->id + ": " +*/ ctx->id;
			all_ctxs.Add(key, ctx);
		}
	}
	
	// Entity's context
	String ent_ctx = ent.Data("ctx");
	String match_key = ent_ctx;
	
	
	// Entity data fields
	data.SetCount(0);
	bool show_hidden_values = info.show_hidden_values.Get();
	int row = 0;
	for(int i = 0; i < ent.data.GetCount(); i++) {
		String key = ent.data.GetKey(i);
		if (!show_hidden_values && key.GetCount() && key[0] == '.')
			continue;
		Value value = ent.data[i];
		data.Set(row, "IDX", i);
		data.Set(row, 0, key);
		if (key == "gender") {
			DropList* dl = new DropList;
			for (auto g : Genders())
				dl->Add(g);
			data.SetCtrl(row, 1, dl);
			const auto& cats = VfsValueExtFactory::GetCategories();
			int gender_i = max(0, Genders().Find(value.ToString()));
			dl->SetIndex(gender_i);
			dl->WhenAction = [&ent,dl]{
				ent.data.GetAdd("gender") = Genders()[dl->GetIndex()];
			};
		}
		else if (key == "ctx") {
			DropList* dl = new DropList;
			data.SetCtrl(row, 1, dl);
			dl->Clear();
			dl->Add("");
			int active_ctx = 0;
			for(int i = 0; i < all_ctxs.GetCount(); i++) {
				String ctx_key = all_ctxs.GetKey(i);
				dl->Add(ctx_key);
				if (ctx_key == match_key)
					active_ctx = 1+i;
			}
			if (active_ctx >= 0 && active_ctx < dl->GetCount()) {
				dl->SetIndex(active_ctx);
			}
			dl->WhenAction = [this,&ent,dl]{
				int ctx_i = dl->GetIndex()-1;
				if (ctx_i >= 0 && ctx_i < dl->GetCount()) {
					VfsValue& ctx = *all_ctxs[ctx_i];
					ent.Data("ctx") = ctx.id;
				}
				else {
					ent.Data("ctx") = String();
				}
			};
		}
		else {
			data.Set(row, 1, value);
		}
		row++;
	}
	data.SetCount(row);
	data.SetSortColumn(0);
	
	if (!data.IsCursor())
		data.SetCursor(0);
	else
		DataCursor();
}

void EntityInfoCtrl::DataCursor() {
	if (!data.IsCursor()) {
		value.Clear();
		return;
	}
	
	Entity& ent = GetExt<Entity>();
	int data_i = data.Get("IDX");
	Value val = ent.data[data_i];
	value.SetData(val.ToString());
}

void EntityInfoCtrl::OnEdit() {
	Entity& e = GetExt<Entity>();
	auto& enode = e.val;
	enode.id = ~info.name;
		
	WhenValueChange();
}

void EntityInfoCtrl::OnEditValue() {
	if (!data.IsCursor())
		return;
	Entity& e = GetExt<Entity>();
	int data_i = data.Get("IDX");
	Value val = value.GetData();
	e.data[data_i] = val.ToString();
	data.Set(1, val);
}

void EntityInfoCtrl::ToolMenu(Bar& bar) {
	
}

void EntityInfoCtrl::GetDataset(DatasetPtrs& p) const {
	EntityInfoCtrl* e = const_cast<EntityInfoCtrl*>(this);
	VfsValue& n = e->GetValue();
	p.entity = &e->GetExt<Entity>();
	FillDataset(p, n, 0);
}

INITIALIZER_COMPONENT_CTRL(Entity, EntityInfoCtrl)

END_UPP_NAMESPACE
