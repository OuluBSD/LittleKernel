#include "Core.h"


NAMESPACE_UPP


void  TcpSocketReadStream::_Put(const void *data, dword size) {
	ASSERT_(0, "This stream is for reading only");
}

dword TcpSocketReadStream::_Get(void *data, dword size) {
	return sock->Get(data, size);
}

void  TcpSocketReadStream::Seek(int64 pos) {
	
}

int64 TcpSocketReadStream::GetSize() const {
	return 0;
}

void  TcpSocketReadStream::SetSize(int64 size) {
	ASSERT_(0, "This stream is for reading only");
}






void  TcpSocketWriteStream::_Put(const void *data, dword size) {
	sock->Put(data, size);
}

dword TcpSocketWriteStream::_Get(void *data, dword size) {
	ASSERT_(0, "This stream is for writing only");
	return 0;
}

void  TcpSocketWriteStream::Seek(int64 pos) {
	ASSERT_(0, "This stream is for writing only");
}

int64 TcpSocketWriteStream::GetSize() const {
	ASSERT_(0, "This stream is for writing only");
	return 0;
}

void  TcpSocketWriteStream::SetSize(int64 size) {
	
}

	
END_UPP_NAMESPACE
