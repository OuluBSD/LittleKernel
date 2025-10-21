#ifndef _AI_Core_Core_NatLang_h_
#define _AI_Core_Core_NatLang_h_



class NaturalTokenizer {
	WString ws;
	Vector<Vector<WString>> lines;
	bool has_foreign = false;
	
public:
	typedef NaturalTokenizer CLASSNAME;
	NaturalTokenizer();
	
	void Clear() {ws.Clear(); lines.Clear(); has_foreign = false;}
	bool Parse(const String& txt);
	bool HasForeign() const {return has_foreign;}
	
	const Vector<Vector<WString>>& GetLines() const {return lines;}
	
	static bool IsToken(int chr);
	
};



#endif
