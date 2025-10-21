#ifndef _Core_Daemon_h_
#define _Core_Daemon_h_

class DaemonBase;

class DaemonService {
	
protected:
	friend class DaemonBase;
	DaemonBase* base;
	
public:
	virtual ~DaemonService() {}
	
	virtual bool Init(String name) = 0;
	virtual void Update() = 0;
	virtual void Deinit() = 0;
	virtual void Stop() {}
	
	
	DaemonBase& GetBase() const {return *base;}
	DaemonService* FindService(String name) const;
	
};

class DaemonBase {
	ArrayMap<String, DaemonService> services;
	Index<String> requested_services;
	RunningFlag flag;
	bool inited = false;
	int timeout = 0;
	
public:
	typedef DaemonBase CLASSNAME;
	DaemonBase();
	virtual ~DaemonBase();
	
	DaemonBase& SetTimeout(int sec) {timeout = sec; return *this;}
	void Add(String svc_name);
	
	virtual bool Init();
	virtual void Run();
	virtual void Stop();
	virtual void Deinit();
	void Update();
	void DefaultProcedure();
	void SetNotRunning();
	
	void RunInThread() {Thread::Start(THISBACK(Run));}
	
	DaemonService* FindService(String name);
	template <class T> T* FindServiceT() {
		for (DaemonService& svc : services.GetValues()) {
			T* o = CastPtr<T>(&svc);
			if (o)
				return o;
		}
		return 0;
	}
	
	static DaemonBase*& Latest() {static DaemonBase* p; return p;}
	static DaemonBase& Single();
	
public:
	typedef DaemonService* (*NewFn)();
	static ArrayMap<String, NewFn>& NewFnArray() {static ArrayMap<String, NewFn> a; return a;}
	template <class T> static DaemonService* Create() {return new T();}
	template <class T> static void Register(String name) {NewFnArray().GetAdd(name) = &Create<T>;}
	
};


#ifdef flagGLIB2
struct Glib2Daemon : DaemonBase {
	GMainLoop *loop = NULL;
	GList *device_list = NULL;
	static int num_devices;
	
	Glib2Daemon() {}
	~Glib2Daemon() {}
	bool Init() override;
	void Run() override;
	void Stop() override;
	void Deinit() override;
	
};
#endif



class SerialServiceBase : public DaemonService {
	
public:
	
	typedef enum {
		FN_FIXED,
		FN_SERIALIZED,
		FN_STREAMED,
	} FnType;
	
	struct HandlerBase {
		virtual ~HandlerBase() {}
		
		uint32 magic = 0;
		uint32 in_sz = 0;
		uint32 out_sz = 0;
		FnType fn_type;
		
		virtual void Call(const Vector<byte>& in, Vector<byte>& out) {Panic("not implemented");}
		virtual void Call(Stream& in, Stream& out) {Panic("not implemented");}
	};
	
	template <class In, class Out> struct FixedHandlerT : HandlerBase {
		byte b_in[sizeof(In)];
		byte b_out[sizeof(Out)];
		
		Callback2<const In&, Out&> cb;
		
		void Call(const Vector<byte>& in, Vector<byte>& out) override {
			out.SetCount(sizeof(Out));
			const In* o_in = (const In*)(const byte*)in.Begin();
			Out* o_out = (Out*)(byte*)out.Begin();
			cb(*o_in, *o_out);
		}
		
		void Call(Stream& in, Stream& out) override {
			uint32 in_sz, out_sz;
			in.Get(&in_sz, sizeof(in_sz));
			in.Get(&out_sz, sizeof(out_sz));
			if (in_sz == sizeof(In) && out_sz == sizeof(Out)) {
				in.Get(b_in, sizeof(In));
				const In* o_in = (const In*)b_in;
				Out* o_out = (Out*)b_out;
				cb(*o_in, *o_out);
				out.Put(b_out, sizeof(Out));
			}
			else {
				#ifdef flagDEBUG
				LOG("FixedHandlerT: error: size mismatch");
				#endif
			}
		}
	};
	
	template <class In, class Out> struct SerializerHandlerT : HandlerBase {
		
		Callback2<const In&, Out&> cb;
		One<In> tmp_in;
		One<Out> tmp_out;
		
		void Call(Stream& in, Stream& out) override {
			if (tmp_in.IsEmpty()) {
				tmp_in.Create();
				tmp_out.Create();
			}
			in % *tmp_in;
			cb(*tmp_in, *tmp_out);
			out % *tmp_out;
		}
	};
	
	struct StreamHandler : HandlerBase {
		Callback2<Stream&, Stream&> cb;
		
		void Call(Stream& in, Stream& out) override {
			uint32 in_sz, out_sz;
			in.Get(&in_sz, 4);
			in.Get(&out_sz, 4);
			
			cb(in, out);
		}
	};
	
protected:
	ArrayMap<int, HandlerBase> handlers;
	
	
	template <class Handler, class In, class Out, class Cb=Callback2<const In&, Out&>>
	bool AddReceiverT(uint32 magic, Cb cb, FnType fn_type) {
		int i = handlers.Find(magic);
		if (i >= 0) {
			Handler* h = CastPtr<Handler>(&handlers[i]);
			if (!h)
				return false;
			h->cb << cb;
			return true;
		}
		Handler* h = new Handler();
		h->cb = cb;
		h->magic = magic;
		h->in_sz = sizeof(In);
		h->out_sz = sizeof(Out);
		h->fn_type = fn_type;
		//ASSERT(h->in_sz && h->out_sz);
		handlers.Add(magic, h);
		return true;
	}
	
public:
	typedef SerialServiceBase CLASSNAME;
	SerialServiceBase();
	
	
	template <class In, class Out>
	bool AddFixed(uint32 magic, Callback2<const In&, Out&> cb) {
		return AddReceiverT<FixedHandlerT<In,Out>,In,Out>(magic, cb, FN_FIXED);
	}
	
	template <class In, class Out>
	bool AddSerializer(uint32 magic, Callback2<const In&, Out&> cb) {
		return AddReceiverT<SerializerHandlerT<In,Out>,In,Out>(magic, cb, FN_SERIALIZED);
	}
	
	bool AddStream(uint32 magic, Callback2<Stream&, Stream&> cb) {
		return AddReceiverT<StreamHandler,dword,dword,Callback2<Stream&,Stream&>>(magic, cb, FN_STREAMED);
	}
	
};


class SerialServiceServer : public SerialServiceBase {
	TcpSocket tcp;
	RunningFlag flag;
	int keepalive_limit = 10;
	
	void ClientHandler(TcpSocket* sock);
public:
	typedef SerialServiceServer CLASSNAME;
	SerialServiceServer();
	~SerialServiceServer();
	
	
	bool ListenTcp(uint16 port);
	void StartThread();
	void ListenerHandler();
	
	void CloseTcp();
	void StopThread();
	
	
	// DaemonService
	bool Init(String name) override;
	void Update() override;
	void Deinit() override;
	
};


class SerialServiceClient : public SerialServiceBase {
	TcpSocket tcp;
	
	
public:
	typedef SerialServiceClient CLASSNAME;
	SerialServiceClient();
	~SerialServiceClient();
	
	bool ConnectTcp(String addr, uint16 port);
	void CloseTcp();
	bool CallMem(uint32 magic, const void* out, int out_sz, void* in, int in_sz);
	bool CallMem(uint32 magic, const void* out, int out_sz, Vector<byte>& in);
	bool CallStream(uint32 magic, Callback2<Stream&, Stream&> cb);
	//bool CallSocket(uint32 magic, Callback1<TcpSocket&> cb);
	
	template <class In, class Out>
	bool Call(uint32 magic, const In& in, Out& out) {
		return CallMem(magic, (const void*)&in, sizeof(In), (void*)&out, sizeof(Out));
	}
	
	template <class In, class Out>
	bool CallSerialized(uint32 magic, In& in, Out& out) {
		StringStream ss;
		ss.SetStoring();
		ss % in;
		String in_data = ss.GetResult();
		thread_local static Vector<byte> out_data;
		out_data.SetCount(0);
		if (!CallMem(magic, (const void*)in_data.Begin(), in_data.GetCount(), out_data))
			return false;
		MemStream ms(out_data.Begin(), out_data.GetCount());
		ms % out;
		return true;
	}
	
	bool IsOpen() const {return tcp.IsOpen();}
	
	
	// DaemonService
	bool Init(String name) override;
	void Update() override;
	void Deinit() override;
	
};




#endif
