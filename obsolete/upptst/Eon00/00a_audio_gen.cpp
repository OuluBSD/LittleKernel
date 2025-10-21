#include "Eon00.h"

/*

loop tester.generator:
center.customer
center.audio.src.test
center.audio.sink.test.realtime:
	dbg_limit = 100

NamePart: tester
	LoopStmt: generator
		CompoundStmt:
			NamePart: center
				AtomStmt: customer
				NamePart: audio
					NamePart: src
						AtomStmt: test
					NamePart: sink
						NamePart: test
							AtomStmt: realtime
								CompoundStmt:
									ExprStmt:
										Unresolved: dbg_limit
											int32: const(int32: 100)
										assign: op(=)
*/

NAMESPACE_UPP

void Run00aAudioGen(Engine& eng, int method) {
	auto sys = eng.GetAdd<Eon::ScriptLoader>();
	sys->SetEagerChainBuild(true);
	
	switch(method) {
	case 2: {
		using namespace Eon;
		// Manually writing directly to the VFS using Context
		Val& loop_root = eng.GetRootLoop();
		Val& space_root = eng.GetRootSpace();
		VfsValue* l = &loop_root;
		VfsValue* s = &space_root;
		for (const String& part : Split("tester.generator", ".")) {
			l = &l->GetAdd(part, 0);
			s = &s->GetAdd(part, 0);
		}
		// Alternative: resolve loop directly using Core helper
		VfsValue* l2 = Eon::ResolveLoopPath(eng, String("tester.generator"));
		ASSERT(l2);
		if (l2 == l)
			LOG("ResolveLoopPath matched manual loop path");
		else
			LOG("ResolveLoopPath did not match manual loop path");
		
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
		// center.audio.src.test
		{
			ChainContext::AtomSpec& a = atoms.Add();
			a.action = "center.audio.src.test";
			AtomTypeCls atom; LinkTypeCls link;
			if (ChainContext::ResolveAction(a.action, atom, link))
				a.iface.Realize(atom);
		}
		// center.audio.sink.test.realtime with dbg_limit=100
		{
			ChainContext::AtomSpec& a = atoms.Add();
			a.action = "center.audio.sink.test.realtime";
			a.args.GetAdd("dbg_limit") = 100;
			AtomTypeCls atom; LinkTypeCls link;
			if (ChainContext::ResolveAction(a.action, atom, link))
				a.iface.Realize(atom);
		}
		
		LoopContext& loop = cc.AddLoop(*l, atoms, true);
		LOG(cc.GetTreeString(0));
		LOG(loop.GetTreeString(0));
		if (!cc.PostInitializeAll()) {
			LOG("PostInitialize failed");
			Exit(1);
		}
		if (!cc.StartAll()) {
			LOG("Start failed");
			cc.UndoAll();
			Exit(1);
		}
		break;
	}
	case 1: {
		// Manually building AstNode (skipping .eon file parsing)
		Eon::Builder& builder = sys->val.GetAdd<Eon::Builder>("builder");
		auto& loop = builder.AddLoop("tester.generator");
		auto& a0 = loop.AddAtom("center.customer");
		auto& a1 = loop.AddAtom("center.audio.src.test");
		auto& a2 = loop.AddAtom("center.audio.sink.test.realtime");
		a2.Assign("dbg_limit", 100);
		Eon::AstNode* root = 0;
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
		break;
	}
	case 0:
		sys->PostLoadFile(GetDataFile("00a_audio_gen.eon"));
		break;
	default:
		throw Exc(Format("Run00aAudioGen: unknown method %d", method));
	}
}

END_UPP_NAMESPACE
