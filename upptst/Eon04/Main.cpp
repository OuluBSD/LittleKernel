#include <Shell/Shell.h>

/*
machine sdl.app:

	driver context:
		sdl.context

	chain program:
		
		loop audio:
			center.customer
			center.audio.src.dbg_generator
			sdl.audio

NamePart: sdl
	AtomStmt: app
	CompoundStmt:
		NamePart: context
			NamePart: driver
			CompoundStmt:
				AtomStmt: sdl
				NamePart: context
		NamePart: program
			NamePart: chain
			CompoundStmt:
				NamePart: audio
					LoopStmt: audio
					CompoundStmt:
						NamePart: center
							AtomStmt: customer
						NamePart: audio
							NamePart: src
							AtomStmt: dbg_generator
						NamePart: sdl
						AtomStmt: audio
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
			
			// Handle driver context first: sdl.context
			{
				Val& driver_loop = eng.GetRootLoop().GetAdd("context", 0).GetAdd("driver", 0);
				ChainContext cc_driver;
				Vector<ChainContext::AtomSpec> driver_atoms;
				ChainContext::AtomSpec& a = driver_atoms.Add();
				a.action = "sdl.context";
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
			
			// Create the main audio loop
			Val& loop_root = eng.GetRootLoop();
			Val& space_root = eng.GetRootSpace();
			VfsValue* l = &loop_root;
			VfsValue* s = &space_root;
			for (const String& part : Split("audio", ".")) {
				l = &l->GetAdd(part, 0);
				s = &s->GetAdd(part, 0);
			}
			// Alternative: resolve loop directly using Core helper
			VfsValue* l2 = Eon::ResolveLoopPath(eng, String("audio"));
			ASSERT(l2);
			if (l2 == l)
				LOG("ResolveLoopPath matched manual loop path");
			else
				LOG("ResolveLoopPath did not match manual loop path");
			// Describe atoms in the audio loop via actions and args
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
			// center.audio.src.dbg_generator
			{
				ChainContext::AtomSpec& a = atoms.Add();
				a.action = "center.audio.src.dbg_generator";
				AtomTypeCls atom; LinkTypeCls link;
				if (ChainContext::ResolveAction(a.action, atom, link))
					a.iface.Realize(atom);
			}
			// sdl.audio
			{
				ChainContext::AtomSpec& a = atoms.Add();
				a.action = "sdl.audio";
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
			auto& driver_atom = driver_loop.AddAtom("sdl.context");
			
			// Create the main audio loop
			auto& loop = builder.AddLoop("audio");
			auto& a0 = loop.AddAtom("center.customer");
			auto& a1 = loop.AddAtom("center.audio.src.dbg_generator");
			auto& a2 = loop.AddAtom("sdl.audio");
			
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
			sys->PostLoadFile(GetDataFile("04a_sdl_audio.eon"));
		}
	};
	
	ShellMain(true);
}
