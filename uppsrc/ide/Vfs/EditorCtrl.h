#ifndef _ide_Vfs_EditorCtrl_h_
#define _ide_Vfs_EditorCtrl_h_


class EntityEditorCtrl : public ToolAppCtrl {
	Splitter hsplit, lsplit;
	//ArrayCtrl entlist, extlist;
	TreeCtrl ecs_tree;
	TreeCtrl content_tree;
	Ctrl ext_place;
	One<VfsValueExtCtrl> ext_ctrl;
	hash_t ext_ctrl_type_hash = 0;
	int post_content_cursor = -1;
	
	void DataEcsTreeVisit(int treeid, VfsValue& n);
	
protected:
	Ptr<VfsValue> file_root;
	Vector<VfsValue*> ecs_tree_nodes;
	
public:
	typedef EntityEditorCtrl CLASSNAME;
	EntityEditorCtrl();
	
	void Data() override;
	void SetFont(Font fnt);
	void ToolMenu(Bar& bar) override;
	VfsSrcFile& RealizeFileRoot();
	void DataEcsTree_RefreshNames();
	void DataEcsTree();
	void DataExtension();
	void DataExtCtrl();
	void Visit(Vis& v) override;
	void Move(VfsValue* n);
	void RemoveValue(VfsValue* n);
	void AddValue(VfsValue* n, String id, hash_t type_hash);
	void AddEntity();
	void RemoveEntity();
	void AddComponent();
	void RemoveComponent();
	void SetExtensionCtrl(hash_t type_hash, VfsValueExtCtrl* ctrl);
	void ClearExtensionCtrl() {SetExtensionCtrl(0,0);}
	VfsValueExt* GetSelected();
	void EditPos(JsonIO& jio) override;
	void SelectEcsTree(VfsValue* n);
	VfsValue* SelectTreeValue(String title);
	
	static bool AcceptsExt(String e) { return e == ".ecs"; }
	static String GetID() { return "Entity Editor"; }
	
	void Do(int i);
	
	Event<> WhenSaveEditPos;
};


#endif
