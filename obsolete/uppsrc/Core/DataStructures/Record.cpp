#include "DataStructures.h"


NAMESPACE_UPP
using namespace UPP;





Record::Record() {
	
}

Record::Record(const Record& r) {
	
}


void Record::operator=(const Record& r) {
	
}

void Record::Clear() {
	if (data) {
		data->Retain();
		data = 0;
	}
}

#define PROXY_TO_DATA(x, def) if (data) data->buffer.x; return def;

int Record::GetSampleRate() const {
	PROXY_TO_DATA(GetSampleRate(), 0)
}

float Record::Get(int ch, int i) const {
	PROXY_TO_DATA(Get(ch, i), 0)
}

int Record::GetCount() const {
	PROXY_TO_DATA(GetCount(), 0)
}

int Record::GetChannels() const {
	PROXY_TO_DATA(GetChannels(), 0)
}

bool Record::IsEmpty() const {
	return !data;
}









Record StreamRecord::LoadAny(String path) {
	TODO
	return Record();
}


END_UPP_NAMESPACE
