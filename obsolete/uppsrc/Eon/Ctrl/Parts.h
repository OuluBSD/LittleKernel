#ifndef _EonCtrl_Parts_h_
#define _EonCtrl_Parts_h_


class PoolTreeCtrl : public ParentCtrl {
	Engine*		mach = 0;
	EntityStorePtr	es;
	PoolRef		selected;
	TreeCtrl			tree;
	uint64				last_hash = 0;

	void OnCursor();
	hash_t GetPoolTreeHash() const;
	
	void AddPool(int parent, PoolRef pool);
	
public:
	typedef PoolTreeCtrl CLASSNAME;
	PoolTreeCtrl();
	
	void SetEngine(Engine& m);
	void Updated() override;
	void Data();
	
	PoolRef GetSelected() {return selected;}
	
	Callback WhenPoolChanged;
	
};


class EntityListCtrl : public ParentCtrl {
	EntityStorePtr	es;
	PoolRef		pool;
	EntityPtr		selected;
	ArrayCtrl			list;
	uint64				last_hash = 0;

	void OnCursor();
	hash_t GetEntityListHash() const;
	
public:
	typedef EntityListCtrl CLASSNAME;
	EntityListCtrl();
	
	void SetPool(PoolRef pool);
	void Updated() override;
	void Data();
	
	EntityPtr GetSelected() {return selected;}
	
	Callback WhenEntityChanged;
	
};


class EntityBrowserCtrl : public ParentCtrl {
	Splitter			vsplit;
	PoolTreeCtrl		pool_tree;
	EntityListCtrl		ent_list;
	PoolRef				sel_pool;
	
	void OnPoolCursorChanged();
	
public:
	typedef EntityBrowserCtrl CLASSNAME;
	EntityBrowserCtrl();
	
	void SetEngine(Engine& m) {pool_tree.SetEngine(m);}
	void Updated() override;
	void Data();
	EntityPtr GetSelected() {return ent_list.GetSelected();}
	
	Callback WhenEntityChanged;
	
};

class EntityContentCtrl : public ParentCtrl {
	TreeCtrl tree;
	Image ent_icon, comp_icon, iface_icon;
	EntityPtr ent;
	int64 ent_changed_time = -1;
	VectorMap<int, int> node_comps;
	
	void OnCursor();
	void AddTreeEntity(int tree_i, const Entity& e);
	
public:
	typedef EntityContentCtrl CLASSNAME;
	EntityContentCtrl();
	
	void Updated() override;
	
	void SetEntity(EntityPtr e) {ent = e;}
	void GetCursor(ComponentPtr& c);
	
	Callback WhenContentCursor;
	
};


class InterfaceListCtrl : public ParentCtrl {
	LinkedList<ExchangeProviderBasePtr> ifaces;
	ArrayCtrl list;
	EntityPtr ent;
	int64 ent_changed_time = -1;
	int write_cursor;
	
	void OnCursor();
	void Data();
	
	
	template <class T>
	void AddInterface(int comp_i, Ptr<T> o) {
		int iface_i = ifaces.GetCount();
		ifaces.Add(o);
		list.Set(write_cursor, 0, comp_i);
		list.Set(write_cursor, 1, iface_i);
		list.Set(write_cursor, 2, TypeId(AsTypeCls<T>()).CleanDemangledName());
		list.Set(write_cursor, 3, o->GetConnections().GetCount());
		write_cursor++;
	}
	
public:
	typedef InterfaceListCtrl CLASSNAME;
	InterfaceListCtrl();
	
	void Updated() override;
	
	void SetEntity(EntityPtr e) {ent = e;}
	void GetCursor(ComponentPtr& c, ExchangeProviderBasePtr& i);
	
	Callback WhenInterfaceCursor;
	
};


#endif
