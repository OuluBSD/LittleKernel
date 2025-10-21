#ifndef _ide_Vfs_MetaIndexerCtrl_h_
#define _ide_Vfs_MetaIndexerCtrl_h_


class MetaIndexerCtrl : public ParentCtrl {
	Splitter split;
	ArrayCtrl tasklist;
	Ctrl placeholder;
	
public:
	typedef MetaIndexerCtrl CLASSNAME;
	MetaIndexerCtrl();
	
	void Data();
	
};


#endif
