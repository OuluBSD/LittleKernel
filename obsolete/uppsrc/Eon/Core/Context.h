#ifndef _Eon_Core_Context_h_
#define _Eon_Core_Context_h_

namespace Eon {

class PoolContext;
class EntityContext;
class ComponentContext;
class LoopContext;
class ChainContext;

// Core helpers: resolve loop path without Script/Ast types
VfsValue* ResolveLoopPath(Engine& eng, const Vector<String>& parts);
VfsValue* ResolveLoopPath(Engine& eng, const String& dotted);

class PoolContext {
public:
    VfsValue& v;
    ErrorSource* err = nullptr;
    FileLocation loc;

    PoolContext(VfsValue& v, ErrorSource* err = nullptr, const FileLocation* loc = nullptr);

    PoolContext AddPool(const String& name, const ArrayMap<String, Value>* args = nullptr, const FileLocation* loc = nullptr);
    EntityContext AddEntity(const String& name, const ArrayMap<String, Value>* args = nullptr, const FileLocation* loc = nullptr);
    String GetTreeString(int indent=0) const;
};

class EntityContext {
public:
    Entity& ent;
    ErrorSource* err = nullptr;

    EntityContext(Entity& e, ErrorSource* err = nullptr);

    ComponentContext AddComponent(const String& comp_id, const ArrayMap<String, Value>* args = nullptr, const FileLocation* loc = nullptr);
    String GetTreeString(int indent=0) const;
};

class ComponentContext {
public:
    Component* comp = nullptr;
    ErrorSource* err = nullptr;

    ComponentContext() = default;
    ComponentContext(Component& c, ErrorSource* err = nullptr) : comp(&c), err(err) {}
    String GetTreeString(int indent=0) const;
};

class LoopContext {
public:
    VfsValue& space; // loop/space owner where atoms/links are placed

    struct AddedAtom : Moveable<AddedAtom> {
        AtomBasePtr a;
        LinkBasePtr l;
        IfaceConnTuple iface;
    };

    Vector<AddedAtom> added;

    LoopContext(VfsValue& space);

    AtomBasePtr AddAtom(AtomTypeCls atom, LinkTypeCls link, const IfaceConnTuple& iface, const ArrayMap<String, Value>* args = nullptr, int idx = -1);
    bool MakePrimaryLinks();
    bool PostInitializeAll();
    bool StartAll();
    void UndoAll();
    String GetTreeString(int indent=0) const;

    static bool ConnectSides(const LoopContext& loop0, const LoopContext& loop1);
};

class ChainContext {
public:
    struct AtomSpec : Moveable<AtomSpec> {
        String action;                       // e.g., "audio.capture", must map to a registered atom
        ArrayMap<String, Value> args;        // key/value args for the atom
        IfaceConnTuple iface;                // realized interface (with side-link info)
        LinkTypeCls link;                    // optional explicit link type; if invalid, resolve from action
        int idx = -1;                        // optional id/index for the atom/link
    };

    Array<LoopContext> loops;

    // Helper: resolve action to AtomTypeCls and its default LinkTypeCls. Returns false if not found.
    static bool ResolveAction(const String& action, AtomTypeCls& out_atom, LinkTypeCls& out_link);

    // Add a loop at a given space node with a sequence of AtomSpec; makes primary links if requested.
    LoopContext& AddLoop(VfsValue& loop_space, const Vector<AtomSpec>& atoms, bool make_primary_links = true);
    bool PostInitializeAll();
    bool StartAll();
    void UndoAll();
    String GetTreeString(int indent=0) const;
};

}

#endif
