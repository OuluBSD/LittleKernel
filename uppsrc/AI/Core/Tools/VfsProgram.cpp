#include "Tools.h"


NAMESPACE_UPP


void VfsProgramIteration::Visit(Vis& v) {
	v.Ver(1)
	(1) VIS_(code)
		VIS_(global)
		VIS_(vfsvalue)
		VIS_(log)
		;
}

void VfsProgramSession::Visit(Vis& v) {
	
}

void VfsProgramProject::Visit(Vis& v) {
	v.Ver(1)
	(1)	VIS_(code);
}

bool VfsProgramProject::GetAll(Vector<VirtualNode>& vec) {
	return false;
	#if 0
	#if 0
	VfsValue& n = val;
	v.Reserve(n.sub.GetCount());
	for(int i = 0; i < n.sub.GetCount(); i++) {
		auto& sub = n.sub[i];
		if (sub.id.IsEmpty())
			v.Add().Create(/*data->path +*/VfsPath() + (i), &sub);
		else
			v.Add().Create(/*data->path +*/VfsPath() + (Value)sub.id, &sub);
	}
	#else
	Value& v = val.value;
	if (v.IsNull())
		v = ValueMap();
	if (v.Is<ValueMap>()) {
		ValueMap map = v;
		vec.Reserve(map.GetCount());
		for(int i = 0; i < map.GetCount(); i++) {
			Value& val = const_cast<Value&>(map.GetValue(i));
			if (val.Is<ValueMap>()) {
				Value key = map.GetKey(i);
				auto& o = vec.Add().CreateValue(/*data->path +*/ VfsPath() + key, &val, key);
				ASSERT(o.poly_value->Is<ValueMap>());
			}
		}
	}
	else if (v.Is<ValueArray>()) {
		ValueArray arr = v;
		vec.Reserve(arr.GetCount());
		for(int i = 0; i < arr.GetCount(); i++) {
			Value& val = arr.At(i);
			if (val.Is<ValueMap>()) {
				vec.Add().CreateValue(/*data->path +*/ VfsPath() + i, &val, i);
			}
		}
	}
	/*else if (v.Is<AstValue>()) {
		TODO
	}
	else {
		LOG(v.GetTypeName());
		LOG(v.ToString());
		TODO;
	}*/
	#endif
	return true;
	#endif
}



VfsProgram::VfsProgram(VfsValue& owner) : Component(owner) {
	
}

bool VfsProgram::Initialize(const WorldState& ws) {
	AddToUpdateList();
	return true;
}

void VfsProgram::Uninitialize() {
	RemoveFromUpdateList();
}

void VfsProgram::Visit(Vis& v) {
	v
		VIS_(formxml)
		VIS_(formxml_compressed);
}

void VfsProgram::Update(double dt) {
	
	if (running) {
		TODO
	}
	
}

VirtualNode VfsProgram::RealizePath(const VfsPath& path, hash_t type_hash) {
	VirtualNode root = val.RootPolyValue();
	VirtualNode cur = root;
	for(int i = 0; i < path.Parts().GetCount()-1; i++) {
		const auto& part = path.Parts()[i];
		cur = cur.GetAdd(part, 0);
	}
	cur = cur.GetAdd(path.Parts().Top(), type_hash);
	ASSERT(cur.IsValue());
	return cur;
}



INITIALIZER_VFSEXT(VfsProgramIteration, "", "");
INITIALIZER_VFSEXT(VfsProgramSession, "", "");
INITIALIZER_VFSEXT(VfsProgramProject, "", "");
INITIALIZER_COMPONENT(VfsProgram, "vfs.program", "Vfs|Program")
INITIALIZER_COMPONENT(VfsForm, "", "")

END_UPP_NAMESPACE
