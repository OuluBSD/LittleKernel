#include "Ecs.h"



NAMESPACE_UPP

void (*FillDatasetPtr)(DatasetPtrs& p, VfsValue& n, Component* this_comp);

void Component::GetDataset(DatasetPtrs& p) const {
	VfsValue& n = val;
	ASSERT(FillDatasetPtr);
	if (FillDatasetPtr)
		FillDatasetPtr(p, n, const_cast<Component*>(this));
}

Entity::Entity(VfsValue& v) : VfsValueExt(v) {
	
}

int Entity::GetGender() const {
	int i = data.Find("gender");
	if (i < 0)
		return 0;
	else {
		Value val = data[i];
		if (val.Is<int>() || val.Is<int64>() ||val.Is<double>())
			return (int)val;
		else
			return val.ToString() == "female";
	}
}

EntityData* Entity::FindData(const VfsPath& path) {
	int i = objs.Find(path);
	if (i >= 0)
		return &objs[i];
	return 0;
}

void Entity::Visit(Vis& v) {
	v.Ver(2)
	(1)	("data",data);
	if (v.file_ver >= 2) {
		struct Item : Moveable<Item> {
			VfsPath path;
			hash_t type_hash;
			void Visit(Vis& v) {v("path",path,VISIT_NODE)("type_hash",(int64&)type_hash);}
		};
		if (v.IsLoading()) {
			Vector<Item> header;
			v(2)(".header", header, VISIT_VECTOR);
			this->objs.Clear();
			for(auto& it : header) {
				TODO
				#if 0
				#define DATASET_ITEM(a,b,e) \
				if (it.kind == c) {\
					a* o = new a(); \
					this->objs.Add(it.path, o); \
					v((String)it.path, *o, VISIT_NODE); \
					continue;\
				}
				VIRTUALNODE_DATASET_LIST
				#undef DATASET_ITEM
				#endif
				ASSERT_(0,"Invalid kind");
				v.SetError("Invalid kind");
				break;
			}
		}
		else {
			Vector<Item> header;
			header.Reserve(objs.GetCount());
			for (auto it : ~objs) {
				Item& i = header.Add();
				i.path = it.key;
				i.type_hash = it.value.GetTypeHash();
			}
			v(2)(".header", header, VISIT_VECTOR);
			for (auto it : ~objs) {
				v((String)it.key, it.value, VISIT_NODE); \
			}
		}
	}
}

int64 Entity::GetNextIdx() {
	static int64 i;
	return ++i;
}


void Entity::Initialize(String prefab) {
	SetPrefab(prefab);
	
	Engine* e = val.FindOwner<Engine>();
	ASSERT(e);
	if (!e) throw Exc("No Engine found");
	uint64 ticks = e->GetTicks();
	SetCreated(ticks);
	SetChanged(ticks);
}

ComponentPtr Entity::CreateEon(String id) {
	auto* f = VfsValueExtFactory::FindFactory(id);
	if (!f)
		return ComponentPtr();
	
	if (f->type != VFSEXT_COMPONENT)
		return ComponentPtr();
	
	VfsValue& val = this->val.Add();
	val.ext = f->new_fn(val);
	val.type_hash = val.ext ? val.ext->GetTypeHash() : 0;
	Component* comp = dynamic_cast<Component*>(&*val.ext);
	ASSERT(comp);
	return comp;
}

INITIALIZER_VFSEXT(Entity, "entity", "Ecs|Basic");




INITIALIZER_COMPONENT(Context, "vfs.context", "Vfs|Util")
INITIALIZER_COMPONENT(PkgEnv, "vfs.pkg.env", "Vfs|Util")
INITIALIZER_COMPONENT(DbRef, "vfs.db.ref", "Vfs|Util")
INITIALIZER_COMPONENT(VirtualIOScript, "vfs.virtio.script", "Vfs|Util")
INITIALIZER_COMPONENT(VirtualIOScriptProofread, "vfs.virtio.script.proofread", "Vfs|Util")
INITIALIZER_COMPONENT(VirtualIOScriptLine, "vfs.virtio.script.line", "Vfs|Util")
INITIALIZER_COMPONENT(VirtualIOScriptSub, "vfs.virtio.script.sub", "Vfs|Util")

END_UPP_NAMESPACE
