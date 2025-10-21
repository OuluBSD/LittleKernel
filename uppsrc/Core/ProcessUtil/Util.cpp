#include "ProcessUtil.h"
#include <Core/MathNumeric/MathNumeric.h>

NAMESPACE_UPP

double FastMultiply(double prev_norm, int64 count, double multiplier) {
	if (count <= 0)
		return multiplier;
	if (prev_norm == 0.0 || multiplier == 0.0)
		return 0.0;
	ASSERT(count < 300);
	return FastPow(FastPow(prev_norm, (double)count) * multiplier, 1.0 / (count + 1));
}

void RenameFile(String oldpath, String newpath) {
	rename(oldpath.Begin(), newpath.Begin());
}

void TouchFile(String path) {
	if (!FileExists(path)) {
		FileOut fout(path);
		fout.Close();
	}
}





String EscapeString(String s) {
	s.Replace("\n", "\\n");
	s.Replace("\t", "\\t");
	s.Replace("\r", "\\r");
	s.Replace("\"", "\\\"");
	s.Replace("\\", "\\\\");
	
	return s;
}

String EscapeCharacter(String s) {
	s.Replace("\n", "\\n");
	s.Replace("\t", "\\t");
	s.Replace("\r", "\\r");
	s.Replace("\"", "\\\"");
	s.Replace("\'", "\\\'");
	s.Replace("\\", "\\\\");
	
	return s;
}

String ReadFileName(String s) {
	String out;
	bool force_add = false;
	for(int i = 0; i < s.GetCount(); i++) {
		int chr = s[i];
		if (force_add)
			out.Cat(chr);
		else {
			if (chr == '\\')
				force_add = true;
			else if (IsSpace(chr))
				break;
			else
				out.Cat(chr);
		}
	}
	return out;
}

String GetLineNumStr(String s, bool from_zero) {
	Vector<String> lines = Split(s, "\n", false);
	String out;
	const int tab_size = 4;
	int begin = from_zero ? 0 : 1;
	for(int i = 0; i < lines.GetCount(); i++) {
		String& l = lines[i];
		String nstr = IntStr(begin + i);
		if (l.IsEmpty()) {
			out << nstr << "\n";
		}
		else {
			nstr.Cat('\t', nstr.GetCount() < tab_size ? 2 : 1);
			out << nstr << l << "\n";
		}
	}
	return out;
}

bool ScanBoolString(const String& s) {
	return ToLower(s) == "true" || s == "1";
}

bool IsConstChar(const WString& n, const char* cmp, int cmp_len) {
	if (n.GetCount() < cmp_len)
		return false;
	for(int i = 0; i < cmp_len; i++) {
		int chr = n[i];
		if (chr != cmp[i])
			return false;
	}
	if (cmp_len == n.GetCount())
		return true;
	
	return n[cmp_len] == ',';
}

bool IsConstCharEnd(const WString& n, const char* cmp, int cmp_begin, int cmp_len) {
	for(int i = cmp_begin; i < cmp_len; i++) {
		int chr = n[i];
		if (chr != cmp[i])
			return false;
	}
	return true;
}

bool IsArg(const WString& n, const char* cmp, int cmp_len) {
	int count = n.GetCount();
	if (count < cmp_len)
		return false;
	
	for(int i = 0; i < cmp_len; i++) {
		int chr = n[count-cmp_len+i];
		if (chr != cmp[i])
			return false;
	}
	if (cmp_len == n.GetCount())
		return true;
	
	return n[count-cmp_len-1] == ',';
}

bool IsAllSpace(const String& a) {
	bool is_space = true;
	for(int i = 0; i < a.GetCount() && is_space; i++)
		if (!IsSpace(a[i]))
			is_space = false;
	return is_space;
}

String CamelToName(String s) {
	String o;
	bool is_cap = true;
	
	for(int i = 0; i < s.GetCount(); i++) {
		int chr = s[i];
		
		if (chr == '_') {
			is_cap = true;
		}
		else {
			if (is_cap || IsUpper(chr))
				o.Cat(ToUpper(chr));
			else
				o.Cat(ToLower(chr));
			is_cap = false;
		}
	}
	
	return o;
}

String ClassPathTop(String s) {
	Vector<String> parts = Split(s, "::");
	return parts.Top();
}

String ToVarName(String s, char sep) {
	String o;
	
	bool all_upper = true;
	for(int i = 0; i < s.GetCount() && all_upper; i++) {
		int chr = s[i];
		if (IsUpper(chr) || chr == sep)
			;
		else
			all_upper = false;
	}
	if (all_upper)
		return ToLower(s);
	
	for(int i = 0; i < s.GetCount(); i++) {
		int chr = s[i];
		
		if (IsUpper(chr)) {
			if (i > 0)
				o.Cat(sep);
			o.Cat(ToLower(chr));
		}
		else {
			o.Cat(chr);
		}
	}
	
	return o;
}

String ToCaps(String s) {
	String out;
	
	for(int i = 0; i < s.GetCount(); i++) {
		int chr = s[i];
		
		if (i && IsUpper(chr))
			out << "_";
		out.Cat(ToUpper(chr));
	}
	
	return out;
}



void GetDirectoryFiles(String dir, Index<String>& files) {
	files.Clear();
	FindFile ff;
	if (ff.Search(AppendFileName(dir, "*"))) {
		do {
			String name = ff.GetName();
			if (name == "." || name == "..") continue;
			files.Add(ff.GetPath());
		}
		while (ff.Next());
	}
}

String Join(const Vector<int>& v, String join_str) {
	String s;
	int i = 0;
	for (int val : v) {
		if (i++ > 0)
			s << join_str;
		s << IntStr(val);
	}
	return s;
}




















String TrimTrailingDirSep(String file) {
	while (file.GetCount() && file[file.GetCount()-1] == DIR_SEP)
		file = file.Left(file.GetCount()-1);
	
	return file;
}







bool __enable_opengl_debug;
bool IsGfxAccelDebugMessages() {return __enable_opengl_debug;}
void EnableGfxAccelDebugMessages(bool b) {__enable_opengl_debug = b;}




String CenteredString(const String& s, int lw) {
	String o;
	Vector<String> lines = Split(s, "\n");
	for(int i = 0; i < lines.GetCount(); i++) {
		if (i) o.Cat('\n');
		String& j = lines[i];
		WString l = j.ToWString();
		int c = l.GetCount();
		int pad = (lw - c) / 2;
		if (pad > 0)
			o.Cat(' ', pad);
		o.Cat(j);
	}
	return o;
}

String FormatScientific(double d) {
	char buffer [100];
	int len = snprintf ( buffer, 100, "%.8g", d);
	return String(buffer, len);
}



float ConvertDipsToPixels(float dips, float dpi) {
    constexpr float dips_per_inch = 96.0f;
    return floorf(dips * dpi / dips_per_inch + 0.5f); // Round to nearest integer.
}




const Index<String>& Genders() {
	static Index<String> genders;
	ONCELOCK {
		genders.Add("male");
		genders.Add("female");
		genders.Add("other");
	}
	return genders;
}

END_UPP_NAMESPACE
