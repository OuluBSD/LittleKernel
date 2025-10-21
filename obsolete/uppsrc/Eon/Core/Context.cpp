#include "Core.h"

NAMESPACE_UPP
namespace Eon {

static String Indent(int n) { String s; s.Cat('\t', n); return s; }

static String PathOf(VfsValue& v) {
    Vector<String> parts;
    VfsValue* it = &v;
    while (it) {
        if (it->id.GetCount())
            parts.Add(it->id);
        VfsValue* owner = it->FindOwnerNull();
        if (!owner) break;
        it = owner;
    }
    String out;
    for (int i = parts.GetCount() - 1; i >= 0; i--) {
        if (!out.IsEmpty()) out << ".";
        out << parts[i];
    }
    return out;
}

static bool MakePrimaryLink(VfsValue& space, AtomBasePtr src_atom, AtomBasePtr dst_atom) {
    InterfaceSourcePtr src = src_atom->GetSource();
    ExchangeSourceProviderPtr src_ep = CastPtr<ExchangeSourceProvider>(&*src);
    if (!src_ep)
        return false;

    auto sink = dst_atom->GetSink();
    Ptr<ExchangeSinkProvider> sinkT = dst_atom->GetSinkT<ExchangeSinkProvider>();
    if (!src || !sink || !sinkT)
        return false;

    int src_ch = 0;
    int sink_ch = 0;

    ValueFormat src_fmt = src->GetSourceValue(src_ch).GetFormat();
    ValueFormat sink_fmt = sink->GetValue(sink_ch).GetFormat();
    if (src_fmt.vd != sink_fmt.vd)
        return false;

    ASSERT(src_atom != dst_atom);
    ASSERT(src_atom->GetLink() != dst_atom->GetLink());

    CookiePtr src_cookie, sink_cookie;
    if (!src->Accept(sinkT, src_cookie, sink_cookie))
        return false;

    // create exchange point in owning space
    auto& sdmap = VfsValueExtFactory::IfaceLinkDataMap();
    int i = sdmap.Find(src_fmt.vd);
    if (i < 0)
        return false;
    const auto& src_d = sdmap[i];
    if (src_d.vd != src_fmt.vd)
        return false;

    String name = src_d.name;
    hash_t type_hash = src_d.type_hash;
    auto owner_space = space.FindOwnerNull();
    if (!type_hash || !owner_space)
        return false;

    VfsValue& link_val = owner_space->Add(name, type_hash);
    ExchangePointPtr ep = link_val.FindExt<ExchangePoint>();
    if (!ep)
        return false;

    src->Link(ep, sinkT, src_cookie, sink_cookie);
    ep->Init(owner_space);
    ep->Set(src_ep, sinkT, src_cookie, sink_cookie);
    src_atom->GetLink()->SetPrimarySink(dst_atom->GetLink());
    dst_atom->GetLink()->SetPrimarySource(src_atom->GetLink());
    return true;
}

VfsValue* ResolveLoopPath(Engine& eng, const Vector<String>& parts) {
    VfsValue* l1 = &eng.GetRootLoop();
    VfsValue* result = nullptr;
    for (int i = 0; i < parts.GetCount(); i++) {
        const String& part = parts[i];
        bool last = i == parts.GetCount() - 1;
        if (last)
            result = &l1->GetAdd(part, 0);
        else
            l1 = &l1->GetAdd(part, 0);
    }
    return result ? result : l1;
}

VfsValue* ResolveLoopPath(Engine& eng, const String& dotted) {
    Vector<String> parts = Split(dotted, ".");
    return ResolveLoopPath(eng, parts);
}

PoolContext::PoolContext(VfsValue& v, ErrorSource* err, const FileLocation* loc)
    : v(v), err(err) {
    if (loc)
        this->loc = *loc;
}

PoolContext PoolContext::AddPool(const String& name, const ArrayMap<String, Value>* args, const FileLocation* loc) {
    VfsValue& pool0 = v.GetAdd(name, 0);
    (void)args; // args reserved for future pool-level properties
    return PoolContext(pool0, err, loc ? loc : &this->loc);
}

EntityContext PoolContext::AddEntity(const String& name, const ArrayMap<String, Value>* args, const FileLocation* /*loc*/) {
    Entity& ent = v.GetAdd<Entity>(name);
    (void)args; // entity-level args optional; components carry most args
    return EntityContext(ent, err);
}

String PoolContext::GetTreeString(int indent) const {
    String s;
    s << Indent(indent) << "PoolContext path: " << PathOf(const_cast<VfsValue&>(v)) << '\n';
    return s;
}

EntityContext::EntityContext(Entity& e, ErrorSource* err)
    : ent(e), err(err) {}

ComponentContext EntityContext::AddComponent(const String& comp_id, const ArrayMap<String, Value>* args, const FileLocation* /*loc*/) {
    ComponentPtr cb = ent.CreateEon(comp_id);
    if (!cb)
        return ComponentContext();
    if (args) {
        for (int i = 0; i < args->GetCount(); i++) {
            String key = args->GetKey(i);
            const Value& value = args->operator[](i);
            if (!value.IsVoid())
                cb->Arg(key, value);
        }
    }
    return ComponentContext(*cb, err);
}

String EntityContext::GetTreeString(int indent) const {
    String s;
    s << Indent(indent) << "EntityContext: " << ent.val.id << '\n';
    return s;
}

String ComponentContext::GetTreeString(int indent) const {
    String s;
    s << Indent(indent) << "ComponentContext: ";
    if (comp)
        s << comp->GetTypeName();
    else
        s << "<null>";
    s << '\n';
    return s;
}

LoopContext::LoopContext(VfsValue& space)
    : space(space) {}

AtomBasePtr LoopContext::AddAtom(AtomTypeCls atom, LinkTypeCls link, const IfaceConnTuple& iface, const ArrayMap<String, Value>* args, int idx) {
    // create atom
    VfsValue& sub_atom = space.Add();
    int ai = VfsValueExtFactory::AtomDataMap().Find(atom);
    if (ai < 0) return nullptr;
    const auto& af = VfsValueExtFactory::AtomDataMap()[ai];
    VfsValueExt* aext = af.new_fn(sub_atom);
    AtomBasePtr ab = CastPtr<AtomBase>(aext);
    if (!ab) return nullptr;
    sub_atom.id = ToVarName(ClassPathTop(af.name));
    sub_atom.ext = ab;
    sub_atom.type_hash = ab->GetTypeHash();

    LinkBasePtr lb;
    if (link.IsValid()) {
        VfsValue& sub_link = space.Add();
        int li = VfsValueExtFactory::LinkDataMap().Find(link);
        if (li >= 0) {
            const auto& lf = VfsValueExtFactory::LinkDataMap()[li];
            VfsValueExt* lext = lf.new_fn(sub_link);
            lb = CastPtr<LinkBase>(lext);
            sub_link.id = ToVarName(ClassPathTop(lf.name));
            sub_link.ext = lb;
            sub_link.type_hash = lb->GetTypeHash();
        }
        if (!lb) return nullptr;
        lb->atom = CastPtr<Atom>(&*ab);
        ab->link = &*lb;
        if (idx >= 0) lb->SetId(idx);
    }

    if (idx >= 0) ab->SetIdx(idx);

    // set interface and initialize
    ab->SetInterface(iface);
    WorldState ws;
    if (args) {
        for (int i = 0; i < args->GetCount(); i++) {
            String key = args->GetKey(i);
            const Value& obj = args->operator[](i);
            ws.values.GetAdd("." + key) = obj;
        }
    }
    if (!ab->InitializeAtom(ws) || !ab->Initialize(ws))
        return nullptr;
    ab->SetInitialized();
    if (lb) {
        if (!lb->Initialize(ws))
            return nullptr;
        lb->SetInitialized();
    }

    AddedAtom& rec = added.Add();
    rec.a = ab;
    rec.l = lb;
    rec.iface = iface;
    return ab;
}

bool LoopContext::MakePrimaryLinks() {
    if (added.IsEmpty() || added.GetCount() == 1)
        return true;
    for (int i = 0; i < added.GetCount(); i++) {
        AddedAtom& src = added[i];
        AddedAtom& dst = added[(i + 1) % added.GetCount()];
        if (!MakePrimaryLink(space, src.a, dst.a))
            return false;
    }
    return true;
}

bool LoopContext::ConnectSides(const LoopContext& loop0, const LoopContext& loop1) {
    for (const auto& sink_info : loop0.added) {
        LinkBasePtr sink_link = sink_info.a->GetLink();
        const IfaceConnTuple& sink_iface = sink_info.iface;
        for (int sink_ch = 1; sink_ch < sink_iface.type.iface.sink.GetCount(); sink_ch++) {
            const IfaceConnLink& sink_conn = sink_iface.sink[sink_ch];
            if (sink_conn.conn < 0 && sink_iface.type.IsSinkChannelOptional(sink_ch))
                continue;
            bool found = false;
            for (const auto& src_info : loop1.added) {
                LinkBasePtr src_link = src_info.a->GetLink();
                const IfaceConnTuple& src_iface = src_info.iface;
                for (int src_ch = 1; src_ch < src_iface.type.iface.src.GetCount(); src_ch++) {
                    const IfaceConnLink& src_conn = src_iface.src[src_ch];
                    if (src_conn.conn < 0 && src_iface.type.IsSourceChannelOptional(src_ch))
                        continue;
                    if (sink_conn.conn == src_conn.conn) {
                        int src_ch_i = src_conn.local;
                        int sink_ch_i = sink_conn.local;
                        if (!src_link->LinkSideSink(sink_link, src_ch_i, sink_ch_i))
                            return false;
                        found = true;
                        break;
                    }
                }
                if (found) break;
            }
            // It is allowed that optional channels remain unconnected
            if (!found && !(sink_conn.conn < 0 && sink_iface.type.IsSinkChannelOptional(sink_ch)))
                return false;
        }
    }
    return true;
}

bool LoopContext::PostInitializeAll() {
    int c = added.GetCount() - 1;
    for (int i = c; i >= 0; i--) {
        const AddedAtom& a = added[i];
        if (!a.a->PostInitialize())
            return false;
        if (a.l && !a.l->PostInitialize())
            return false;
    }
    return true;
}

bool LoopContext::StartAll() {
    int c = added.GetCount() - 1;
    for (int i = c; i >= 0; i--) {
        const AddedAtom& a = added[i];
        if (!a.a->Start())
            return false;
        a.a->SetRunning();
        if (a.l && !a.l->Start())
            return false;
    }
    return true;
}

void LoopContext::UndoAll() {
    for (int i = added.GetCount() - 1; i >= 0; i--)
        added[i].a->Stop();
    for (int i = added.GetCount() - 1; i >= 0; i--)
        added[i].a->UninitializeDeep();
}

String LoopContext::GetTreeString(int indent) const {
    String s;
    s << Indent(indent) << "LoopContext space: " << PathOf(const_cast<VfsValue&>(space)) << '\n';
    int i = 0;
    for (const auto& a : added) {
        s << Indent(indent+1) << i++ << ": Atom " << a.iface.type.ToString();
        if (a.l)
            s << ", link: " << a.l->GetTypeName();
        s << '\n';
    }
    return s;
}

static bool FindAtomByAction(const String& action, AtomTypeCls& out_atom, LinkTypeCls& out_link) {
    const auto& map = VfsValueExtFactory::AtomDataMap();
    for (const auto& data : map.GetValues()) {
        bool match = false;
        for (const String& a : data.actions) {
            if (a == action) { match = true; break; }
        }
        if (!match) continue;
        out_atom = data.cls;
        out_link = data.link_type;
        return out_atom.IsValid();
    }
    return false;
}

bool ChainContext::ResolveAction(const String& action, AtomTypeCls& out_atom, LinkTypeCls& out_link) {
    return FindAtomByAction(action, out_atom, out_link);
}

LoopContext& ChainContext::AddLoop(VfsValue& loop_space, const Vector<AtomSpec>& atoms, bool make_primary_links) {
    LoopContext& lc = loops.Add(new LoopContext(loop_space));
    int idx = 0;
    for (const auto& spec : atoms) {
        AtomTypeCls atom;
        LinkTypeCls link;
        if (!spec.action.IsEmpty()) {
            if (!FindAtomByAction(spec.action, atom, link)) {
                // unresolved action; fail fast by skipping add
                return lc;
            }
        }
        else {
            atom = spec.iface.type; // allow caller to pre-set
        }
        if (spec.link.IsValid())
            link = spec.link;
        int use_idx = spec.idx >= 0 ? spec.idx : idx;
        lc.AddAtom(atom, link, spec.iface, &spec.args, use_idx);
        idx++;
    }
    if (make_primary_links)
        lc.MakePrimaryLinks();
    return lc;
}

bool ChainContext::PostInitializeAll() {
    for (auto& lc : loops)
        if (!lc.PostInitializeAll())
            return false;
    return true;
}

bool ChainContext::StartAll() {
    for (auto& lc : loops)
        if (!lc.StartAll())
            return false;
    return true;
}

void ChainContext::UndoAll() {
    for (int i = loops.GetCount() - 1; i >= 0; i--)
        loops[i].UndoAll();
}

String ChainContext::GetTreeString(int indent) const {
    String s;
    s << Indent(indent) << "ChainContext\n";
    for (const auto& lc : loops)
        s << lc.GetTreeString(indent+1);
    return s;
}

}
END_UPP_NAMESPACE
