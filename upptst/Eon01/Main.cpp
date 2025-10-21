#include <Shell/Shell.h>

/*
machine sdl.app:

chain program:
	state event.register

	loop event:
		center.customer
		event.src.test.pipe
		state.event.pipe:
			target = event.register
			dbg_limit = 100

NamePart: sdl
	AtomStmt: app
	CompoundStmt:
		NamePart: program
			NamePart: chain
			CompoundStmt:
				NamePart: event
					NamePart: register
					AtomStmt: state
			NamePart: event
				LoopStmt: event
				CompoundStmt:
					NamePart: center
						AtomStmt: customer
					NamePart: event
						NamePart: src
							AtomStmt: test
							NamePart: pipe
						AtomStmt: state
						NamePart: event
						NamePart: pipe
						CompoundStmt:
							ExprStmt:
								Unresolved: target
								NamePart: event
								NamePart: register
								assign: op(=)
							ExprStmt:
								Unresolved: dbg_limit
								int32: const(int32: 100)
								assign: op(=)
*/

CONSOLE_APP_MAIN {
	using namespace Upp;
	Engine& eng = ShellMainEngine();
	eng.WhenUserInitialize << [](Engine& eng) {
		auto sys = eng.GetAdd<Eon::ScriptLoader>();
		sys->SetEagerChainBuild(true);
		if (1) {
			// Manually writing directly to the VFS using Context
			using namespace Eon;
			
			// Handle state event.register first (from chain program section)
			{
				Val& state_loop = eng.GetRootLoop().GetAdd("event", 0).GetAdd("register", 0);
				ChainContext cc_state;
				Vector<ChainContext::AtomSpec> state_atoms;
				ChainContext::AtomSpec& a = state_atoms.Add();
				a.action = "state.event.register";
				AtomTypeCls atom; LinkTypeCls link;
				if (ChainContext::ResolveAction(a.action, atom, link))
					a.iface.Realize(atom);
				LoopContext& state_loop_ctx = cc_state.AddLoop(state_loop, state_atoms, true);
				LOG("State context: " << cc_state.GetTreeString(0));
				LOG("State loop: " << state_loop_ctx.GetTreeString(0));
				if (!cc_state.PostInitializeAll()) {
					LOG("State PostInitialize failed");
				}
				if (!cc_state.StartAll()) {
					LOG("State Start failed");
				}
			}
			
			// Create the main event loop
			Val& loop_root = eng.GetRootLoop();
			Val& space_root = eng.GetRootSpace();
			VfsValue* l = &loop_root;
			VfsValue* s = &space_root;
			for (const String& part : Split("event", ".")) {
				l = &l->GetAdd(part, 0);
				s = &s->GetAdd(part, 0);
			}
			// Alternative: resolve loop directly using Core helper
			VfsValue* l2 = Eon::ResolveLoopPath(eng, String("event"));
			ASSERT(l2);
			if (l2 == l)
				LOG("ResolveLoopPath matched manual loop path");
			else
				LOG("ResolveLoopPath did not match manual loop path");
			// Describe atoms in the event loop via actions and args
			ChainContext cc;
			Vector<ChainContext::AtomSpec> atoms;
			// center.customer
			{
				ChainContext::AtomSpec& a = atoms.Add();
				a.action = "center.customer";
				AtomTypeCls atom; LinkTypeCls link;
				if (ChainContext::ResolveAction(a.action, atom, link))
					a.iface.Realize(atom);
			}
			// event.src.test.pipe
			{
				ChainContext::AtomSpec& a = atoms.Add();
				a.action = "event.src.test.pipe";
				AtomTypeCls atom; LinkTypeCls link;
				if (ChainContext::ResolveAction(a.action, atom, link))
					a.iface.Realize(atom);
			}
			// state.event.pipe with target=event.register and dbg_limit=100
			{
				ChainContext::AtomSpec& a = atoms.Add();
				a.action = "state.event.pipe";
				a.args.GetAdd("target") = "event.register";
				a.args.GetAdd("dbg_limit") = 100;
				AtomTypeCls atom; LinkTypeCls link;
				if (ChainContext::ResolveAction(a.action, atom, link))
					a.iface.Realize(atom);
			}
			// Build the loop under the loop path and link atoms
			LoopContext& loop = cc.AddLoop(*l, atoms, true);
			LOG(cc.GetTreeString(0));
			LOG(loop.GetTreeString(0));
			// Finalize and start: call PostInitialize and Start via contexts
			if (!cc.PostInitializeAll()) {
				LOG("PostInitialize failed");
				Exit(1);
			}
			if (!cc.StartAll()) {
				LOG("Start failed");
				cc.UndoAll();
				Exit(1);
			}
		}
		else if (1) {
			// Manually building AstNode (skipping .eon file parsing)
			Eon::Builder& builder = sys->val.GetAdd<Eon::Builder>("builder");
			
			// Create state.event.register first
			auto& state_loop = builder.AddLoop("event.register");
			auto& state_atom = state_loop.AddAtom("state.event.register");
			
			// Then create the main event loop
			auto& loop = builder.AddLoop("event");
			auto& a0 = loop.AddAtom("center.customer");
			auto& a1 = loop.AddAtom("event.src.test.pipe");
			auto& a2 = loop.AddAtom("state.event.pipe");
			a2.Assign("target", "event.register");
			a2.Assign("dbg_limit", 100);
			
			AstNode* root = 0;
			try {
				root = builder.CompileAst();
				if (!root) throw Exc("empty root");
			}
			catch (Exc e) {
				LOG("error: " << e);
				Exit(1);
			}
			LOG(root->GetTreeString());
			sys->LoadAst(root);
		}
		else {
			// Load eon file (parses AstNode and loads it)
			sys->PostLoadFile(GetDataFile("01a_event_state.eon"));
		}
	};
	
	ShellMain(true);
}
