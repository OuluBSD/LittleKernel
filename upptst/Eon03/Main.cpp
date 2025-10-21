#include <Shell/Shell.h>

/*
machine x11.app:

	driver context:
		x11.context

	chain program:
		
		loop video:
			center.customer
			center.video.src.dbg_generator:
				mode = "noise"
			x11.video.pipe

NamePart: x11
	AtomStmt: app
	CompoundStmt:
		NamePart: context
			NamePart: driver
			CompoundStmt:
				AtomStmt: x11
				NamePart: context
		NamePart: program
			NamePart: chain
			CompoundStmt:
				NamePart: video
					LoopStmt: video
					CompoundStmt:
						NamePart: center
							AtomStmt: customer
						NamePart: video
							NamePart: src
							AtomStmt: dbg_generator
							CompoundStmt:
								ExprStmt:
									Unresolved: mode
									String: const("noise")
									assign: op(=)
						NamePart: x11
						NamePart: video
						AtomStmt: pipe
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
			
			// Handle driver context first: x11.context
			{
				Val& driver_loop = eng.GetRootLoop().GetAdd("context", 0).GetAdd("driver", 0);
				ChainContext cc_driver;
				Vector<ChainContext::AtomSpec> driver_atoms;
				ChainContext::AtomSpec& a = driver_atoms.Add();
				a.action = "x11.context";
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
			// Describe atoms in the video loop via actions and args
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
			// center.video.src.dbg_generator with mode="noise"
			{
				ChainContext::AtomSpec& a = atoms.Add();
				a.action = "center.video.src.dbg_generator";
				a.args.GetAdd("mode") = "noise";
				AtomTypeCls atom; LinkTypeCls link;
				if (ChainContext::ResolveAction(a.action, atom, link))
					a.iface.Realize(atom);
			}
			// x11.video.pipe
			{
				ChainContext::AtomSpec& a = atoms.Add();
				a.action = "x11.video.pipe";
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
			auto& driver_atom = driver_loop.AddAtom("x11.context");
			
			// Create the main video loop
			auto& loop = builder.AddLoop("video");
			auto& a0 = loop.AddAtom("center.customer");
			auto& a1 = loop.AddAtom("center.video.src.dbg_generator");
			a1.Assign("mode", "noise");
			auto& a2 = loop.AddAtom("x11.video.pipe");
			
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
			sys->PostLoadFile(GetDataFile("03a_x11_video.eon"));
		}
	};
	
	ShellMain(true);
}
