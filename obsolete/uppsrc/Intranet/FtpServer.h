#ifndef _Intranet_FtpServer_h_
#define _Intranet_FtpServer_h_

#ifdef flagFTP
NAMESPACE_UPP

struct FTPClientConnection {

	One<TcpSocket> data_socket;
	String command, arg, arg2, cwd;
	TcpSocket* sock;
	int state;
	bool passive, running, stopped;

	String Scan();
	String GetSysDir(String cwd);
	String AppendCwd(String s);
	String AppendCwdSys(String s);
	String ChangeDirectory(String s);
	String ChangeDirectorySys(String s);
	void Response(String s);
	void CheckPassive();

public:
	FTPClientConnection();

	void Stop();
	void Process(TcpSocket& sock);
	bool IsStopped() const {return stopped;}
};

class FTPServer {

protected:
	ArrayMap<int, FTPClientConnection> client_list;
	Vector<int> rm_queue;
	Mutex lock;
	int port;
	bool running, stopped;
	Atomic client_counter;

	void ServeClient(int id, TcpSocket* sock);
	void Listener();

public:
	typedef FTPServer CLASSNAME;
	FTPServer();
	~FTPServer();
	void Start();
	void Stop();

	void SetPort(uint16 p) { port = p; }
};

END_UPP_NAMESPACE
#endif

#endif
