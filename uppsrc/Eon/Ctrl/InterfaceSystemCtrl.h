#ifndef _EonCtrl_InterfaceSystemCtrl_h_
#define _EonCtrl_InterfaceSystemCtrl_h_


struct InterfaceDataCtrl : public InterfaceCtrl {
	ArrayCtrl list;
	int iface_cursor = 0;
	
	InterfaceDataCtrl();
	
	void UpdateInterfaceData(ExchangeProviderBasePtr e);
	void AddInterfaceDataRow(UPP::Value key, UPP::Value value);
	
	void SetInterface(ComponentPtr, ExchangeProviderBasePtr) override {}
	
};

class InterfaceSystemCtrl : public ParentCtrl {
	ArrayMap<TypeId, InterfaceCtrl>		iface_ctrls;
	Splitter							vsplit, hsplit;
	EntityBrowserCtrl					ent_browser;
	InterfaceListCtrl					iface_list;
	InterfaceDataCtrl					ent_data;
	ParentCtrl							ent_cont;
	
	VectorMap<int, int>					node_ifaces;
	EntityPtr						sel_ent;
	ExchangeProviderBasePtr				sel_iface;
	InterfaceCtrl*						active_ctrl = 0;
	
	Image ent_icon, comp_icon, iface_icon;
	
	void OnEntityCursorChanged();
	void OnInterfaceCursorChanged();
	void ClearActiveCtrl();
	void SetInterfaceCtrl(ComponentPtr c, ExchangeProviderBasePtr b);
	
public:
	typedef InterfaceSystemCtrl CLASSNAME;
	InterfaceSystemCtrl();
	
	void Updated() override;
	void SetEngine(Engine& m) {ent_browser.SetEngine(m);}
	
};


#endif
