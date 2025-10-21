#include "Script.h"

NAMESPACE_UPP

namespace Eon {


AtomBuilder& AtomBuilder::Assign(String id, Value val) {
	assigns.Add(id, val);
	return *this;
}

AstNode& AtomBuilder::CreateNode(AstNode& root) {
	ASSERT(id.parts.GetCount());
	FileLocation loc; // empty
	AstNode* cont = &root;
	if (cont->src == Cursor_LoopStmt) {
		cont = &cont->GetAdd(loc, Cursor_CompoundStmt);
	}
	for (auto& p : id.parts) {
		AstNode& n = cont->val.GetAdd<AstNode>(p);
		cont = &n;
		n.src = Cursor_NamePart;
	}
	cont->src = Cursor_AtomStmt;
	if (!assigns.IsEmpty()) {
		AstNode* expr = &cont->GetAdd(loc, Cursor_CompoundStmt);
		for(int i = 0; i < assigns.GetCount(); i++) {
			AstNode* exprstmt = &expr->Add(loc);
			exprstmt->src = Cursor_ExprStmt;
			
			AstNode& name = exprstmt->val.GetAdd<AstNode>(assigns.GetKey(i));
			name.src = Cursor_Unresolved;
			name.str = assigns.GetKey(i);
			
			AstNode& val = name.val.GetAdd<AstNode>();
			
			Value v = assigns[i];
			if (v.GetType() == INT64_V) {
				val.src = Cursor_Literal_INT64;
				val.i64 = (int64)v;
			}
			else if (v.GetType() == INT_V) {
				val.src = Cursor_Literal_INT32;
				val.i64 = (int)v;
			}
			else if (v.GetType() == DOUBLE_V) {
				val.src = Cursor_Literal_DOUBLE;
				val.dbl = (double)v;
			}
			else if (v.GetType() == STRING_V) {
				val.src = Cursor_Literal_STRING;
				val.str = v.ToString();
			}
			else {
				throw Exc("Can't assign value: " + v.ToString());
			}
			
			AstNode& assign = exprstmt->val.GetAdd<AstNode>();
			assign.src = Cursor_Op_ASSIGN;
			assign.arg[0] = &name;
			assign.arg[1] = &val;
			exprstmt->rval = &assign;
		}
	}
	return *cont;
}

AtomBuilder& LoopBuilder::AddAtom(String id) {
	auto& atom = atoms.Add();
	atom.id.Parse(id);
	return atom;
}

AstNode& LoopBuilder::CreateNode(AstNode& root) {
	ASSERT(id.parts.GetCount());
	AstNode* cont = &root;
	for (auto& p : id.parts) {
		AstNode& n = cont->val.GetAdd<AstNode>(p);
		cont = &n;
		n.src = Cursor_NamePart;
	}
	cont->src = Cursor_LoopStmt;
	for(int i = 0; i < atoms.GetCount(); i++) {
		atoms[i].CreateNode(*cont);
	}
	return *cont;
}

LoopBuilder& Builder::AddLoop(String id) {
	auto& loop = loops.Add();
	loop.id.Parse(id);
	return loop;
}

AstNode* Builder::CompileAst() {
	int i = val.Find("root");
	if (i >= 0)
		val.Remove(i);
	
	AstNode& root = val.GetAdd<AstNode>("root");
	for (auto& loop : loops) {
		AstNode& n = loop.CreateNode(root);
		
	}
	
	return &root;
}

void Builder::Visit(Vis& v) {
	
}


}

END_UPP_NAMESPACE
