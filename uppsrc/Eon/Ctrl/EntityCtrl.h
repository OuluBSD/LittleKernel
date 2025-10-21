#ifndef _EonCtrl_EntityCtrl_h_
#define _EonCtrl_EntityCtrl_h_


struct EntityDataCtrl : public ComponentCtrl {
	ArrayCtrl list;
	int ent_cursor = 0;
	
	EntityDataCtrl();
	
	void UpdateEntityData(Entity& e);
	void AddEntityDataRow(UPP::Value key, UPP::Value value);
	
	void SetComponent(Component&) override {}
	
};

class EntityCtrl : public ParentCtrl {
	ArrayMap<TypeId, ComponentCtrl>		comp_ctrls;
	Splitter							vsplit, hsplit;
	EntityBrowserCtrl					ent_browser;
	EntityContentCtrl					ent_content;
	EntityDataCtrl						ent_data;
	ParentCtrl							ent_cont;
	
	EntityPtr						sel_ent;
	ComponentCtrl*						active_ctrl = 0;
	Engine*						mach = 0;
	
	void ClearActiveCtrl();
	void UpdateEntityData();
	void SetEntityDataCtrl();
	void SetComponentCtrl(Component& c);
	
public:
	typedef EntityCtrl CLASSNAME;
	EntityCtrl();
	
	void SetEngine(Engine& m) {mach = &m; ent_browser.SetEngine(m);}
	void Updated() override;
	void OnEntityCursorChanged();
	void OnContentCursorChanged();
	
	
};


#endif
