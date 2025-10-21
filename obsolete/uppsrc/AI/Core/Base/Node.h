#ifndef _AICore2_Node_h_
#define _AICore2_Node_h_


template <class T>
struct Node {
	VfsValue* n = 0;
	
	Node() {}
	Node(VfsValue& n) : n(&n) {}
	Node(T& n) : n(&n.val) {}
	T* operator->() const {
		if (!n || !n->ext) return 0;
		T* o = CastPtr<T>(&*n->ext);
		ASSERT(o);
		return o;
	}
	VfsValue& operator()() const {
		ASSERT(n);
		return *n;
	}
	operator VfsValue&() const {
		ASSERT(n);
		return *n;
	}
	operator T&() const {
		ASSERT(n && n->ext);
		T* o = CastPtr<T>(&*n->ext);
		ASSERT(o);
		return *o;
	}
	operator T*() const {
		if (n && !n->ext.IsEmpty())
			return CastPtr<T>(&*n->ext);
		return 0;
	}
};

void GenerateTree(VfsValue& root, int total, int branching_low, int branching_high, Event<VfsValue&> set_value);


#endif
