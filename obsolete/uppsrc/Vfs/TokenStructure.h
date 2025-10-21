#ifndef _Vfs_TokenStructure_h_
#define _Vfs_TokenStructure_h_



struct TokenStructure;

struct TokenNode :
	VfsValueExt
{
	const Token* begin = 0;
	const Token* end = 0;
	
public:
	CLASSTYPE(TokenNode);
	TokenNode(VfsValue& v);
	void Visit(Vis& v) override {}
	
	void		Clear();
	
	TokenNode&	Add();
	
	String		GetTreeString(int indent=0) const override;
	String		GetCodeString(const CodeArgs2& args) const;
	String		ToString() const override;
	
};

struct TokenStructure :
	VfsValueExt,
	ErrorSource {
	
private:
	// Temp
	const Token *iter, *end;
	bool IsEnd() const {ASSERT(iter <= end); return iter == end;}
	
public:
	CLASSTYPE(TokenStructure);
	TokenStructure(VfsValue& v);
	void Visit(Vis& v) override {}
	
	TokenNode& GetRoot() const;
	
	bool ProcessEon(const Tokenizer& t);
	bool ParseBlock(TokenNode& n);
	bool ParseStatement(TokenNode& n, bool break_comma);
	
	bool PassType(int tk);
	const Token& Current() {ASSERT(iter <= end); return *iter;}
	bool Next() {return ++iter != end;}
	
	String		GetTreeString(int indent=0) const override;
	String		GetCodeString(const CodeArgs2& args) const;
	String		ToString() const override;
	
};




#endif
