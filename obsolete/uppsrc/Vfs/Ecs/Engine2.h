#ifndef _Vfs_Ecs_Engine2_h_
#define _Vfs_Ecs_Engine2_h_


struct SerialLoaderBase {
	
	virtual ~SerialLoaderBase() {}
	virtual String LoadFile(String file_path) = 0;
	
};

struct SerialLoaderFactory {
	typedef SerialLoaderBase*(*NewFn)();
	struct Loader : Moveable<Loader> {
		NewFn fn;
		
	};
	
	static VectorMap<String,Loader>& GetLoaders() {static VectorMap<String,Loader> l; return l;}
	
	
	template <class T>
	static SerialLoaderBase* New() {return new T();}
	
	
	template <class T>
	static void Register(String file_ext) {
		Loader& l = GetLoaders().Add(file_ext);
		l.fn = &New<T>;
	}
	
	
	static String LoadFile(String file_path);
};

void DefaultStartup(Engine& mach);
void DefaultSerialInitializer(Engine& mach);
void DefaultSerialInitializerInternalEon(Engine& mach);

#endif
