#ifndef _Vfs_Ast_h_
#define _Vfs_Ast_h_




struct SemanticParser;
struct EonStd;
struct AstNode;

bool IsPartially(CodeCursor src, CodeCursor t);

struct Endpoint : Moveable<Endpoint> {
	AstNode* n;
	FileLocation rel_loc;
	
	Endpoint();
	Endpoint(AstNode& n);
	Endpoint(AstNode* n);
	void operator=(const Endpoint& ep);
	String ToString() const;
};

struct AstNode :
	VfsValueExt
{
	static const int ARG_COUNT = 4;
	
	const AstNode* prev = 0;
	AstNode* type = 0;
	AstNode* arg[ARG_COUNT] = {0,0,0,0};
	AstNode* rval = 0;
	AstNode* ctx_next = 0;
	mutable bool locked = false;
	Value obj;
	
	CodeCursor src = Cursor_Null;
	//StmtType stmt = STMT_Null;
	//OpType op = OP_NULL;
	//ConstType con = CONST_NULL;
	CodeCursor filter = Cursor_Null;
	FileLocation loc;
	PathIdentifier id;
	
	union {
		int64 i64;
		double dbl;
	};
	String str;
	
public:
	CLASSTYPE(AstNode);
	AstNode(VfsValue& v);
	void Visit(Vis& v) override {}
	
	//void			Clear() {sub.Clear();}
	void			CopyFrom(EonStd* e, const AstNode& n);
	void			CopyFromValue(const FileLocation& loc, const Value& n);
	void			CopyToValue(Value& n) const;
	
	AstNode&		Add(const FileLocation& loc, String name="", int idx=-1);
	AstNode&		GetAdd(const FileLocation& loc, String name="");
	AstNode&		GetAdd(const FileLocation& loc, CodeCursor accepts);
	AstNode*		Find(String name, CodeCursor accepts=Cursor_Null);
	const AstNode*	Find(String name, CodeCursor accepts=Cursor_Null) const;
	AstNode*		FindPartial(CodeCursor t);
	AstNode*		Find(CodeCursor t);
	const AstNode*	Find(CodeCursor t) const;
	String			GetConstantString() const;
	AstNode*		FindWithPrevDeep(const AstNode* prev);
	void			FindAll(Vector<Endpoint>& ptrs, CodeCursor accepts, const FileLocation* rel_loc=0);
	void			FindAllStmt(Vector<Endpoint>& ptrs, CodeCursor accepts, const FileLocation* rel_loc=0);
	void			FindAllNonIdEndpoints(Vector<Endpoint>& ptrs, CodeCursor accepts=Cursor_Null, const FileLocation* rel_loc=0);
	void			FindAllNonIdEndpoints0(Vector<Endpoint>& ptrs, CodeCursor accepts=Cursor_Null, const FileLocation* rel_loc=0);
	void			FindAllNonIdEndpoints2(Vector<Endpoint>& ptrs, CodeCursor accepts1, CodeCursor accepts2, const FileLocation* rel_loc=0);
	void			FindAllNonIdEndpoints20(Vector<Endpoint>& ptrs, CodeCursor accepts1, CodeCursor accepts2, const FileLocation* rel_loc=0);
	
	String			GetTreeItemString() const;
	String			GetTreeString(int indent, bool links) const;
	String			GetTreeString(int indent=0) const override;
	String			GetCodeString(const CodeArgs2& args) const;
	String			ToString() const override;
	String			GetName() const override {return val.id;}
	String			GetPartStringArray() const;
	CodeCursor		GetCodeCursor() const {return src;}
	bool			IsPartially(CodeCursor t) const;// {return (CodeCursorPrimitive)src & (CodeCursorPrimitive)t;}
	bool			IsPartially(CodeCursor src, CodeCursor t) const;
	bool			IsStmtPartially(CodeCursor t) const;// {return src == Cursor_Stmt && ((CodeCursorPrimitive)stmt & (CodeCursorPrimitive)t);}
	
};


struct AstNodeLess {
	bool operator()(const AstNode* a, const AstNode* b) const {
		return a->loc < b->loc;
	}
	bool operator()(const Endpoint& a, const Endpoint& b) const {
		return a.rel_loc < b.rel_loc;
	}
};

Value EvaluateAstNodeValue(AstNode& n);




#endif
