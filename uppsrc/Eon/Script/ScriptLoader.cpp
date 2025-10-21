#include "Script.h"
// For Core contexts (AST-free building)
#include <Eon/Core/Core.h>

#define VERBOSE_SCRIPT_LOADER 0

NAMESPACE_UPP

extern void (*Eon_PostLoadString)(Engine& eng, String script_str);
extern bool (*Eon_AddScriptLoader)(Engine& eng);

namespace Eon {

void PostLoadString(Engine& eng, String script_str) {
	Ptr<Eon::ScriptLoader> script = eng.FindAdd<Eon::ScriptLoader>();
	if (script)
		script->PostLoadString(script_str);
}

bool AddScriptLoader(Engine& eng) {
	Ptr<Eon::ScriptLoader> script = eng.FindAdd<Eon::ScriptLoader>();
	if (!script) {
		LOG("No ScriptLoader added to machine and the machine is already started");
		return false;
	}
	
	#if 0
	#ifdef flagGUI
    Gu::GuboSystemPtr gubo	= mach.FindAdd<Gu::GuboSystem>();
    WindowSystemPtr win		= mach.FindAdd<WindowSystem>();
    #endif
    #endif
    return true;
}

INITBLOCK {
	Eon_PostLoadString = &PostLoadString;
	Eon_AddScriptLoader = &AddScriptLoader;
}


String Id::ToString() const {
	String s;
	for(const String& part : parts) {
		if (!s.IsEmpty())
			s << ".";
		s << part;
	}
	return s;
}






int ScriptLoader::loop_counter = 0;


ScriptLoader::ScriptLoader(VfsValue& n) :
	System(n),
	ErrorSource("ScriptLoader") {
	
}

ScriptLoader::~ScriptLoader() {
	
}

void ScriptLoader::Visit(Vis& v) {
	if (!loader.IsEmpty())
		v("loader",*loader,VISIT_NODE);
	
	_VIS_(tmp_side_id_counter)
	 VIS_(post_load_file)
	 VIS_(post_load_string)
	 VISN(cunit)
	 VIS_(collect_errors);
}

void ScriptLoader::LogMessage(ProcMsg msg) {
	LOG(msg.ToString());
}

bool ScriptLoader::Initialize(const WorldState& ws) {
	
	if (!WhenMessage)
		WhenMessage << THISBACK(LogMessage);
	
	RTLOG("ScriptLoader::Initialize success!");
	return true;
}

bool ScriptLoader::PostInitialize() {
	if (!DoPostLoad())
		return false;
	
	return true;
}

bool ScriptLoader::DoPostLoad() {
	bool success = true;
	
	while (!post_load_file.IsEmpty()) {
		String path = post_load_file[0];
		post_load_file.Remove(0);
		
		if (!LoadFile(path))
			success = false;
	}
	
	while (!post_load_string.IsEmpty()) {
		String s = post_load_string[0];
		post_load_string.Remove(0);
		
		if (!Load(s, "input"))
			success = false;
	}
	
	return success;
}

void ScriptLoader::Update(double dt) {
	
	if (!DoPostLoad()) {
		GetEngine().SetNotRunning();
	}
	
}

void ScriptLoader::Uninitialize() {
	//es.Clear();
	//ss.Clear();
	loader.Clear();
}

bool ScriptLoader::LoadFile(String path) {
	if (!FileExists(path)) {
		LOG("Could not find EON file");
		return false;
	}
	String eon = UPP::LoadFile(path);
	return Load(eon, path);
}

bool ScriptLoader::Load(const String& content, const String& filepath) {
	RTLOG("ScriptLoader::Load: Loading \"" << filepath << "\"");
	
	WhenEnterScriptLoad(*this);
	
	Compiler& c = val.GetAdd<Compiler>("cu");
	AstNode* root = c.CompileAst(content, filepath, true);
	
	if (!root) {
		RTLOG(GetLineNumStr(content, 1));
		WhenLeaveScriptLoad();
		return false;
	}
	return LoadAst(root);
}

bool ScriptLoader::LoadAst(AstNode* root) {
	if (!LoadCompilationUnit(root)) {
		LOG("error dump:");
		if (loader) loader->Dump();
		
		Cleanup();
		WhenLeaveScriptLoad();
		return false;
	}
	
	if (!ImplementScript()) {
		Cleanup();
		WhenLeaveScriptLoad();
		return false;
	}
	
	
	if (!loader->LoadEcs()) {
		AddError(root->loc, "ecs loading failed: " + loader->GetErrorString());
		return false;
	}
	
	
	
	Cleanup();
	WhenLeaveScriptLoad();
	
	return true;
}

bool ScriptLoader::BuildChain(const Eon::ChainDefinition& chain) {
    ChainContext cc;

    // Build each loop under its absolute id (loop.id is already resolved during parsing)
    for (const Eon::LoopDefinition& loop_def : chain.loops) {
        Eon::Id deep_id = loop_def.id; // absolute id path
        VfsValue* l = ResolveLoop(deep_id);
        if (!l) {
            AddError(loop_def.loc, String("Could not resolve loop id: ") + deep_id.ToString());
            return false;
        }

        Vector<ChainContext::AtomSpec> specs;
        bool has_link = !loop_def.is_driver;
        for (const Eon::AtomDefinition& a : loop_def.atoms) {
            ChainContext::AtomSpec& s = specs.Add();
            s.iface = a.iface;            // includes side-link conn field assignments
            s.link = has_link ? a.link : LinkTypeCls();
            s.args <<= a.args;           // copy args
        }

        cc.AddLoop(*l, specs, has_link);
    }

    // Connect side-links across loops in this chain according to conn ids
    for (int i = 0; i < cc.loops.GetCount(); i++)
        for (int j = 0; j < cc.loops.GetCount(); j++)
            if (i != j)
                if (!LoopContext::ConnectSides(cc.loops[i], cc.loops[j]))
                    return false;

    return true;
}

void ScriptLoader::Cleanup() {
	loader.Clear();
}

bool ScriptLoader::ImplementScript() {
    RTLOG("ScriptLoader::ImplementScript: load states");
    Vector<ScriptStateLoader*> states;
    loader->GetStates(states);
    for (ScriptStateLoader* dl: states)
        if (!dl->Load())
            return false;

    if (!eager_build_chains || built_chains.IsEmpty()) {
        RTLOG("ScriptLoader::ImplementScript: build chains (loops)");
        if (!loader->Load())
            return false;

        // Collect loop pointers created by chain loaders for post steps
        Vector<ScriptLoopLoader*> loops;
        loader->GetLoops(loops);
	
        RTLOG("ScriptLoader::ImplementScript: connect sides");
        for (ScriptLoopLoader* loop0 : loops) {
            for (ScriptLoopLoader* loop1 : loops) {
                if (loop0 != loop1) {
                    if (!ConnectSides(*loop0, *loop1)) {
                        AddError(loop0->def.loc, "Side connecting failed");
                        return false;
                    }
                }
            }
        }
	
        RTLOG("ScriptLoader::ImplementScript: loop post initialize");
        for (ScriptLoopLoader* ll : loops) {
            if (!ll->PostInitialize())
                return false;
        }

        RTLOG("ScriptLoader::ImplementScript: loop start");
        for (ScriptLoopLoader* ll : loops) {
            if (!ll->Start())
                return false;
        }
    } else {
        RTLOG("ScriptLoader::ImplementScript: eager mode: connect sides across built chains");
        for (int i = 0; i < built_chains.GetCount(); i++) {
            ChainContext& A = *built_chains[i];
            for (int j = 0; j < built_chains.GetCount(); j++) if (i != j) {
                ChainContext& B = *built_chains[j];
                for (auto& la : A.loops)
                    for (auto& lb : B.loops)
                        if (!LoopContext::ConnectSides(la, lb))
                            return false;
            }
        }

        RTLOG("ScriptLoader::ImplementScript: eager mode: post initialize");
        for (int i = 0; i < built_chains.GetCount(); i++) {
            ChainContext& C = *built_chains[i];
            if (!C.PostInitializeAll()) {
                for (int k = i - 1; k >= 0; k--) built_chains[k]->UndoAll();
                return false;
            }
        }

        RTLOG("ScriptLoader::ImplementScript: eager mode: start");
        for (int i = 0; i < built_chains.GetCount(); i++) {
            ChainContext& C = *built_chains[i];
            if (!C.StartAll()) {
                for (int k = i; k >= 0; k--) built_chains[k]->UndoAll();
                return false;
            }
        }
    }
	
	return true;
}

bool ScriptLoader::GetPathId(Eon::Id& script_id, AstNode* from, AstNode* to) {
	Vector<Endpoint> path;
	
	if (from == to) {
		if (to->val.id.GetCount())
			path.Add(to);
	}
	else {
		AstNode* iter = to;
		while (iter && iter != from) {
			if (iter->val.id.GetCount() || iter == to) {
				path.Add(iter);
			}
			iter = iter->val.owner ? iter->val.owner->FindExt<AstNode>() : 0;
		}
	}
	if (path.IsEmpty()) {
		AddError(to->loc, "internal error: empty path");
		return false;
	}
	for (int i = path.GetCount()-1; i >= 0; i--) {
		AstNode* id = path[i].n;
		ASSERT(id->val.id.GetCount());
		script_id.parts.Add(id->val.id);
	}
	
	return true;
}

bool ScriptLoader::LoadCompilationUnit(AstNode* root) {
	loader.Clear();
	
	if (!LoadGlobalScope(cunit.glob, root))
		return false;
	
	loader = new ScriptSystemLoader(*this, 0, cunit.glob);
	
	return true;
}

bool ScriptLoader::LoadGlobalScope(Eon::GlobalScope& def, AstNode* n) {
	ASSERT(n);
	if (!n) return false;
	
	
	// Serial machine part
	Vector<Endpoint> items;
	n->FindAllNonIdEndpoints(items, Cursor_MetaDecl);
	Sort(items, AstNodeLess());
	
	bool has_machine = false;
	for (const Endpoint& ep : items) {
		AstNode* item = ep.n;
		if (item->src == Cursor_MachineDecl) {
			AstNode* block = item->Find(Cursor_CompoundStmt);
			if (!block) {AddError(n->loc, "internal error: no stmt block"); return false;}
			
			Eon::MachineDefinition& mach_def = def.machs.Add();
			
			if (!GetPathId(mach_def.id, n, item))
				return false;
			
			ASSERT(!mach_def.id.IsEmpty());
			if (!LoadMachine(mach_def, block))
				return false;
			has_machine = true;
		}
		else if (item->src == Cursor_EngineStmt) {
			TODO
		}
	}
	
	if (!has_machine) {
		Eon::MachineDefinition& mach = def.machs.Add();
		return LoadMachine(mach, n);
	}
	
	
	// Entity machine / ecs-engine part
	items.SetCount(0);
	n->FindAllNonIdEndpoints(items, Cursor_EcsStmt);
	Sort(items, AstNodeLess());
	
	bool has_world = false;
	for (const Endpoint& ep : items) {
		AstNode* item = ep.n;
		#if VERBOSE_SCRIPT_LOADER
		LOG(item->GetTreeString(0));
		#endif
		if (item->src == Cursor_WorldStmt) {
			AstNode* block = item->Find(Cursor_CompoundStmt);
			if (!block) {AddError(n->loc, "internal error: no stmt block"); return false;}
			
			Eon::WorldDefinition& world_def = def.worlds.Add();
			
			if (!GetPathId(world_def.id, n, item))
				return false;
			
			ASSERT(!world_def.id.IsEmpty());
			if (!LoadWorld(world_def, block))
				return false;
			has_world = true;
		}
		else if (item->src == Cursor_SystemStmt) {
			TODO
		}
	}
	
	
	return true;
}

bool ScriptLoader::LoadMachine(Eon::MachineDefinition& def, AstNode* n) {
	#if VERBOSE_SCRIPT_LOADER
	LOG(n->GetTreeString());
	#endif
	
	Vector<Endpoint> items;
	n->FindAllNonIdEndpoints2(items, Cursor_EcsStmt, Cursor_OldEcsStmt);
	Sort(items, AstNodeLess());
	
	if (items.IsEmpty()) {
		AddError(def.loc, "empty node");
		return false;
	}
	
	Eon::ChainDefinition* anon_chain = 0;
	
	bool has_chain = false;
	for (const Endpoint& ep : items) {
		AstNode* item = ep.n;
		if (item->src == Cursor_ChainStmt) {
			Eon::ChainDefinition& chain_def = def.chains.Add();
			
			if (!GetPathId(chain_def.id, n, item))
				return false;
			
			ASSERT(!chain_def.id.IsEmpty());
			
			AstNode* block = item->Find(Cursor_CompoundStmt);
			if (!block) {AddError(n->loc, "internal error: no stmt block"); return false;}
			
			Vector<Endpoint> items;
			block->FindAllNonIdEndpoints(items, Cursor_ChainStmt);
			if (items.IsEmpty()) {
				if (!LoadChain(chain_def, block))
					return false;
			} else {
				if (!LoadTopChain(chain_def, block))
					return false;
			}
			has_chain = true;
		}
		else if (item->src == Cursor_DriverStmt || item->src == Cursor_LoopStmt) {
			if (!anon_chain)
				anon_chain = &def.chains.Add();
			
			if (!LoadChain(*anon_chain, item))
				return false;
			has_chain = true;
		}
	}
	
	if (!has_chain) {
		Eon::ChainDefinition& chain = def.chains.Add();
		return LoadChain(chain, n);
	}
	
	return true;
}

bool ScriptLoader::LoadWorld(Eon::WorldDefinition& def, AstNode* n) {
	Vector<Endpoint> items;
	n->FindAllNonIdEndpoints(items, Cursor_EcsStmt);
	Sort(items, AstNodeLess());
	
	if (items.IsEmpty()) {
		AddError(def.loc, "empty node");
		return false;
	}
	
	Eon::PoolDefinition* anon_pool = 0;
	
	bool has_chain = false;
	for (const Endpoint& ep : items) {
		AstNode* item = ep.n;
		if (item->src == Cursor_PoolStmt) {
			Eon::PoolDefinition& pool_def = def.pools.Add();
			
			if (!GetPathId(pool_def.id, n, item))
				return false;
			
			ASSERT(!pool_def.id.IsEmpty());
			
			AstNode* block = item->Find(Cursor_CompoundStmt);
			if (!block) {AddError(n->loc, "internal error: no stmt block"); return false;}
			
			Vector<Endpoint> items;
			block->FindAllNonIdEndpoints(items, Cursor_PoolStmt);
			if (items.IsEmpty()) {
				if (!LoadPool(pool_def, block))
					return false;
			} else {
				if (!LoadTopPool(pool_def, block))
					return false;
			}
			has_chain = true;
		}
		else if (item->src == Cursor_SystemStmt) {
			Eon::EcsSysDefinition& sys_def = def.systems.Add();
			
			if (!GetPathId(sys_def.id, n, item))
				return false;
			
			ASSERT(!sys_def.id.IsEmpty());
			
			if (!LoadEcsSystem(sys_def, item))
				return false;
			has_chain = true;
		}
	}
	
	return true;
}

bool ScriptLoader::LoadDriver(Eon::DriverDefinition& def, AstNode* n) {
	
	TODO
	return false;
}

bool ScriptLoader::LoadTopChain(Eon::ChainDefinition& def, AstNode* n) {
	
	TODO
	return false;
}

bool ScriptLoader::LoadEcsSystem(Eon::EcsSysDefinition& def, AstNode* n) {
	
	if (!LoadArguments(def.args, n))
		return false;
	
	return true;
}

bool ScriptLoader::LoadPool(Eon::PoolDefinition& def, AstNode* n) {
	const auto& map = VfsValueExtFactory::AtomDataMap();
	Vector<Endpoint> items;
	
	n->FindAll(items, Cursor_EntityStmt);
	n->FindAll(items, Cursor_PoolStmt);
	Sort(items, AstNodeLess());
	
	for (Endpoint& ep : items) {
		AstNode* item = ep.n;
		if (item->src == Cursor_PoolStmt) {
			Eon::PoolDefinition& pool_def = def.pools.Add();
			pool_def.loc = item->loc;
			
			if (!GetPathId(pool_def.id, n, item))
				return false;
			
			if (!LoadPool(pool_def, item))
				return false;
		}
		else if (item->src == Cursor_EntityStmt) {
			Eon::EntityDefinition& ent_def = def.ents.Add();
			ent_def.loc = item->loc;
			
			if (!GetPathId(ent_def.id, n, item))
				return false;
			
			if (!LoadEntity(ent_def, item))
				return false;
		}
	}
	
	return true;
}

bool ScriptLoader::LoadTopPool(Eon::PoolDefinition& def, AstNode* n) {
	
	TODO
	return false;
}

bool ScriptLoader::LoadState(Eon::StateDeclaration& def, AstNode* n) {
	
	TODO
	return false;
}

bool ScriptLoader::LoadEntity(Eon::EntityDefinition& def, AstNode* n) {
	#if VERBOSE_SCRIPT_LOADER
	LOG(n->GetTreeString(0));
	#endif
	
	const auto& map = VfsValueExtFactory::AtomDataMap();
	Vector<Endpoint> items;
	
	n->FindAll(items, Cursor_ComponentStmt);
	Sort(items, AstNodeLess());
	
	for (Endpoint& ep : items) {
		AstNode* item = ep.n;
		Eon::ComponentDefinition& comp_def = def.comps.Add();
		comp_def.loc = item->loc;
		
		if (!GetPathId(comp_def.id, n, item))
			return false;
		
		if (!LoadComponent(comp_def, item))
			return false;
	}
	
	if (!LoadArguments(def.args, n))
		return false;
	
	return true;
}

bool ScriptLoader::LoadComponent(Eon::ComponentDefinition& def, AstNode* n) {
	
	if (!LoadArguments(def.args, n))
		return false;
	
	return true;
}

bool ScriptLoader::LoadChain(Eon::ChainDefinition& chain, AstNode* n) {
	const auto& map = VfsValueExtFactory::AtomDataMap();
	Vector<Endpoint> loops, states, atoms, stmts, conns;
	
	n->FindAll(loops, Cursor_DriverStmt); // subset of loops
	n->FindAll(loops, Cursor_LoopStmt);
	Sort(loops, AstNodeLess());
	
	for (Endpoint& ep : loops) {
		AstNode* loop = ep.n;
		bool is_driver = loop->src == Cursor_DriverStmt;
		
		Eon::LoopDefinition& loop_def = chain.loops.Add();
		loop_def.loc = loop->loc;
		loop_def.is_driver = is_driver;
		
		if (!GetPathId(loop_def.id, n, loop))
			return false;
		
		AstNode* stmt_block = loop->Find(Cursor_CompoundStmt);
		if (!stmt_block) {
			AddError(loop->loc, "loop has no statement-block");
			return false;
		}
		
		atoms.SetCount(0);
		stmt_block->FindAll(atoms, Cursor_AtomStmt);
		Sort(atoms, AstNodeLess());
		
		if (atoms.IsEmpty()) {
			AddError(loop->loc, "no atoms in statement-block");
			return false;
		}
		
		if (is_driver && atoms.GetCount() > 1) {
			AddError(loop->loc, "only single atom is allowed in driver");
			return false;
		}
		
		if (atoms.GetCount() == 1 && !is_driver) {
			AddError(loop->loc, "only one atom in the loop");
			return false;
		}
		
		for (Endpoint& ep : atoms) {
			AstNode* atom = ep.n;
			Eon::AtomDefinition& atom_def = loop_def.atoms.Add();
			
			if (!GetPathId(atom_def.id, loop, atom))
				return false;
			
			String loop_action = atom_def.id.ToString();
			const VfsValueExtFactory::AtomData* found_atom = 0;
			for (const VfsValueExtFactory::AtomData& atom_data : map.GetValues()) {
				bool match = false;
				for (const String& action : atom_data.actions) {
					if (action == loop_action) {
						match = true;
						break;
					}
				}
				if (!match)
					continue;
				found_atom = &atom_data;
			}
			
			if (!found_atom) {
				AddError(atom->loc, "could not find atom for '" + loop_action + "'");
				return false;
			}
			
			AtomTypeCls type = found_atom->cls;
			LinkTypeCls link = found_atom->link_type;
			ASSERT(type.IsValid());
			
			atom_def.iface.Realize(type);
			
			if (!is_driver) {
				ASSERT(atom_def.iface.sink.GetCount());
				ASSERT(atom_def.iface.src.GetCount());
				atom_def.link = link;
				
				conns.SetCount(0);
				atom->FindAllStmt(conns, Cursor_AtomConnectorStmt);
				Sort(conns, AstNodeLess());
				if (!conns.IsEmpty()) {
					int sink_conn_i = 0, src_conn_i = 0; // side ids start from 1, so this shouldn't be -1
					for (Endpoint& ep : conns) {
						AstNode* conn = ep.n;
						bool is_src = conn->i64 != 0;
						if (is_src)
							src_conn_i++;
						else
							sink_conn_i++;
						
						if (conn->val.Sub<AstNode>().IsEmpty()) {
							// Empty connector is allowed
							continue;
						}
							
						AstNode* expr = conn->Find(Cursor_ExprOp);
						if (!expr) {
							AddError(conn->loc, "internal error: no expression");
							return false;
						}
						
						bool succ = false;
						if (expr->src == Cursor_Op_EQ) {
							Eon::AtomDefinition::LinkCandidate& cand =
								is_src ?
									atom_def.src_link_cands.Add(src_conn_i) :
									atom_def.sink_link_cands.Add(sink_conn_i);
							
							AstNode* a0 = expr->arg[0];
							AstNode* a1 = expr->arg[1];
							while (a0->src == Cursor_Rval && a0->rval) a0 = a0->rval;
							while (a1->src == Cursor_Rval && a1->rval) a1 = a1->rval;
							String key;
							if (a0->src == Cursor_VarDecl) {
								key = a0->val.id;
							}
							else if (a0->src == Cursor_Unresolved) {
								key = a0->str;
							}
							if (key.GetCount()) {
								if (IsPartially(a1->src, Cursor_Literal)) {
									a1->CopyToValue(cand.req_args.GetAdd(key));
									succ = true;
								}
								else if (a1->src == Cursor_Unresolved) {
									cand.req_args.GetAdd(key) = a1->str;
									succ = true;
								}
							}
						}
						
						if (!succ) {
							AddError(expr->loc, "invalid atom connector expression");
							return false;
						}
					}
				}
			}
			
			if (!LoadArguments(atom_def.args, atom))
				return false;
		}
		
		if (!LoadArguments(loop_def.args, loop))
			return false;
		
	}
	
	for (Eon::LoopDefinition& src_loop : chain.loops) {
		for (Eon::AtomDefinition& src_atom : src_loop.atoms) {
			int src_count = src_atom.iface.type.iface.src.GetCount();
			
			for(int src_i = 1; src_i < src_count; src_i++) {
				const ValDevTuple::Channel& src_ch = src_atom.iface.type.iface.src[src_i];
				const ValDevCls& src_vd = src_ch.vd;
				bool src_is_opt = src_ch.is_opt;
				IfaceConnLink& src_conn = src_atom.iface.src[src_i];
				
				bool connected = false;
				
				int sink_cand_i = src_atom.src_link_cands.Find(src_i);
				Eon::AtomDefinition::LinkCandidate* src_cand =
					sink_cand_i >= 0 ?
						&src_atom.src_link_cands[sink_cand_i] : 0;
				
				ASSERT(src_conn.conn < 0);
				
				for (Eon::LoopDefinition& sink_loop : chain.loops) {
					if (&src_loop == &sink_loop)
						continue;
					
					for (Eon::AtomDefinition& sink_atom : sink_loop.atoms) {
						int sink_count = sink_atom.iface.type.iface.sink.GetCount();
						
						for(int sink_i = 1; sink_i < sink_count; sink_i++) {
							const ValDevTuple::Channel& sink_ch = sink_atom.iface.type.iface.sink[sink_i];
							const ValDevCls& sink_vd = sink_ch.vd;
							bool sink_is_opt = sink_ch.is_opt;
							IfaceConnLink& sink_conn = sink_atom.iface.sink[sink_i];
							
							int sink_cand_i = sink_atom.sink_link_cands.Find(sink_i);
							Eon::AtomDefinition::LinkCandidate* sink_cand =
								sink_cand_i >= 0 ?
									&sink_atom.sink_link_cands[sink_cand_i] : 0;
							
							// Only single fully optional is tried (so continue if connection exists)
							if ((sink_is_opt && !sink_cand) || (src_is_opt && !src_cand)) {
								if (src_atom.iface.HasCommonConnection(sink_atom.iface))
									continue;
							}
							
							if (sink_conn.conn < 0 && src_vd == sink_vd) {
								bool cond_prevents = false;
								
								if (src_cand) {
									for(int i = 0; i < src_cand->req_args.GetCount() && !cond_prevents; i++) {
										String key = src_cand->req_args.GetKey(i);
										if (key == "loop") {
											String loop_req = src_cand->req_args[i].ToString();
											Vector<String> parts = Split(loop_req, ".");
											if (!sink_loop.IsPathTrailMatch(parts))
												cond_prevents = true;
										}
										else {
											int j = sink_loop.args.Find(key);
											if (j >= 0) {
												const Value& src_obj = src_cand->req_args[i];
												const Value& sink_obj = sink_loop.args[j];
												if (src_obj != sink_obj)
													cond_prevents = true;
											}
											else cond_prevents = true;
										}
									}
								}
								
								if (sink_cand) {
									for(int i = 0; i < sink_cand->req_args.GetCount() && !cond_prevents; i++) {
										String key = sink_cand->req_args.GetKey(i);
										if (key == "loop") {
											String loop_req = sink_cand->req_args[i].ToString();
											Vector<String> parts = Split(loop_req, ".");
											if (!src_loop.IsPathTrailMatch(parts))
												cond_prevents = true;
										}
										else {
											int j = src_loop.args.Find(key);
											if (j >= 0) {
												const Value& sink_obj = sink_cand->req_args[i];
												const Value& src_obj = src_loop.args[j];
												if (sink_obj != src_obj)
													cond_prevents = true;
											}
											else cond_prevents = true;
										}
									}
								}
								
								if (!cond_prevents) {
									int conn_id = GetLoader().NewConnectionId();
									
									src_conn.conn = conn_id;
									src_conn.local = src_i;
									src_conn.other = sink_i;
									
									sink_conn.conn = conn_id;
									sink_conn.local = sink_i;
									sink_conn.other = src_i;
									
									connected = true;
									break;
								}
							}
						}
						
						if (connected)
							break;
					}
					
					if (connected)
						break;
				}
				
				if (!connected && !src_is_opt) {
					AddError(src_atom.loc, "could not connect source to any sink. Loop '" + src_loop.id.ToString() + "', atom '" + src_atom.id.ToString() + "'");
					return false;
				}
			}
		}
	}
	
	
	n->FindAll(states, Cursor_StateStmt);
	Sort(states, AstNodeLess());
	for (Endpoint& ep : states) {
		AstNode* state = ep.n;
		Eon::StateDeclaration& state_def = chain.states.Add();
		state_def.loc = state->loc;
		
		if (!GetPathId(state_def.id, n, state))
			return false;
		
	}
	
    bool ok = true;
    if (eager_build_chains) {
        // Experimental: directly materialize this chain using Core contexts.
        // Note: ImplementScript still builds via loader->Load(), so enabling this
        // can lead to duplicate instantiation. Keep it disabled unless using a
        // custom post-init/start path.
        if (!BuildChain(chain))
            ok = false;
        // Reduce memory: keep chain id/args, drop contents so loader tree stays small.
        chain.loops.Clear();
        chain.subchains.Clear();
        chain.states.Clear();
    }
    return ok;
}

bool ScriptLoader::LoadArguments(ArrayMap<String, Value>& args, AstNode* n) {
	if (!n)
		return true; // only failed statements returns false
	
	if (n->src != Cursor_CompoundStmt) {
		AstNode* block = n->Find(Cursor_CompoundStmt);
		return LoadArguments(args, block);
	}
	
	AstNode* block = n;
	Vector<Endpoint> stmts;
	
	for (AstNode& stmt : block->val.Sub<AstNode>()) {
		if (!IsPartially(stmt.src, Cursor_Stmt) || stmt.src == Cursor_AtomConnectorStmt)
			continue;
		
		static int dbg_i;
		bool succ = false;
		if (stmt.src == Cursor_ExprStmt) {
			if (stmt.rval) {
				AstNode* r = stmt.rval;
				while (r->src == Cursor_Rval && r->rval) r = r->rval;
				AstNode& rval = *r;
				
				if (IsPartially(rval.src, Cursor_Op)) {
					if (rval.src == Cursor_Op_ASSIGN) {
						dbg_i++;
						AstNode* key = rval.arg[0];
						AstNode* value = rval.arg[1];
						while (key->src == Cursor_Rval && key->rval) key = key->rval;
						while (value->src == Cursor_Rval && value->rval) value = key->rval;
						if (key->src == Cursor_Unresolved && key->str.GetCount()) {
							String key_str = key->str;
							if (IsPartially(value->src, Cursor_Literal)) {
								Value val_obj;
								value->CopyToValue(val_obj);
								args.GetAdd(key_str) = val_obj;
								succ = true;
							}
							else if (value->src == Cursor_Unresolved && value->str.GetCount()) {
								args.GetAdd(key_str) = value->str;
								succ = true;
							}
							else if (IsPartially(value->src, Cursor_Op)) {
								Value val_obj = EvaluateAstNodeValue(*value);
								args.GetAdd(key_str) = val_obj;
								succ = true;
							}
							else {
								LOG(rval.GetTreeString(0));
								TODO
							}
						}
						else if (key->src == Cursor_VarDecl) {
							String key_str = key->val.id;
							if (IsPartially(value->src, Cursor_Literal)) {
								Value val_obj;
								value->CopyToValue(val_obj);
								args.GetAdd(key_str) = val_obj;
								succ = true;
							}
							else {
								LOG(rval.GetTreeString(0));
								TODO
							}
						}
						else {
							LOG(rval.GetTreeString(0));
							TODO
						}
					}
					else {
						LOG(rval.GetTreeString(0));
						TODO
					}
				}
				else if (rval.src == Cursor_CompoundStmt ||
						 rval.src == Cursor_SymlinkStmt) {
					if (!LoadArguments(args, &rval))
						return false;
					succ = true;
				}
				else {
					LOG(rval.GetTreeString(0));
					TODO
				}
			}
			else {
				LOG(stmt.GetTreeString(0));
				TODO
			}
		}
		else {
			LOG(stmt.GetTreeString(0));
			TODO
		}
		
		if (!succ) {
			DUMP(dbg_i);
			LOG(stmt.GetTreeString(0));
			AddError(stmt.loc, "could not resolve statement in atom");
			return false;
		}
	}
	
	return true;
}

VfsValue* ScriptLoader::ResolveLoop(Eon::Id& id) {
	Engine* mach = val.FindOwner<Engine>();
	ASSERT(mach);
	if (!mach) throw Exc("no machine");
	
	VfsValue* l0;
	VfsValue* l1 = &mach->GetRootLoop();
	VfsValue* s0 = 0;
	VfsValue* s1 = &mach->GetRootSpace();
	int i = 0, count = id.parts.GetCount();
	
	for (const String& part : id.parts) {
		if (i++ == count - 1) {
			l0 = &l1->GetAdd(part, 0);
			s0 = &s1->GetAdd(part, 0);
		}
		else {
			l1 = &l1->GetAdd(part, 0);
			s1 = &s1->GetAdd(part, 0);
		}
	}
	
	ASSERT(l0);
	return l0;
}

bool ScriptLoader::ConnectSides(ScriptLoopLoader& loop0, ScriptLoopLoader& loop1) {
	
	int dbg_i = 0;
	for (AtomBasePtr& sink : loop0.atoms) {
		LinkBasePtr sink_link = sink->GetLink();
		const IfaceConnTuple& sink_iface = sink->GetInterface();
		for (int sink_ch = 1; sink_ch < sink_iface.type.iface.sink.GetCount(); sink_ch++) {
			const IfaceConnLink& sink_conn = sink_iface.sink[sink_ch];
			RTLOG("ScriptLoader::ConnectSides:	sink ch #" << sink_ch << " " << sink_conn.ToString());
			ASSERT(sink_conn.conn >= 0 || sink_iface.type.IsSinkChannelOptional(sink_ch));
			if (sink_conn.conn < 0 && sink_iface.type.IsSinkChannelOptional(sink_ch))
				continue;
			bool found = false;
			for (AtomBasePtr& src : loop1.atoms) {
				LinkBasePtr src_link = src->GetLink();
				const IfaceConnTuple& src_iface = src->GetInterface();
				for (int src_ch = 1; src_ch < src_iface.type.iface.src.GetCount(); src_ch++) {
					const IfaceConnLink& src_conn = src_iface.src[src_ch];
					RTLOG("ScriptLoader::ConnectSides:		src ch #" << src_ch << " " << src_conn.ToString());
					ASSERT(src_conn.conn >= 0 || src_iface.type.IsSourceChannelOptional(src_ch));
					if (src_conn.conn < 0 && src_iface.type.IsSourceChannelOptional(src_ch))
						continue;
					if (sink_conn.conn == src_conn.conn) {
						RTLOG("ScriptLoader::ConnectSides:			linking side-link src ch #" << src_ch << " " << src_conn.ToString() << " to sink ch #" << sink_ch << " " << sink_conn.ToString());
						found = true;
						
						int src_ch_i = src_conn.local;
						int sink_ch_i = sink_conn.local;
						ASSERT(src_conn.other == sink_conn.local);
						ASSERT(sink_conn.other == src_conn.local);
						
						if (!src_link->LinkSideSink(sink_link, src_ch_i, sink_ch_i)) {
							AddError(loop0.def.loc, "Side-linking was refused");
							return false;
						}
						
						#if VERBOSE_SCRIPT_LOADER
						LOG(ClassPathTop(src->ToString()) + "(" << HexStrPtr(&*src) << "," << src_ch_i << ") side-linked to " + ClassPathTop(sink->ToString()) + "(" << HexStrPtr(&*sink) << "," << sink_ch_i << ")");
						#else
						RTLOG(ClassPathTop(src->ToString()) + "(" << HexStrPtr(&*src) << "," << src_ch_i << ") side-linked to " + ClassPathTop(sink->ToString()) + "(" << HexStrPtr(&*sink) << "," << sink_ch_i << ")");
						#endif
						
						loop0.UpdateLoopLimits();
						loop1.UpdateLoopLimits();
						break;
					}
				}
			}
			
			
			dbg_i++;
		}
	}
	
	
	return true;
}


}
END_UPP_NAMESPACE
