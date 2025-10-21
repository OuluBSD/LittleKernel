#include "Vfs.h"


NAMESPACE_UPP


MetaIndexerCtrl::MetaIndexerCtrl() {
	Add(split.SizePos());
	
	split.Horz() << tasklist << placeholder;
	
	tasklist.AddColumn("#");
	tasklist.AddColumn("Path");
	//tasklist.AddColumn("Includes");
	//tasklist.AddColumn("Defines");
	tasklist.AddColumn("Ext");
	tasklist.ColumnWidths("1 5 1");
	
}

void MetaIndexerCtrl::Data() {
	
	for(int i = 0; i < Indexer::Jobs().GetCount(); i++) {
		const IndexerJob& job = Indexer::Jobs()[i];
		tasklist.Set(i, 0, i);
		tasklist.Set(i, 1, job.path);
		tasklist.Set(i, 2, job.ext);
	}
	tasklist.SetCount(Indexer::Jobs().GetCount());
	
}


END_UPP_NAMESPACE
