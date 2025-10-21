#include <Shell/Shell.h>

/*
machine sdl.app:
	
	driver context:
		x11.sw.context

	chain program:
		state event.register
		
		loop video:
			center.customer
			x11.sw.fbo.program:
				drawmem =	"false"
				program =		"ecs_view"
				shader.default.frag.name =	"obj_view"
				shader.default.vtx.name =	"obj_view"
			
			x11.sw.video.pipe:
				close_machine =	true
				sizeable =		true
				env =			event.register
				recv.data =      true


world ecs.dummy:
    system rendering
    system events
    system physics:
        //log: debug
    
    pool world:
        entity ground:

NamePart: sdl
	AtomStmt: app
	CompoundStmt:
		NamePart: context
			NamePart: driver
			CompoundStmt:
				AtomStmt: x11
				NamePart: sw
				NamePart: context
		NamePart: program
			NamePart: chain
			CompoundStmt:
				NamePart: event
					NamePart: register
					AtomStmt: state
				NamePart: video
					LoopStmt: video
					CompoundStmt:
						NamePart: center
						AtomStmt: customer
						NamePart: x11
						NamePart: sw
						NamePart: fbo
						AtomStmt: program
						CompoundStmt:
							ExprStmt:
								Unresolved: drawmem
								String: const("false")
								assign: op(=)
							ExprStmt:
								Unresolved: program
								String: const("ecs_view")
								assign: op(=)
							ExprStmt:
								Unresolved: shader.default.frag.name
								String: const("obj_view")
								assign: op(=)
							ExprStmt:
								Unresolved: shader.default.vtx.name
								String: const("obj_view")
								assign: op(=)
						NamePart: x11
						NamePart: sw
						NamePart: video
						AtomStmt: pipe
						CompoundStmt:
							ExprStmt:
								Unresolved: close_machine
								Bool: const(true)
								assign: op(=)
							ExprStmt:
								Unresolved: sizeable
								Bool: const(true)
								assign: op(=)
							ExprStmt:
								Unresolved: env
								NamePart: event
								NamePart: register
								assign: op(=)
							ExprStmt:
								Unresolved: recv.data
								Bool: const(true)
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
			
			// Handle driver context first: x11.sw.context
			{
				Val& driver_loop = eng.GetRootLoop().GetAdd("context", 0).GetAdd("driver", 0);
				ChainContext cc_driver;
				Vector<ChainContext::AtomSpec> driver_atoms;
				ChainContext::AtomSpec& a = driver_atoms.Add();
				a.action = "x11.sw.context";
				AtomTypeCls atom; LinkTypeCls link;
				if (ChainContext::ResolveAction(a.action, atom, link))
					a.iface.Realize(atom);
				LoopContext& driver_loop_ctx = cc_driver.AddLoop(driver_loop, driver_atoms, true);
				LOG("Driver context: " << cc_driver.GetTreeString(0));
				LOG("Driver loop: " << driver_loop_ctx.GetTreeString(0));
				if (!cc_driver.PostInitializeAll()) {
					LOG("Driver PostInitialize failed");
				}
				if (!cc_driver.StartAll()) {
					LOG("Driver Start failed");
				}
			}
			
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
			
			// Create the main video loop
			Val& loop_root = eng.GetRootLoop();
			Val& space_root = eng.GetRootSpace();
			VfsValue* l = &loop_root;
			VfsValue* s = &space_root;
			for (const String& part : Split("video", ".")) {
				l = &l->GetAdd(part, 0);
				s = &s->GetAdd(part, 0);
			}
			// Alternative: resolve loop directly using Core helper
			VfsValue* l2 = Eon::ResolveLoopPath(eng, String("video"));
			ASSERT(l2);
			if (l2 == l)
				LOG("ResolveLoopPath matched manual loop path");
			else
				LOG("ResolveLoopPath did not match manual loop path");
			// Create the main video loop with its atoms
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
			// x11.sw.fbo.program with multiple parameters
			{
				ChainContext::AtomSpec& a = atoms.Add();
				a.action = "x11.sw.fbo.program";
				a.args.GetAdd("drawmem") = "false";
				a.args.GetAdd("program") = "ecs_view";
				a.args.GetAdd("shader.default.frag.name") = "obj_view";
				a.args.GetAdd("shader.default.vtx.name") = "obj_view";
				AtomTypeCls atom; LinkTypeCls link;
				if (ChainContext::ResolveAction(a.action, atom, link))
					a.iface.Realize(atom);
			}
			// x11.sw.video.pipe with multiple parameters
			{
				ChainContext::AtomSpec& a = atoms.Add();
				a.action = "x11.sw.video.pipe";
				a.args.GetAdd("close_machine") = true;
				a.args.GetAdd("sizeable") = true;
				a.args.GetAdd("env") = "event.register";
				a.args.GetAdd("recv.data") = true;
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
			
			// Create driver context
			auto& driver_loop = builder.AddLoop("context.driver");
			auto& driver_atom = driver_loop.AddAtom("x11.sw.context");
			
			// Create state.event.register
			auto& state_loop = builder.AddLoop("event.register");
			auto& state_atom = state_loop.AddAtom("state.event.register");
			
			// Create the main video loop
			auto& loop = builder.AddLoop("video");
			auto& a0 = loop.AddAtom("center.customer");
			auto& a1 = loop.AddAtom("x11.sw.fbo.program");
			a1.Assign("drawmem", "false");
			a1.Assign("program", "ecs_view");
			a1.Assign("shader.default.frag.name", "obj_view");
			a1.Assign("shader.default.vtx.name", "obj_view");
			auto& a2 = loop.AddAtom("x11.sw.video.pipe");
			a2.Assign("close_machine", true);
			a2.Assign("sizeable", true);
			a2.Assign("env", "event.register");
			a2.Assign("recv.data", true);
			
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
			sys->PostLoadFile(GetDataFile("08a_gui.eon"));
		}
	};
	
	ShellMain(true);
}
