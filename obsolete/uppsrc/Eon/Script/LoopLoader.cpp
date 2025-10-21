#include "Script.h"

NAMESPACE_UPP
namespace Eon {


bool LoopDefinition::IsPathTrailMatch(const Vector<String>& parts) const {
	if (parts.IsEmpty() || parts.GetCount() > id.parts.GetCount())
		return false;
	for (int i = 0; i < parts.GetCount(); i++) {
		int a = parts.GetCount()-1-i;
		int b = id.parts.GetCount()-1-i;
		const String& as = parts[a];
		const String& bs = id.parts[b];
		if (as != bs)
			return false;
	}
	return true;
}




ScriptLoopLoader::ScriptLoopLoader(ScriptChainLoader& parent, int id, Eon::LoopDefinition& def) :
	Base(parent, id, def)
{
	
}

void ScriptLoopLoader::GetLoops(Vector<ScriptLoopLoader*>& v) {
	Panic("internal error");
}

String ScriptLoopLoader::GetTreeString(int indent) {
	String s;
	s.Cat('\t', indent);
	s << "Loop " << id;
	s.Cat('\n');
	
	return s;
}

void ScriptLoopLoader::SetSideSourceConnected(const AtomTypeCls& type, int ch_i, ScriptLoopLoader& sink) {
	ASSERT(type.IsValid());
	ASSERT(ch_i > 0);
	ASSERT(type.iface.src.GetCount() > 1 && ch_i < type.iface.src.GetCount());
	int side_ch_i = ch_i - 1;
	
	AtomSideLinks& atom = atom_links.Top();
	
	RTLOG("ScriptLoopLoader::SetSideSourceConnected: loop " << HexStrPtr(this) << " src ch #" << ch_i << " set " << HexStrPtr(&sink));
	ASSERT(side_ch_i >= 0 && side_ch_i < atom.src_side_conns.GetCount());
	SideLink& l = atom.src_side_conns[side_ch_i];
	if (l.link == &sink)
		return; // todo: prevent this call happening
	ASSERT(!l.link);
	if (l.link) {AddError(def.loc, "ScriptLoopLoader::SetSideSourceConnected: internal error: atom already linked"); return;}
	l.link = &sink;
	
	MACHVER_STATUS(LoopLoader_AtomLinked, this);
}

void ScriptLoopLoader::SetSideSinkConnected(const AtomTypeCls& type, int ch_i, ScriptLoopLoader& src) {
	ASSERT(type.IsValid());
	ASSERT(ch_i > 0);
	ASSERT(type.iface.sink.GetCount() > 1 && ch_i < type.iface.sink.GetCount());
	int side_ch_i = ch_i - 1;
	
	AtomSideLinks& atom = atom_links.Top();
	
	RTLOG("ScriptLoopLoader::SetSideSourceConnected: loop " << HexStrPtr(this) << " sink ch #" << ch_i << " set " << HexStrPtr(&src));
	ASSERT(side_ch_i >= 0 && side_ch_i < atom.sink_side_conns.GetCount());
	SideLink& l = atom.sink_side_conns[side_ch_i];
	if (l.link == &src)
		return; // todo: prevent this call happening
	ASSERT(!l.link);
	l.link = &src;
	
	MACHVER_STATUS(LoopLoader_AtomLinked, this);
}

bool ScriptLoopLoader::IsAllSidesConnected() const {
	ASSERT(atom_links.GetCount());
	for (const auto& atom : atom_links) {
		for (const SideLink& l : atom.src_side_conns)
			if (l.is_required && !l.link)
				return false;
		for (const SideLink& l : atom.sink_side_conns)
			if (l.is_required && !l.link)
				return false;
	}
	return true;
}

bool ScriptLoopLoader::IsTopSidesConnected() const {
	ASSERT(atom_links.GetCount());
	const auto& atom = atom_links.Top();
	int dbg_i = 0;
	for (const SideLink& l : atom.src_side_conns) {
		if (l.is_required && !l.link) {
			return false;
		}
		dbg_i++;
	}
	dbg_i = 0;
	for (const SideLink& l : atom.sink_side_conns) {
		if (l.is_required && !l.link) {
			return false;
		}
		dbg_i++;
	}
	return true;
}

void ScriptLoopLoader::UndoLoad() {
	for(int i = added_atoms.GetCount()-1; i >= 0; i--)
		added_atoms[i].a->Stop();
	for(int i = added_atoms.GetCount()-1; i >= 0; i--)
		added_atoms[i].a->UninitializeDeep();
}

bool ScriptLoopLoader::Load() {
    RTLOG("ScriptLoopLoader::Load: " << def.id.ToString());
    ScriptLoader& loader = GetLoader();

    bool has_link = !def.is_driver;

    // Target entity for atoms
    Eon::Id deep_id =
        has_link ?
            GetDeepId() :
            parent.GetDeepId();
    VfsValue* l = loader.ResolveLoop(deep_id);
    if (!l) {
        AddError(def.loc, "Could not resolve entity with deep id: " + deep_id.ToString());
        return false;
    }

    // Build using Core contexts to avoid duplication
    ChainContext cc;
    Vector<ChainContext::AtomSpec> specs;
    for (int i = 0; i < def.atoms.GetCount(); i++) {
        const Eon::AtomDefinition& a = def.atoms[i];
        ChainContext::AtomSpec& spec = specs.Add();
        spec.iface = a.iface;
        spec.link = has_link ? a.link : LinkTypeCls();
        spec.idx = id;
    }

    LoopContext& lc = cc.AddLoop(*l, specs, has_link);

    // Mirror for lifecycle and side-link compatibility
    added_atoms.SetCount(0);
    atoms.SetCount(0);
    for (const auto& it : lc.added) {
        auto& c = added_atoms.Add();
        c.a = it.a;
        c.l = it.l;
        c.iface = it.iface;
        atoms.Add(it.a);
    }

    if (has_link)
        UpdateLoopLimits();

    return true;
}

AtomBasePtr ScriptLoopLoader::AddAtomTypeCls(VfsValue& val, AtomTypeCls cls) {
	VfsValue& sub = val.Add();
	
	int i = VfsValueExtFactory::AtomDataMap().Find(cls);
	ASSERT_(i >= 0, "Invalid to create non-existant atom");
	if (i < 0) return 0;
	const auto& f = VfsValueExtFactory::AtomDataMap()[i];
	VfsValueExt* ext = f.new_fn(sub);
	AtomBasePtr obj = CastPtr<AtomBase>(ext);
	ASSERT(obj);
	if (!obj) return 0;
	
	sub.id = ToVarName(ClassPathTop(f.name));
	sub.ext = obj;
	sub.type_hash = obj->GetTypeHash();
	
	return obj;
}

LinkBasePtr ScriptLoopLoader::AddLinkTypeCls(VfsValue& val, LinkTypeCls cls) {
	VfsValue& sub = val.Add();
	
	int i = VfsValueExtFactory::LinkDataMap().Find(cls);
	ASSERT_(i >= 0, "Invalid to create non-existant atom");
	if (i < 0) return 0;
	const auto& f = VfsValueExtFactory::LinkDataMap()[i];
	VfsValueExt* ext = f.new_fn(sub);
	LinkBasePtr obj = CastPtr<LinkBase>(ext);
	
	sub.id = ToVarName(ClassPathTop(f.name));
	sub.ext = obj;
	sub.type_hash = obj->GetTypeHash();
	
	return obj;
}

void ScriptLoopLoader::UpdateLoopLimits() {
	
	// as in AtomBase::LinkSideSink
	
	bool changes = false;
	int c = added_atoms.GetCount();
	int total_max = 1000000;
	int total_min = 0;
	
	for(int i = 0; i < c; i++) {
		AddedAtom& info = added_atoms[i];
		InterfaceSourcePtr src = info.a->GetSource();
		int src_c = src->GetSourceCount();
		for(int j = 0; j < src_c; j++) {
			int src_min_packets = src->GetSourceValue(j).GetMinPackets();
			int src_max_packets = src->GetSourceValue(j).GetMaxPackets();
			total_min = max(total_min, src_min_packets);
			total_max = min(total_max, src_max_packets);
		}
		
		InterfaceSinkPtr sink = info.a->GetSink();
		int sink_c = sink->GetSinkCount();
		for(int j = 0; j < sink_c; j++) {
			int sink_min_packets = sink->GetValue(j).GetMinPackets();
			int sink_max_packets = sink->GetValue(j).GetMaxPackets();
			total_min = max(total_min, sink_min_packets);
			total_max = min(total_max, sink_max_packets);
		}
	}
	
	if (total_min > total_max) {
		total_max = total_min;
	}
	
	LOG("ScriptLoopLoader::UpdateLoopLimits: set loop limits: min=" << total_min << ", max=" << total_max);
	
	for(int i = 0; i < c; i++) {
		AddedAtom& info = added_atoms[i];
		
		InterfaceSourcePtr src = info.a->GetSource();
		InterfaceSinkPtr sink = info.a->GetSink();
		
		int sink_c = sink->GetSinkCount();
		for(int k = 0; k < sink_c; k++) {
			ValueBase& v = sink->GetValue(k);
			v.SetMinQueueSize(total_min);
			v.SetMaxQueueSize(total_max);
		}
		
		int src_c = src->GetSourceCount();
		for(int k = 0; k < src_c; k++) {
			ValueBase& v = src->GetSourceValue(k);
			v.SetMinQueueSize(total_min);
			v.SetMaxQueueSize(total_max);
		}
		
	}
}


bool ScriptLoopLoader::PostInitialize() {
	int c = added_atoms.GetCount()-1;
	for(int i = c; i >= 0; i--) {
		AddedAtom& a = added_atoms[i];
		if (!a.a->PostInitialize())
			return false;
		if (a.l && !a.l->PostInitialize())
			return false;
	}
	return true;
}

bool ScriptLoopLoader::Start() {
	int c = added_atoms.GetCount()-1;
	for(int i = c; i >= 0; i--) {
		AddedAtom& a = added_atoms[i];
		if (!a.a->Start())
			return false;
		a.a->SetRunning();
		
		if (a.l && !a.l->Start())
			return false;
	}
	return true;
}



}
END_UPP_NAMESPACE
