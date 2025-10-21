#ifndef _Eon_Core_Network_h_
#define _Eon_Core_Network_h_


enum {
	NET_LATEST_BRIGHT_FRAME = 10100,
	NET_LATEST_DARK_FRAME,
	
	NET_SEND_FUSION_DATA = 10200,
	
	NET_GEOM_LOAD_ENGINE = 10300,
	NET_GEOM_STORE_ENGINE,
	
};




class TcpSocketReadStream : public Stream {
	TcpSocket* sock = 0;
	
	void  _Put(const void *data, dword size) override;
	dword _Get(void *data, dword size) override;
	
	void  Seek(int64 pos) override;
	int64 GetSize() const override;
	void  SetSize(int64 size) override;
	
public:
	TcpSocketReadStream(TcpSocket& s) : sock(&s) {}
	
};

class TcpSocketWriteStream : public Stream {
	TcpSocket* sock = 0;
	
	void  _Put(const void *data, dword size) override;
	dword _Get(void *data, dword size) override;
	
	void  Seek(int64 pos) override;
	int64 GetSize() const override;
	void  SetSize(int64 size) override;
	
public:
	TcpSocketWriteStream(TcpSocket& s) : sock(&s) {}
	
};



#endif
