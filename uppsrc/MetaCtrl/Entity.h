#ifndef _AI_TextCtrl_Entity_h_
#define _AI_TextCtrl_Entity_h_


struct Script;
class ToolAppCtrl;
struct VfsSrcFile;

struct ComponentCtrl : VfsValueExtCtrl, DatasetProvider {
	void GetDataset(DatasetPtrs&) const override;
};

class VNodeComponentCtrl;

class VirtualFSComponentCtrl : public ComponentCtrl {
	One<VNodeComponentCtrl> vnode_ctrl;
	VfsPath vnode_path;
	int data_iter = 0;
	
	bool Visit(TreeCtrl& tree, int id, VirtualNode n);
	void OnTreeCursor(TreeCtrl* tree);
	virtual bool TreeItemString(const VirtualNode& n, const Value& key, String& qtf_value) {return false;}
protected:
	friend class ValueVFSComponentCtrl;
	VirtualFSComponentCtrl();
	void DataTree(TreeCtrl& tree) override;
	VfsPath GetCursorPath() const override;
	VfsPath GetCursorRelativePath() const;
	void EditPos(JsonIO& json) override;
public:
	typedef VirtualFSComponentCtrl CLASSNAME;
	
	virtual VirtualNode Root() = 0;
	virtual VNodeComponentCtrl* CreateCtrl(const VirtualNode& vnode) = 0;
	virtual void Init() {}
	void Data() override;
	
	VirtualNode GetAdd(const VfsPath& rel_path, int kind);
	VirtualNode Find(const VfsPath& rel_path);
	VNodeComponentCtrl* GetVNodeComponentCtrl() {return vnode_ctrl ? &*vnode_ctrl : 0;}
};

class ValueVFSComponentCtrl : public VirtualFSComponentCtrl {
protected:
	VirtualNode root;
public:
	typedef ValueVFSComponentCtrl CLASSNAME;
	ValueVFSComponentCtrl();
	VirtualNode Root() override;
	Value* GetPolyValue() override;
	void Set(Value key, Value value);
	Value Get(Value key);
};

class VNodeComponentCtrl : public ParentCtrl, public DatasetProvider  {
	
protected:
	friend class VirtualFSComponentCtrl;
	hash_t type_hash = 0;
	VirtualNode vnode;
	ValueVFSComponentCtrl& owner;
public:
	typedef VNodeComponentCtrl CLASSNAME;
	VNodeComponentCtrl(ValueVFSComponentCtrl&, const VirtualNode& vnode);
	
	void GetDataset(DatasetPtrs&) const override;
	DatasetPtrs RealizeEntityVfsObject(const VirtualNode& vnode, hash_t type_hash);
	hash_t GetTypeHash() const {return type_hash;}
	virtual void Data() {}
	virtual void EditPos(JsonIO& json) {}
	
	VirtualNode GetVnode() const {return vnode;}
	template <class T>
	T& GetAddValue(String name) {
		ASSERT(vnode);
		T& o = vnode.template GetAddExt<T>(name);
		return o;
	}
	
	// NOTE requires full declaration instead of fwd decl only
	/*
	#define DATASET_ITEM(type, field, kind, cat, strname) \
		type& GetAdd##type() {return this->template GetAddValue<type>(#field);}
	VIRTUALNODE_DATASET_LIST
	#undef DATASET_ITEM
	*/
};

class EntityInfoCtrl : public VfsValueExtCtrl, public DatasetProvider {
	WithEntityInfo<Ctrl> info;
	VectorMap<String,VfsValue*> all_ctxs;
	ArrayCtrl data;
	DocEdit value;
	
public:
	typedef EntityInfoCtrl CLASSNAME;
	EntityInfoCtrl();
	void Data() override;
	void ToolMenu(Bar& bar) override;
	void OnEdit();
	void OnEditValue();
	void DataCursor();
	void GetDataset(DatasetPtrs&) const override;
	
	Event<> WhenValueChange;
};

INITIALIZE(EntityInfoCtrl)



#endif
