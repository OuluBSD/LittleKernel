#include "Core.h"

NAMESPACE_UPP

static void DaemonBase_signal_handler(int sig)
{
	#ifdef flagPOSIX
	signal(sig, SIG_IGN);
	#else
	TODO
	#endif
	
	Cout() << "DaemonBase - stopping\n";

	DaemonBase::Latest()->SetNotRunning();
	Thread::ShutdownThreads();
}




DaemonService* DaemonService::FindService(String name) const {
	return base->FindService(name);
}


DaemonBase::DaemonBase() {
	Latest() = this;
}

DaemonBase::~DaemonBase() {
	if (flag.IsRunning()) {
		Stop();
		Deinit();
	}
}

bool DaemonBase::Init() {
	#ifdef flagPOSIX
	signal(SIGINT, DaemonBase_signal_handler);
	signal(SIGABRT, DaemonBase_signal_handler);
	signal(SIGTERM, DaemonBase_signal_handler);
	signal(SIGQUIT, DaemonBase_signal_handler);
	signal(SIGHUP, DaemonBase_signal_handler);
	#else
	LOG("DaemonBase::Init: warning: TODO signal handlers");
	#endif
	
	if (requested_services.IsEmpty()) {
		LOG("DaemonBase::Init: error: no requested service");
		return false;
	}
	
	for (String svc : requested_services) {
		int i  = NewFnArray().Find(svc);
		if (i < 0) {
			LOG("DaemonBase::Init: error: no service named '" << svc << "' registered");
			return false;
		}
		DaemonService& ds = services.Add(svc, NewFnArray()[i]());
		ds.base = this;
	}
	
	for(int i = 0; i < services.GetCount(); i++) {
		String name = services.GetKey(i);
		DaemonService& svc = services[i];
		if (!svc.Init(name)) {
			LOG("DaemonBase::Init: error: init failed for service named '" << name << "'");
			return false;
		}
	}
	
	inited = true;
	return true;
}

void DaemonBase::Run() {
	flag.Stop();
	flag.Start(1);
	
	TimeStop ts;
	while (	 flag.IsRunning() &&
			(!timeout || ts.Seconds() < timeout) &&
			 !Thread::IsShutdownThreads()) {
		Update();
		Sleep(10);
	}
	flag.DecreaseRunning();
}

void DaemonBase::Update() {
	for (DaemonService& s : services.GetValues())
		s.Update();
}

void DaemonBase::SetNotRunning() {
	flag.SetNotRunning();
}

void DaemonBase::Stop() {
	flag.Stop();
	
	for(int i = 0; i < services.GetCount(); i++) {
		String name = services.GetKey(i);
		DaemonService& svc = services[i];
		svc.Stop();
	}
	
	inited = false;
}

void DaemonBase::Deinit() {
	for(int i = 0; i < services.GetCount(); i++) {
		String name = services.GetKey(i);
		LOG("DaemonBase::Deinit: " << name);
		DaemonService& svc = services[i];
		svc.Deinit();
	}
	services.Clear();
}

void DaemonBase::DefaultProcedure() {
	if (!Init())
		return;
	
	Run();
	Stop();
	Deinit();
}

DaemonService* DaemonBase::FindService(String name) {
	int i = services.Find(name);
	if (i < 0)
		return 0;
	return &services[i];
}

void DaemonBase::Add(String svc_name) {
	ASSERT(!svc_name.IsEmpty());
	requested_services.FindAdd(svc_name);
}

DaemonBase& DaemonBase::Single() {
	return UPP::Single<DaemonBase>();
}






#ifdef flagGLIB2
bool Glib2Daemon::Init() {
	if (!DaemonBase::Init())
		return false;
	
	loop = g_main_loop_new(NULL, TRUE);
	if (!loop)
		return false;
	
	return true;
}

void Glib2Daemon::Run() {
	g_main_loop_run(loop);
	
	DaemonBase::Run();
}

void Glib2Daemon::Stop() {
	DaemonBase::Stop();
	
	g_main_loop_quit(loop);
}

void Glib2Daemon::Deinit() {
	DaemonBase::Deinit();
	
	g_main_loop_unref(loop);
}
#endif





SerialServiceBase::SerialServiceBase() {
	
}

/*bool SerialServiceBase::AddTcpSocket(uint32 magic, Callback1<TcpSocket&> cb) {
	int i = handlers.Find(magic);
	if (i >= 0) {
		TcpSocketHandler* h = CastPtr<TcpSocketHandler>(&handlers[i]);
		if (!h)
			return false;
		h->cb << cb;
		return true;
	}
	TcpSocketHandler* h = new TcpSocketHandler();
	h->cb = cb;
	h->magic = magic;
	h->in_sz = 0;
	h->out_sz = 0;
	h->serialized = false;
	h->socket_handler = true;
	handlers.Add(magic, h);
	return true;
}
*/


SerialServiceServer::SerialServiceServer() {
	
}

SerialServiceServer::~SerialServiceServer() {
	CloseTcp();
	StopThread();
}

bool SerialServiceServer::ListenTcp(uint16 port) {
	
	if (tcp.IsOpen())
		CloseTcp();
	
	tcp.Timeout(500);
	
	if (!tcp.Listen(port))
		return false;
	
	return true;
}

void SerialServiceServer::CloseTcp() {
	tcp.Close();
}

void SerialServiceServer::StartThread() {
	StopThread();
	
	ASSERT(tcp.IsOpen());
	if (!tcp.IsOpen()) return;
	
	Thread::Start(THISBACK(ListenerHandler));
}

void SerialServiceServer::ListenerHandler() {
	flag.Start(1);
	
	tcp.Timeout(500);
	
	while (tcp.IsOpen() && flag.IsRunning()) {
		One<TcpSocket> sock;
		sock.Create();
		if (sock->Accept(tcp)) {
			LOG("SerialServiceServer::ListenerHandler: info: accepted " << sock->GetPeerAddr());
			flag.IncreaseRunning();
			Thread::Start(THISBACK1(ClientHandler, sock.Detach()));
		}
	}
	
	flag.DecreaseRunning();
}

void SerialServiceServer::StopThread() {
	flag.Stop();
}

void SerialServiceServer::ClientHandler(TcpSocket* ptr) {
	One<TcpSocket> sock_owner = ptr;
	TcpSocket& sock = *ptr;
	
	LOG("SerialServiceServer::ClientHandler: starting handling client " << sock.GetPeerAddr());
	
	Vector<byte> in, out;
	StringStream ss;
	
	sock.Timeout(3000);
	int wait_count = 0;
	
	while (sock.IsOpen() && flag.IsRunning()) {
		int got = 0, sent = 0;
		
		uint32 magic = 0, in_sz = 0, out_sz = 0;
		
		#define RECV(x) \
			got = sock.Get(&x, sizeof(x));
		#define GET_ERROR(x) \
			if (got != sizeof(x)) { \
				LOG("SerialServiceServer::ClientHandler: error: expected " << (int)sizeof(x) << ", but got " << got); break;}
		#define GET(x) \
			RECV(x) \
			GET_ERROR(x)
		#define SEND(x) \
			sent = sock.Put(&x, sizeof(x)); \
			if (sent != sizeof(x)) { \
				LOG("SerialServiceServer::ClientHandler: error: expected to send " << (int)sizeof(x) << ", but sent " << sent); break;}
		
		RECV(magic);
		if (!magic) {
			if (++wait_count >= keepalive_limit)
				break;
			continue;
		}
		GET_ERROR(magic);
		
		wait_count = 0;
		
		// Keepalive magic
		if (magic == 1)
			continue;
		
		int i = handlers.Find(magic);
		if (i < 0) {
			LOG("SerialServiceServer::ClientHandler: error: could not find magic " << magic);
			magic = 0;
			SEND(magic);
			continue;
		}
		SEND(magic);
		
		//LOG("SerialServiceServer::ClientHandler: info: magic " << magic << ", i " << i);
		
		HandlerBase& hb = handlers[i];
		
		GET(in_sz);
		if (in_sz > 10000000) {
			LOG("SerialServiceServer::ClientHandler: error: too large input packet: " << in_sz);
			break;
		}
		
		GET(out_sz);
		
		
		in.SetCount(in_sz);
		got = sock.Get(in.Begin(), in.GetCount());
		if (got != in_sz) {
			LOG("SerialServiceServer::ClientHandler: error: expected " << in_sz << ", but got " << got);
			break;
		}
		
		switch (hb.fn_type) {
			case FN_FIXED: {
				if (out_sz != hb.out_sz) {
					LOG("SerialServiceServer::ClientHandler: error: unexpected output size: " << out_sz << " (expected " << hb.out_sz << ")");
					break;
				}
				out.SetCount(hb.out_sz);
				hb.Call(in, out);
				sent = sock.Put(out.Begin(), out.GetCount());
				
				if (sent != out_sz) {
					LOG("SerialServiceServer::ClientHandler: error: couldn't send full structure (" << sent << " < " << hb.out_sz << ")");
					break;
				}
				break;
			}
			
			case FN_SERIALIZED:
			case FN_STREAMED: {
				if (out_sz != 0) {
					LOG("SerialServiceServer::ClientHandler: error: unexpected output size: " << out_sz);
					break;
				}
				
				MemStream ms(in.Begin(), in.GetCount());
				//ms.SetLoading();
				ss.SetSize(0);
				ss.SetStoring();
				
				hb.Call(ms, ss);
				
				String result = ss.GetResult();
				out_sz = result.GetCount();
				SEND(out_sz);
				
				sent = sock.Put(result.Begin(), result.GetCount());
				
				if (sent != result.GetCount()) {
					LOG("SerialServiceServer::ClientHandler: error: couldn't send full memory (" << sent << " < " << result.GetCount() << ")");
					break;
				}
				break;
			}
		}
	}
	#undef RECV
	#undef GET_ERROR
	#undef GET
	#undef SEND
	
	flag.DecreaseRunning();
	
	LOG("SerialServiceServer::ClientHandler: stopped handling client " << sock.GetPeerAddr());
}

bool SerialServiceServer::Init(String name) {
	
	if (!ListenTcp(7776)) {
		LOG("SerialServiceServer::Init: Could not listen port 7776");
		return false;
	}
	
	StartThread();
	
	return true;
}

void SerialServiceServer::Update() {
	
}

void SerialServiceServer::Deinit() {
	tcp.Shutdown();
	tcp.Close();
	StopThread();
}





SerialServiceClient::SerialServiceClient() {
	
}

SerialServiceClient::~SerialServiceClient() {
	CloseTcp();
}

bool SerialServiceClient::ConnectTcp(String addr, uint16 port) {
	CloseTcp();

	return tcp.Connect(addr, port);
}

void SerialServiceClient::CloseTcp() {
	tcp.Close();
}

#define RECV(x) \
	got = sock.Get(&x, sizeof(x));
#define GET_ERROR(x) \
	if (got != sizeof(x)) { \
		LOG("SerialServiceClient::CallMem: error: expected " << (int)sizeof(x) << ", but got " << got); sock.Close(); return false;}
#define GET(x) \
	RECV(x) \
	GET_ERROR(x)
#define SEND(x) \
	sent = sock.Put(&x, sizeof(x)); \
	if (sent != sizeof(x)) { \
		LOG("SerialServiceClient::CallMem: error: expected to send " << (int)sizeof(x) << ", but sent " << sent); sock.Close(); return false;}


bool SerialServiceClient::CallMem(uint32 magic, const void* out, int out_sz, void* in, int in_sz) {
	if (!tcp.IsOpen())
		return false;
	
	auto& sock = tcp;
	uint32 sent = 0, got = 0, got_magic = 0;
	
	SEND(magic);
	GET(got_magic);
	if (!got_magic) {
		LOG("SerialServiceClient::CallMem: error: magic not found on server");
		return false;
	}
	SEND(out_sz);
	SEND(in_sz);
	
	sent = sock.Put(out, out_sz);
	if (sent != out_sz) {
		LOG("SerialServiceClient::CallMem: error: expected to send " << out_sz << ", but sent " << sent);
		return false;
	}
	
	got = sock.Get(in, in_sz);
	if (got != in_sz) {
		LOG("SerialServiceClient::CallMem: error: expected to get " << in_sz << ", but got " << got);
		return false;
	}
	
	return true;
}

bool SerialServiceClient::CallMem(uint32 magic, const void* out, int out_sz, Vector<byte>& in) {
	if (!tcp.IsOpen())
		return false;
	
	auto& sock = tcp;
	int in_sz = 0;
	uint32 sent = 0, got = 0, got_magic = 0;
	
	SEND(magic);
	GET(got_magic);
	if (!got_magic) {
		LOG("SerialServiceClient::CallMem: error: magic not found on server");
		return false;
	}
	SEND(out_sz);
	SEND(in_sz); // == 0
	
	sent = sock.Put(out, out_sz);
	if (sent != out_sz) {
		LOG("SerialServiceClient::CallMem: error: expected to send " << out_sz << ", but sent " << sent);
		return false;
	}
	
	GET(in_sz);
	in.SetCount(in_sz);
	got = sock.Get(in.Begin(), in_sz);
	if (got != in_sz) {
		LOG("SerialServiceClient::CallMem: error: expected to get " << in_sz << ", but got " << got);
		return false;
	}
	
	return true;
}

bool SerialServiceClient::CallStream(uint32 magic, Callback2<Stream&, Stream&> cb) {
	if (!tcp.IsOpen())
		return false;
	
	auto& sock = tcp;
	int in_sz = 0;
	uint32 sent = 0, got = 0, got_magic = 0;
	
	SEND(magic);
	GET(got_magic);
	if (!got_magic) {
		LOG("SerialServiceClient::CallMem: error: magic not found on server");
		return false;
	}
	
	TODO //cb(tcp);
	
	return true;
}

bool SerialServiceClient::Init(String name) {
	
	TODO
	return false;
}

void SerialServiceClient::Update() {
	
	TODO
	
}

void SerialServiceClient::Deinit() {
	
	TODO
	
}

	
#undef RECV
#undef GET_ERROR
#undef GET
#undef SEND


END_UPP_NAMESPACE
