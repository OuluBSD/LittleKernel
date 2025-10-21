#include <Shell/Shell.h>

CONSOLE_APP_MAIN {
    using namespace Upp;
    Engine& eng = ShellMainEngine();

    eng.WhenUserInitialize << [](Engine& eng) {
        using namespace Eon;

        // ECS contexts: build a small pool/entity tree and print it
        PoolContext root_pool(eng.GetRootPool());
        PoolContext world = root_pool.AddPool("world");
        EntityContext player = world.AddEntity("player");
        ComponentContext comp = player.AddComponent("Transform");
        LOG("--- ECS Contexts ---");
        LOG(root_pool.GetTreeString(0));
        LOG(world.GetTreeString(1));
        LOG(player.GetTreeString(2));
        LOG(comp.GetTreeString(3));

        // Loop/chain contexts: resolve a loop path and add atoms if actions are known
        VfsValue* loop_node = ResolveLoopPath(eng, String("demo.chain"));
        if (!loop_node) {
            LOG("Could not resolve loop path");
            return;
        }

        ChainContext cc;
        Vector<ChainContext::AtomSpec> atoms;

        // Try to add some known actions if they are registered; otherwise, skip.
        for (String action : {String("center.customer"), String("center.audio.src.test"), String("center.audio.sink.test.realtime") }) {
            AtomTypeCls atom; LinkTypeCls link;
            if (ChainContext::ResolveAction(action, atom, link)) {
	            ChainContext::AtomSpec& a = atoms.Add();
	            a.action = action;
                a.iface.Realize(atom);
                if (action.Find("realtime") >= 0)
                    a.args.GetAdd("dbg_limit") = 100;
            }
        }

        LoopContext& lc = cc.AddLoop(*loop_node, atoms, true);
        LOG("--- Chain/Loop Contexts ---");
        LOG(cc.GetTreeString(0));
        LOG(lc.GetTreeString(0));
    };

    ShellMain(true);
}

