#include "Core.h"


NAMESPACE_UPP


String KeyToName(String s) {
	s = ToLower(s);
	String o;
	bool upper = true;
	for(int i = 0; i < s.GetCount(); i++) {
		int chr = s[i];
		if (chr == '_') {
			upper = true;
			o.Cat(' ');
			continue;
		}
		if (upper && chr >= 'a' && chr <= 'z')
			chr = ToUpper(chr);
		upper = false;
		o.Cat(chr);
	}
	return o;
}

String StringToName(String s) {
	s = ToLower(s);
	String o;
	bool upper = true;
	for(int i = 0; i < s.GetCount(); i++) {
		int chr = s[i];
		if (chr == ' ') {
			upper = true;
			continue;
		}
		if (upper && chr >= 'a' && chr <= 'z')
			chr = ToUpper(chr);
		upper = false;
		o.Cat(chr);
	}
	return o;
}


END_UPP_NAMESPACE
