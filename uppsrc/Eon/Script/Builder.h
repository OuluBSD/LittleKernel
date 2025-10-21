#ifndef _Eon_Script_Builder_h_
#define _Eon_Script_Builder_h_


namespace Eon {


struct AtomBuilder
{
	Id id;
	VectorMap<String,Value> assigns;
	
	AtomBuilder&	Assign(String id, Value val);
	AstNode&		CreateNode(AstNode& root);
	
};

struct LoopBuilder
{
	Id id;
	Array<AtomBuilder> atoms;
	
	AtomBuilder&	AddAtom(String id);
	AstNode&		CreateNode(AstNode& root);
	
};

struct Builder : VfsValueExt
{
	METANODE_EXT_CONSTRUCTOR(Builder)
	
	Array<LoopBuilder> loops;
	
	LoopBuilder&	AddLoop(String id);
	AstNode*		CompileAst();
	void			Visit(Vis& v) override;
};



}


#endif
