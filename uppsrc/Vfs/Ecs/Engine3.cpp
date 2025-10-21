#include "Ecs.h"
#include <Core/EcsEngine/EcsEngine.h>


NAMESPACE_UPP

void (*Eon_PostLoadString)(Engine& eng, String script_str);
bool (*Eon_AddScriptLoader)(Engine& eng);

bool DefaultInitializer(Engine& mach, bool skip_eon_file) {
	return mach.CommandLineInitializer(skip_eon_file);
}

bool Engine::CommandLineInitializer(bool skip_eon_file) {
	SetCoutLog();
	CommandLineArguments cmd;
	DaemonBase& daemon = DaemonBase::Single();
	
	// Script interpreter platform
	cmd.AddArg('e', "The path for the eon file", true, "path");
	
	// Development platform (remote connection)
	cmd.AddArg('l', "Listen to connections", false);
	cmd.AddArg('p', "Port", true, "integer");
	cmd.AddArg('v', "Verbose", false);
	
	// Parse command line
	if (!cmd.Parse()) {
		cmd.PrintHelp();
		return false;
	}
	
	// Get rest of the command line arguments
	Value cmd_params = cmd.GetVariables();
	if (cmd_params.Is<ValueMap>()) {
		ValueMap m = cmd_params;
		for(int i = 0; i < m.GetCount(); i++)
			eon_params(m.GetKey(i)) = m.GetValue(i);
	}
	
	// Arguments for remote connection
	#if 0
	if (cmd.IsArg('l')) {
		daemon.Add("EnetServer");
		daemon.Add("Ecs");
		EnetServiceServer::SetVerbose(cmd.IsArg('v'));
		EnetServiceServer::port_arg = StrInt(cmd.GetArg('p'));
		if (EnetServiceServer::port_arg) {
			LOG("User gave listening port " << (int)EnetServiceServer::port_arg);
		}
		SetShellMode(SHELLMODE_REMOTE);
	}
	#endif
	
	// Arguments for script interpreter platform
	const auto& inputs = cmd.GetInputs();
	for(const auto& in : inputs) {
		if (in.key == 'e') {
			eon_file = in.value;
		}
	}
	
	// Remote connection startup
	#if 0
	if (IsShellMode(SHELLMODE_REMOTE)) {
		// Start daemon in a separate thread
		if (!daemon.Init()) {
			LOG("DefaultInitializer: error: couldn't start daemon");
			return false;
		}
		daemon.RunInThread();
	}
	#endif
	
	// Script interpreter platform startup
	if (!skip_eon_file) {
		if (eon_file.IsEmpty()) {
			cmd.PrintHelp();
			LOG("");
			LOG("\te.g. -e play_audio_file.eon -MACHINE_TIME_LIMIT=3 -FILE=/home/user/some.mp3");
			return false;
		}
		
		eon_file = RealizeEonFile(eon_file);
		
		if (GetFileExt(eon_file) != ".eon") {
			eon_script = SerialLoaderFactory::LoadFile(eon_file);
			if (eon_script.IsEmpty())
				return false;
		}
	}
	
	return true;
}


size_t break_addr = 0;



void DefaultSerialInitializer0(Engine& mach, bool skip_eon_file) {
	SetCoutLog();
	
	if (0)
		break_addr = 0x1;
	
	if (!mach.CommandLineInitializer(skip_eon_file))
		mach.SetFailed("Default serial initializer failed");
	
}

void DefaultSerialInitializer(Engine& mach) {DefaultSerialInitializer0(mach, false);}

void DefaultSerialInitializerInternalEon(Engine& mach) {
	DefaultSerialInitializer0(mach, true);
}

void DefaultStartup(Engine& mach) {
	LOG("<-- Startup -->");
}




int __dbg_time_limit;





void Engine::MainLoop(bool (*fn)(void*), void* arg) {
	int iter = 0;
    TimeStop t, total;
    double sleep_dt_limit = 0.001;
    int fast_iter = 0;
    while (this->IsRunning() && !Thread::IsShutdownThreads()) {
        double dt = ResetSeconds(t);
        
        this->Update(dt);
        
        if (fn)
            fn(arg);
        
        if (dt < sleep_dt_limit && fast_iter > 5) {
			Sleep(1);
			fast_iter = 0;
        }
        else fast_iter++;
        
        double total_seconds = total.Seconds();
        if (__dbg_time_limit > 0 && total_seconds >= __dbg_time_limit)
            this->SetNotRunning();
    }
    
    //RuntimeDiagnostics::Static().CaptureSnapshot();
}

bool Engine::StartMain(String script_content, String script_file, ValueMap args, bool dbg_ref_visits, uint64 dbg_ref) {
	SetCoutLog();
	
	__dbg_time_limit = 0;
	
	int i = args.Find("MACHINE_TIME_LIMIT");
	if (i >= 0)
		__dbg_time_limit = args.GetValue(i);
	
	{
		Engine& mach = *this;
		
		//RuntimeDiagnostics::Static().SetRoot(mach);
		
	    try {
			bool fail = false;
			{
				if (!mach.IsStarted()) {
				    Ptr<LinkSystem> lsys			= mach.FindAdd<LinkSystem>();
				    
				    mach.FindAdd<PacketTracker>();
				}
				
			    if (Eon_AddScriptLoader) {
			        if (!Eon_AddScriptLoader(mach))
			            return false;
			    }
				
				if (!script_file.IsEmpty()) {
					String path = RealizeEonFile(script_file);
					
					String script_str;
					if (script_content.IsEmpty()) {
						script_str = LoadFile(path);
						if (script_str.IsEmpty()) {
							LOG("Empty file in " << path);
							return false;
						}
					}
					else
						script_str = script_content;
					
					if (script_str.IsEmpty()) {
						LOG("No script");
						return false;
					}
					for(int i = 0; i < args.GetCount(); i++) {
						String key = "${" + args.GetKey(i).ToString() + "}";
						String value = args[i].ToString();
						value = EscapeString(value);
						script_str.Replace(key, value);
					}
					LOG(script_str);
				
					if (Eon_PostLoadString)
						Eon_PostLoadString(mach, script_str);
					else {
						SetFailed("No Eon loader included in this executable");
						return false;
					}
				}
			    else {
			        WhenUserInitialize(*this);
			    }
		    }
		    
		    if (!fail) {
		        if (!mach.IsStarted())
					fail = !mach.Start();
		    }
	    }
	    catch (Exc e) {
	        SetFailed(e);
	    }
	    catch (std::exception e) {
	        SetFailed(e.what());
	    }
	    catch (...) {
	        SetFailed("unknown");
	    }
	}
	return !is_failed;
}

void Engine::StartMainLoop() {
	main_thrd.Stop();
	main_thrd.Start();
	Thread::Start([this]{
		this->MainLoop();
	});
}

void Engine::MainLoop() {
	
	if (is_started &&
		is_initialized &&
		!is_failed) {
		main_thrd.Start();
		try {
		    {
		        if (!WhenUserProgram)
					MainLoop(0,0);
		        else {
		            WhenUserProgram(*this);
		        }
		    }
	    }
	    catch (Exc e) {
	        LOG("error: " << e);
	    }
	    catch (std::exception e) {
	        LOG("error: " << e.what());
	    }
	    catch (...) {
	        LOG("unknown error");
	    }
	}
	
    main_thrd.SetStopped();
}


String SerialLoaderFactory::LoadFile(String file_path) {
	Cout() << "SerialLoaderFactory::LoadFile" << file_path << "\n";
	const auto& l = GetLoaders();
	for(int i = 0; i < l.GetCount(); i++) {
		Cout() << i << ": " << l.GetKey(i) << "\n";
	}
	String ext = GetFileExt(file_path);
	int i = l.Find(ext);
	if (i < 0) {
		LOG("SerialLoaderFactory: error: no loader for file extension: " << ext);
		return String();
	}
	const Loader& el = l[i];
	One<SerialLoaderBase> loader = el.fn();
	return loader->LoadFile(file_path);
}

END_UPP_NAMESPACE
