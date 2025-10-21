#include "Extensions.h"

NAMESPACE_UPP


ArrayMap<TypeCls, StaticIfaceBackend>						StaticIfaceFactory::backends;


StaticIfaceBackend& StaticIfaceFactory::GetAdd(const Backend& b) {
	int i = backends.Find(b.type);
	if (i >= 0)
		return backends[i];
	else
		return backends.Add(b.type, b.new_fn());
}

StaticIfaceBackend* StaticIfaceFactory::GetReader(String ext) {
	if (ext.Left(1) == ".")
		ext = ext.Mid(1);
	int i = NewFns().Find(ext);
	if (i < 0)
		return 0;
	Vector<Backend>& fns = NewFns()[i];
	for (Backend& b : fns) {
		if (b.read)
			return &GetAdd(b);
	}
	return 0;
}

StaticIfaceBackend* StaticIfaceFactory::GetReader(TypeCls type) {
	int i = backends.Find(type);
	if (i >= 0)
		return &backends[i];
	return 0;
}

StaticIfaceBackend* StaticIfaceFactory::GetWriter(String ext) {
	ASSERT(ext.Left(1) != ".");
	int i = NewFns().Find(ext);
	if (i < 0)
		return 0;
	Vector<Backend>& fns = NewFns()[i];
	for (Backend& b : fns) {
		if (b.write)
			return &GetAdd(b);
	}
	return 0;
}




Image RenderTextBlended(Font fnt, const char* s, SysColor c) {
	TODO
	/*SysFont* raw = fnt.GetSysFont();
	if (!raw)
		return Image();
	auto r = Upp::StaticIfaceFactory::GetReader(raw->raw->backend);
	if (r)
		return r->RenderTextBlended(*raw, s, c);
	return Image();*/
	return Image();
}



END_UPP_NAMESPACE
