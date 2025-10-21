#include "Base.h"

NAMESPACE_UPP

void GenerateTree(VfsValue& root, int total, int branching_low, int branching_high, Event<VfsValue&> set_value) {
	root.sub.Clear();
	Vector<VfsValue*> next_level;
	
	set_value(root);
	next_level.Add(&root);
	
	int branching_range = branching_high - branching_low + 1;
	
	int count = 0;
	while (count < total) {
		Vector<VfsValue*> this_level;
		this_level <<= next_level;
		next_level.Clear();
		for(int i = 0; i < this_level.GetCount(); i++) {
			VfsValue& n = *this_level[i];
			int remaining = total-count;
			if (remaining <= 0) break;
			int sub_node_count = branching_low + Random(branching_range);
			if (sub_node_count > remaining) sub_node_count = remaining;
			n.SetCount(sub_node_count);
			count += sub_node_count;
			for(int j = 0; j < sub_node_count; j++) {
				VfsValue& ptr = n.sub[j];
				ptr.id = IntStr(j);
				next_level.Add(&ptr);
				set_value(ptr);
			}
		}
	}
}

END_UPP_NAMESPACE
