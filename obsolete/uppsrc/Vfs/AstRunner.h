#ifndef _Vfs_AstRunner_h_
#define _Vfs_AstRunner_h_




struct AstRunner :
	VfsValueExt,
	EonStd,
	ErrorSource
{
	CLASSTYPE(AstRunner);
	AstRunner(VfsValue& v);
	void Visit(Vis& v) override {}
	
	
	bool		Execute(const AstNode& n);
	AstNode*	Visit(const AstNode& n);
	AstNode*	AddDuplicate(const AstNode& n);
	AstNode*	VisitMetaRVal(const AstNode& n);
	AstNode*	VisitMetaCtor(const AstNode& n);
	AstNode*	VisitMetaFor(const AstNode& n);
	AstNode*	VisitMetaIf(const AstNode& n);
	AstNode*	VisitMetaStaticFunction(const AstNode& n);
	bool		VisitMetaCall(AstNode& d, AstNode& rval, AstNode& args);
	AstNode*	VisitResolve(const AstNode& n);
	bool		VisitStatementBlock(const AstNode& n, bool req_rval);
	AstNode*	VisitReturn(const AstNode& n);
	AstNode*	VisitMetaResolve(const AstNode& n);
	AstNode*	Merge(const AstNode& n);
	AstNode*	MergeStatement(const AstNode& n);
	void		SetMetaBlockType(AstNode& d, AstNode* dup_block, AstNode* closest_type);
	AstNode*	DeclareMetaVariable(const AstNode& n);
	bool		IsMergeable(const AstNode& prev, const AstNode& to_merge) const;
	AstNode*	Evaluate(const AstNode& n);
	
	String		GetTreeString(int indent=0) const override;
	String		GetCodeString(const CodeArgs2& args) const;
	
	String		ToString() const override;
	AstNode&	GetRoot() override;
	AstNode&	GetRoot() const;
	
};




#endif
