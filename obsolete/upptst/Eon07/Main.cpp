#include <Shell/Shell.h>

/*
machine sdl.app:
	
	driver context:
		sdl.context
	
	chain program:
		state event.register
		
		loop center.events:
			center.customer
			sdl.event.pipe
			state.event.pipe:
				target = event.register
		
		loop ogl.fbo:
			ogl.customer
			sdl.ogl.fbo.program:
				drawmem =		"false"
				program =		"ecs_view"
				shader.default.frag.path =	"shaders/tests/07d_fragment.glsl"
				shader.default.vtx.path =	"shaders/tests/07d_vertex.glsl"
			sdl.fbo.sink:
				close_machine =  	true
				sizeable =		  	true
				env =			  	event.register

world ecs.dummy:
	system rendering
	system events
	system interaction:
		env = event.register
	system physics
	system player
	
	pool world:
		entity ground:
			comp transform3
			comp model:
				builtin = plane
			comp physics:
				bind = true
				test.fn = fixed
		
		entity box:
			comp transform3:
				y = 1
			comp body
			comp model:
				builtin = box
			comp physics:
				bind = true
				test.fn = fixed
		
		entity box2:
			comp transform3:
				x = 3
				y = 1
			comp body
			comp model:
				builtin = box
			comp physics:
				bind = true
				test.fn = fixed
		
		entity box3:
			comp transform3:
				x = 5
				y = 2
				z = 2
			comp body
			comp model:
				builtin = box
			comp physics:
				bind = true
				test.fn = fixed
		
		entity player.body:
			comp transform3:
				x = 0
				y = 0
				z = 6
			comp physics:
				bind = true
			comp player.body:
				height = 1.74
		
		entity player.head:
			comp transform3
			comp viewable
			comp viewport:
				fov = 90
			comp camera.chase
			comp player.head:
				body = world.player.body
		
		entity player.hand.left:
			comp transform3
			comp body
			comp model:
				builtin = "cylinder,0.02,0.2"
				pitch = -90
			comp player.hand:
				hand = left
				body = world.player.body
				simulated = true
		
		entity player.hand.right:
			comp transform3
			comp body
			comp model:
				builtin = "cylinder,0.02,0.2"
				pitch = -90
			comp player.hand:
				hand = right
				body = world.player.body
				simulated = true
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
			
			// Ensure machine path exists and create the event register EnvState
			Val& program_loop = eng.GetRootLoop().GetAdd("sdl", 0).GetAdd("app", 0).GetAdd("program", 0);
			Val& program_space = eng.GetRootSpace().GetAdd("sdl", 0).GetAdd("app", 0).GetAdd("program", 0);
			EnvState* env_state = eng.val.Find<EnvState>("event_register");
			if (!env_state) {
				EnvState& st = eng.val.Add<EnvState>("event_register");
				st.SetName("event_register");
			}
			// Driver: sdl.context
			{
				// note: drivers must be in same path than loops that use it (so don't do ".GetAdd("context", 0);");
				Val& driver_loop = eng.GetRootLoop().GetAdd("sdl", 0).GetAdd("app", 0);
				eng.GetRootSpace().GetAdd("sdl", 0).GetAdd("app", 0);
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
				if (!cc_driver.PostInitializeAll())
					LOG("Driver PostInitialize failed");
				if (!cc_driver.StartAll())
					LOG("Driver Start failed");
			}
			
			// Loop: center.events
			{
				Val& center_loop = program_loop.GetAdd("center", 0).GetAdd("events", 0);
				program_space.GetAdd("center", 0).GetAdd("events", 0);
				ChainContext cc_center;
				Vector<ChainContext::AtomSpec> center_atoms;
				// center.customer
				{
					ChainContext::AtomSpec& a = center_atoms.Add();
					a.action = "center.customer";
					AtomTypeCls atom; LinkTypeCls link;
					if (ChainContext::ResolveAction(a.action, atom, link))
						a.iface.Realize(atom);
				}
				// sdl.event.pipe
				{
					ChainContext::AtomSpec& a = center_atoms.Add();
					a.action = "sdl.event.pipe";
					AtomTypeCls atom; LinkTypeCls link;
					if (ChainContext::ResolveAction(a.action, atom, link))
						a.iface.Realize(atom);
				}
				// state.event.pipe with target=event.register
				{
					ChainContext::AtomSpec& a = center_atoms.Add();
					a.action = "state.event.pipe";
					a.args.GetAdd("target") = String("event_register");
					AtomTypeCls atom; LinkTypeCls link;
					if (ChainContext::ResolveAction(a.action, atom, link))
						a.iface.Realize(atom);
				}
				LoopContext& center_loop_ctx = cc_center.AddLoop(center_loop, center_atoms, true);
				LOG("center.events context: " << cc_center.GetTreeString(0));
				LOG("center.events loop: " << center_loop_ctx.GetTreeString(0));
				if (!cc_center.PostInitializeAll()) {
					LOG("center.events PostInitialize failed");
					Exit(1);
				}
				if (!cc_center.StartAll()) {
					LOG("center.events Start failed");
					cc_center.UndoAll();
					Exit(1);
				}
			}
			
			// Loop: ogl.fbo
			{
				Val& ogl_loop = program_loop.GetAdd("ogl", 0).GetAdd("fbo", 0);
				program_space.GetAdd("ogl", 0).GetAdd("fbo", 0);
				ChainContext cc_ogl;
				Vector<ChainContext::AtomSpec> ogl_atoms;
				// ogl.customer
				{
					ChainContext::AtomSpec& a = ogl_atoms.Add();
					a.action = "ogl.customer";
					AtomTypeCls atom; LinkTypeCls link;
					if (ChainContext::ResolveAction(a.action, atom, link))
						a.iface.Realize(atom);
				}
				// sdl.ogl.fbo.program with shader + program args
				{
					ChainContext::AtomSpec& a = ogl_atoms.Add();
					a.action = "sdl.ogl.fbo.program";
					a.args.GetAdd("drawmem") = String("false");
					a.args.GetAdd("program") = String("ecs_view");
					a.args.GetAdd("shader.default.frag.path") = String("shaders/tests/07d_fragment.glsl");
					a.args.GetAdd("shader.default.vtx.path") = String("shaders/tests/07d_vertex.glsl");
					AtomTypeCls atom; LinkTypeCls link;
					if (ChainContext::ResolveAction(a.action, atom, link))
						a.iface.Realize(atom);
				}
				// sdl.fbo.sink with output configuration
				{
					ChainContext::AtomSpec& a = ogl_atoms.Add();
					a.action = "sdl.fbo.sink";
					a.args.GetAdd("close_machine") = true;
					a.args.GetAdd("sizeable") = true;
					a.args.GetAdd("env") = String("event_register");
					AtomTypeCls atom; LinkTypeCls link;
					if (ChainContext::ResolveAction(a.action, atom, link))
						a.iface.Realize(atom);
				}
				LoopContext& ogl_loop_ctx = cc_ogl.AddLoop(ogl_loop, ogl_atoms, true);
				LOG("ogl.fbo context: " << cc_ogl.GetTreeString(0));
				LOG("ogl.fbo loop: " << ogl_loop_ctx.GetTreeString(0));
				if (!cc_ogl.PostInitializeAll()) {
					LOG("ogl.fbo PostInitialize failed");
					Exit(1);
				}
				if (!cc_ogl.StartAll()) {
					LOG("ogl.fbo Start failed");
					cc_ogl.UndoAll();
					Exit(1);
				}
			}

			// Instantiate ECS systems from the script
			Ptr<RenderingSystem> rendering = eng.GetAdd<RenderingSystem>();
			if (!rendering)
				LOG("RenderingSystem could not be created");
			Ptr<EventSystem> events = eng.GetAdd<EventSystem>();
			if (!events)
				LOG("EventSystem could not be created");
			Ptr<InteractionSystem> interaction = eng.GetAdd<InteractionSystem>();
			if (interaction)
				interaction->Arg("env", String("event_register"));
			else
				LOG("InteractionSystem could not be created");
			Ptr<PhysicsSystem> physics = eng.GetAdd<PhysicsSystem>();
			if (!physics)
				LOG("PhysicsSystem could not be created");
			Ptr<PlayerBodySystem> player_sys = eng.GetAdd<PlayerBodySystem>();
			if (!player_sys)
				LOG("PlayerBodySystem could not be created");

			// Build ECS world structure equivalent to the original script
			PoolContext root_pool(eng.GetRootPool());
			PoolContext world_pool = root_pool.AddPool("world");

			{
				EntityContext ground = world_pool.AddEntity("ground");
				ground.AddComponent("transform3");

				ArrayMap<String, Value> model_args;
				model_args.GetAdd("builtin") = String("plane");
				ground.AddComponent("model", &model_args);

				ArrayMap<String, Value> physics_args;
				physics_args.GetAdd("bind") = true;
				physics_args.GetAdd("test.fn") = String("fixed");
				ground.AddComponent("physics", &physics_args);
			}

			{
				EntityContext box = world_pool.AddEntity("box");
				ArrayMap<String, Value> transform_args;
				transform_args.GetAdd("y") = 1;
				box.AddComponent("transform3", &transform_args);
				box.AddComponent("body");

				ArrayMap<String, Value> model_args;
				model_args.GetAdd("builtin") = String("box");
				box.AddComponent("model", &model_args);

				ArrayMap<String, Value> physics_args;
				physics_args.GetAdd("bind") = true;
				physics_args.GetAdd("test.fn") = String("fixed");
				box.AddComponent("physics", &physics_args);
			}

			{
				EntityContext box2 = world_pool.AddEntity("box2");
				ArrayMap<String, Value> transform_args;
				transform_args.GetAdd("x") = 3;
				transform_args.GetAdd("y") = 1;
				box2.AddComponent("transform3", &transform_args);
				box2.AddComponent("body");

				ArrayMap<String, Value> model_args;
				model_args.GetAdd("builtin") = String("box");
				box2.AddComponent("model", &model_args);

				ArrayMap<String, Value> physics_args;
				physics_args.GetAdd("bind") = true;
				physics_args.GetAdd("test.fn") = String("fixed");
				box2.AddComponent("physics", &physics_args);
			}

			{
				EntityContext box3 = world_pool.AddEntity("box3");
				ArrayMap<String, Value> transform_args;
				transform_args.GetAdd("x") = 5;
				transform_args.GetAdd("y") = 2;
				transform_args.GetAdd("z") = 2;
				box3.AddComponent("transform3", &transform_args);
				box3.AddComponent("body");

				ArrayMap<String, Value> model_args;
				model_args.GetAdd("builtin") = String("box");
				box3.AddComponent("model", &model_args);

				ArrayMap<String, Value> physics_args;
				physics_args.GetAdd("bind") = true;
				physics_args.GetAdd("test.fn") = String("fixed");
				box3.AddComponent("physics", &physics_args);
			}

			{
				EntityContext ball = world_pool.AddEntity("ball");
				ball.AddComponent("transform3");
				ball.AddComponent("body");

				ArrayMap<String, Value> model_args;
				model_args.GetAdd("builtin") = String("sphere");
				ball.AddComponent("model", &model_args);

				ArrayMap<String, Value> physics_args;
				physics_args.GetAdd("bind") = true;
				physics_args.GetAdd("test.fn") = String("do.circle");
				ball.AddComponent("physics", &physics_args);
			}

			{
				EntityContext player_body = world_pool.AddEntity("player.body");

				ArrayMap<String, Value> transform_args;
				transform_args.GetAdd("x") = 0;
				transform_args.GetAdd("y") = 0;
				transform_args.GetAdd("z") = 6;
				player_body.AddComponent("transform3", &transform_args);

				ArrayMap<String, Value> physics_args;
				physics_args.GetAdd("bind") = true;
				player_body.AddComponent("physics", &physics_args);

				ArrayMap<String, Value> player_body_args;
				player_body_args.GetAdd("height") = (double)1.74;
				player_body.AddComponent("player.body", &player_body_args);
			}

			auto add_player_hand = [&](const String& id, const String& hand_id) {
				EntityContext hand = world_pool.AddEntity(id);
				hand.AddComponent("transform3");
				hand.AddComponent("body");

				ArrayMap<String, Value> model_args;
				model_args.GetAdd("builtin") = String("cylinder,0.02,0.2");
				model_args.GetAdd("pitch") = -90;
				hand.AddComponent("model", &model_args);

				ArrayMap<String, Value> player_hand_args;
				player_hand_args.GetAdd("hand") = hand_id;
				player_hand_args.GetAdd("body") = String("world.player.body");
				player_hand_args.GetAdd("simulated") = true;
				hand.AddComponent("player.hand", &player_hand_args);
			};

			add_player_hand("player.hand.left", "left");
			add_player_hand("player.hand.right", "right");

			{
				EntityContext player_head = world_pool.AddEntity("player.head");
				player_head.AddComponent("transform3");
				player_head.AddComponent("viewable");

				ArrayMap<String, Value> viewport_args;
				viewport_args.GetAdd("fov") = 90;
				player_head.AddComponent("viewport", &viewport_args);

				player_head.AddComponent("camera.chase");

				ArrayMap<String, Value> player_head_args;
				player_head_args.GetAdd("body") = String("world.player.body");
				player_head.AddComponent("player.head", &player_head_args);
			}

			LOG(root_pool.GetTreeString(0));
			LOG(world_pool.GetTreeString(1));
		}
		else if (1) {
			sys->PostLoadFile(GetDataFile("07d_ecs_first_person_cam.eon"));
		}
		else {
			// Load eon file (parses AstNode and loads it)
			sys->PostLoadFile(GetDataFile("07d_ecs_first_person_cam.eon"));
		}
	};
	
	ShellMain(true);
}
