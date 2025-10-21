#include "ProcessUtil.h"

NAMESPACE_UPP

String ProcMsg::ToString() const {
	String s;
	if (src.GetCount())
		s << ToUpper(src) << ": ";
	if (file.GetCount())
		s	<< GetFileName(file);
	if (line >= 0 || col >= 0)
		s	<< ":" << line
			<< ":" << col << ": ";
	s	<< GetSeverityString()
		<< ": " << msg;
	return s;
}

void ErrorSourceBuffered::DumpMessages() {
	for(const ProcMsg& pm : messages) {
		LOG(pm.ToString());
	}
}

END_UPP_NAMESPACE
