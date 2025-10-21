#include "Vfs.h"

NAMESPACE_UPP

Endpoint::Endpoint() : n(0) {
	
}

Endpoint::Endpoint(AstNode& n) : n(&n), rel_loc(n.loc) {
	
}

Endpoint::Endpoint(AstNode* n) : n(n) {
	if (n)
		rel_loc = n->loc;
}

void Endpoint::operator=(const Endpoint& ep) {
	n = ep.n;
	rel_loc = ep.rel_loc;
}

String Endpoint::ToString() const {
	String s;
	if (n)
		s << GetCodeCursorString(n->src) << ", "
		  << n->val.GetPath() << ", "
		  << rel_loc.ToString();
	return s;
}





AstNode::AstNode(VfsValue& v) : VfsValueExt(v) {
	
}

void AstNode::CopyFrom(EonStd* e, const AstNode& n) {
	val.sub.Clear();
	
	val.id = n.val.id;
	src = n.src;
	filter = n.filter;
	i64 = n.i64;
	str = n.str;
	loc = n.loc;
	
	prev = &n;
	
	if (e) {
		type = n.type ? e->FindStackWithPrev(n.type) : 0;
		rval = n.rval ? e->FindStackWithPrev(n.rval) : 0;
		ctx_next = n.ctx_next ? e->FindStackWithPrev(n.ctx_next) : 0;
		ASSERT(type != this);
		ASSERT(rval != this);
		ASSERT(ctx_next != this);
	
		for(int i = 0; i < AstNode::ARG_COUNT; i++) {
			arg[i] = n.arg[i] ? e->FindStackWithPrev(n.arg[i]) : 0;
			ASSERT(arg[i] != this);
		}
	}
	
}

void AstNode::CopyFromValue(const FileLocation& loc, const Value& o) {
	val.sub.Clear();
	val.id.Clear();
	src = Cursor_Literal;
	filter = Cursor_Null;
	i64 = 0;
	str.Clear();
	this->loc = loc;
	
	dword type = o.GetType();
	ASSERT(type);
	
	if (type == BOOL_V) {
		i64 = o.Get<bool>();
		src = Cursor_Literal_BOOL;
	}
	else if (type == INT_V) {
		i64 = o.Get<int>();
		src = Cursor_Literal_INT32;
	}
	else if (type == INT64_V) {
		i64 = o.Get<int64>();
		src = Cursor_Literal_INT64;
	}
	else if (type == STRING_V) {
		str = o.Get<String>();
		src = Cursor_Literal_STRING;
	}
	else if (type == DOUBLE_V) {
		dbl = o.Get<double>();
		src = Cursor_Literal_DOUBLE;
	}
	else TODO
}

void AstNode::CopyToValue(Value& n) const {
	switch (src) {
		case Cursor_Literal_BOOL: n = (bool)i64; break;
		case Cursor_Literal_INT32: n = (int)i64; break;
		case Cursor_Literal_INT64: n = (int64)i64; break;
		case Cursor_Literal_STRING: n = str; break;
		case Cursor_Literal_DOUBLE: n = dbl; break;
		default: TODO break;
	}
}

AstNode& AstNode::Add(const FileLocation& loc, String name, int idx) {
	ASSERT(!locked);
	VfsValue& n = idx >= 0 ? val.Insert(idx, name) : val.Add(name);
	AstNode& s = n.CreateExt<AstNode>();
	s.loc = loc;
	return s;
}

AstNode& AstNode::GetAdd(const FileLocation& loc, String name) {
	ASSERT(name.GetCount());
	AstNode* p = Find(name);
	if (p)
		return *p;
	else
		return Add(loc, name);
}

AstNode& AstNode::GetAdd(const FileLocation& loc, CodeCursor accepts) {
	ASSERT(val.id.GetCount());
	for (AstNode& s : val.Sub<AstNode>()) {
		if (s.IsPartially(accepts))
			return s;
	}
	AstNode& s = Add(loc);
	s.src = accepts;
	if (accepts == Cursor_TypePointer)
		s.val.id = "#";
	else if (accepts == Cursor_TypeLref)
		s.val.id = "&";
	
	return s;
}

AstNode* AstNode::Find(String name, CodeCursor accepts) {
	ASSERT(name.GetCount());
	for (auto& s : val.Sub<AstNode>())
		if (s.val.id == name && (accepts == Cursor_Null || s.IsPartially(accepts)))
			return &s;
	return 0;
}

const AstNode* AstNode::Find(String name, CodeCursor accepts) const {
	ASSERT(name.GetCount());
	for (auto& s : val.Sub<AstNode>())
		if (s.val.id == name && (accepts == Cursor_Null || s.IsPartially(accepts)))
			return &s;
	return 0;
}

AstNode* AstNode::Find(CodeCursor t) {
	for (auto& s : val.Sub<AstNode>())
		if (IsPartially(s.src, t))
			return &s;
	return 0;
}

AstNode* AstNode::FindPartial(CodeCursor t) {
	for (auto& s : val.Sub<AstNode>())
		if ((int64)s.src & (int64)t)
			return &s;
	return 0;
}

const AstNode* AstNode::Find(CodeCursor t) const {
	for (auto& s : val.Sub<AstNode>())
		if (IsPartially(s.src, t))
			return &s;
	return 0;
}

AstNode* AstNode::FindWithPrevDeep(const AstNode* prev) {
	if (this->prev == prev)
		return this;
	for (AstNode& s : val.Sub<AstNode>()) {
		if (s.prev == prev)
			return &s;
	}
	for (AstNode& s : val.Sub<AstNode>()) {
		AstNode* r = s.FindWithPrevDeep(prev);
		if (r)
			return r;
	}
	return 0;
}

void AstNode::FindAll(Vector<Endpoint>& ptrs, CodeCursor accepts, const FileLocation* rel_loc) {
	if (IsPartially(accepts)) {
		Endpoint& p = ptrs.Add();
		p.n = this;
		if (rel_loc)
			p.rel_loc = *rel_loc;
		else
			p.rel_loc = loc;
	}
	for (AstNode& s : val.Sub<AstNode>()) {
		if (s.src == Cursor_SymlinkStmt && !rel_loc)
			s.FindAll(ptrs, accepts, &loc);
		else
			s.FindAll(ptrs, accepts, rel_loc);
	}
}

void AstNode::FindAllStmt(Vector<Endpoint>& ptrs, CodeCursor accepts, const FileLocation* rel_loc) {
	if (IsPartially(src, Cursor_Stmt) && IsPartially(src, accepts)) {
		Endpoint& p = ptrs.Add();
		p.n = this;
		if (rel_loc)
			p.rel_loc = *rel_loc;
		else
			p.rel_loc = loc;
	}
	for (AstNode& s : val.Sub<AstNode>()) {
		if (s.src == Cursor_SymlinkStmt && !rel_loc)
			s.FindAllStmt(ptrs, accepts, &loc);
		else
			s.FindAllStmt(ptrs, accepts, rel_loc);
	}
}

void AstNode::FindAllNonIdEndpoints(Vector<Endpoint>& ptrs, CodeCursor accepts, const FileLocation* rel_loc) {
	for (AstNode& s : val.Sub<AstNode>()) {
		if (s.src == Cursor_SymlinkStmt && !rel_loc)
			s.FindAllNonIdEndpoints0(ptrs, accepts, &loc);
		else
			s.FindAllNonIdEndpoints0(ptrs, accepts, rel_loc);
	}
	if (val.Sub<AstNode>().IsEmpty() && src != Cursor_NamePart) {
		Endpoint& p = ptrs.Add();
		p.n = this;
		if (rel_loc)
			p.rel_loc = *rel_loc;
		else
			p.rel_loc = loc;
	}
}

void AstNode::FindAllNonIdEndpoints0(Vector<Endpoint>& ptrs, CodeCursor accepts, const FileLocation* rel_loc) {
	if (src == Cursor_NamePart) {
		for (AstNode& s : val.Sub<AstNode>()) {
			if (s.src == Cursor_SymlinkStmt && !rel_loc)
				s.FindAllNonIdEndpoints0(ptrs, accepts, &loc);
			else
				s.FindAllNonIdEndpoints0(ptrs, accepts, rel_loc);
		}
	}
	else if (accepts == Cursor_Null || IsPartially(accepts)) {
		Endpoint& p = ptrs.Add();
		p.n = this;
		if (rel_loc)
			p.rel_loc = *rel_loc;
		else
			p.rel_loc = loc;
	}
}

void AstNode::FindAllNonIdEndpoints2(Vector<Endpoint>& ptrs, CodeCursor accepts1, CodeCursor accepts2, const FileLocation* rel_loc) {
	ASSERT(accepts1 != Cursor_Null);
	ASSERT(accepts2 != Cursor_Null);
	for (AstNode& s : val.Sub<AstNode>()) {
		if (s.src == Cursor_SymlinkStmt && !rel_loc)
			s.FindAllNonIdEndpoints20(ptrs, accepts1, accepts2, &loc);
		else
			s.FindAllNonIdEndpoints20(ptrs, accepts1, accepts2, rel_loc);
	}
	if (val.Sub<AstNode>().IsEmpty() && src != Cursor_NamePart) {
		Endpoint& p = ptrs.Add();
		p.n = this;
		if (rel_loc)
			p.rel_loc = *rel_loc;
		else
			p.rel_loc = loc;
	}
}

void AstNode::FindAllNonIdEndpoints20(Vector<Endpoint>& ptrs, CodeCursor accepts1, CodeCursor accepts2, const FileLocation* rel_loc) {
	ASSERT(accepts1 != Cursor_Null);
	ASSERT(accepts2 != Cursor_Null);
	if (src == Cursor_NamePart) {
		for (AstNode& s : val.Sub<AstNode>()) {
			if (s.src == Cursor_SymlinkStmt && !rel_loc)
				s.FindAllNonIdEndpoints20(ptrs, accepts1, accepts2, &loc);
			else
				s.FindAllNonIdEndpoints20(ptrs, accepts1, accepts2, rel_loc);
		}
	}
	else if (IsPartially(accepts1) || IsPartially(accepts2)) {
		Endpoint& p = ptrs.Add();
		p.n = this;
		if (rel_loc)
			p.rel_loc = *rel_loc;
		else
			p.rel_loc = loc;
	}
}

String AstNode::GetConstantString() const {
	String s = GetCodeCursorString(src) + ": ";
	switch (src) {
		case Cursor_Literal_INT32:	s += IntStr((int)i64);	break;
		case Cursor_Literal_INT64:	s += IntStr64(i64);		break;
		case Cursor_Literal_DOUBLE:	s += DblStr(dbl);		break;
		case Cursor_Literal_STRING:	s += "\"" + str + "\"";	break;
		default: break;
	}
	return s;
}

String AstNode::GetTreeItemString() const {
	String s;
	s << GetCodeCursorString(src) << ": ";
	
	if (val.id.GetCount())
		s << val.id;
	else if (src == Cursor_Object)
		s << "object(" << obj.ToString() << ")";
	else if (src == Cursor_Unresolved)
		s << str;
	else if (IsPartially(src, Cursor_Literal))
		s << "const(" << GetConstantString() << ")";
	else if (IsPartially(src, Cursor_Op))
		s << "op(" << GetOpCodeString(src) << ")";
	else if (filter != Cursor_Null)
		s << "filter(" << GetCodeCursorString(filter) << ")";
	else if (src == Cursor_Rval && rval)
		s << rval->GetName();
	
	return s;
}

String AstNode::GetTreeString(int indent) const {
	String s;
	s.Cat('\t', indent);
	s.Cat(GetTreeItemString());
	s.Cat('\n');
	for (const AstNode& n : val.Sub<AstNode>()) {
		s << n.GetTreeString(indent+1);
	}
	return s;
}

String AstNode::GetTreeString(int indent, bool links) const {
	String s = GetTreeString(indent);
	if (!links) return s;
	for(int i = 0; i < ARG_COUNT; i++) {
		if (arg[i]) {
			s.Cat('\t', indent+1);
			s << "arg:\n";
			s << arg[i]->GetTreeString(indent+2);
		}
	}
	if (rval) {
		s.Cat('\t', indent+1);
		s << "rval:\n";
		s << rval->GetTreeString(indent+2);
	}
	if (ctx_next) {
		s.Cat('\t', indent+1);
		s << "ctx_next:\n";
		s << ctx_next->GetTreeString(indent+2);
	}
	
	return s;
}

String AstNode::GetCodeString(const CodeArgs2& args) const {
	TODO return String();
}

String AstNode::ToString() const {
	return GetTreeItemString();
}

#if 0
String AstNode::GetPath() const {
	static const int MAX_PATH_LEN = 32;
	const AstNode* path[MAX_PATH_LEN];
	const AstNode* cur = this;
	int count = 0;
	while (cur) {
		path[count] = cur;
		cur = cur->GetSubOwner();
		count++;
	}
	
	String s;
	for(int i = count-1, j = 0; i >= 0; i--, j++) {
		if (j) s += ".";
		s += path[i]->GetName();
	}
	return s;
}
#endif

String AstNode::GetPartStringArray() const {
	static const int MAX_PATH_LEN = 32;
	const AstNode* path[MAX_PATH_LEN];
	const AstNode* cur = this;
	int count = 0;
	while (cur) {
		if (cur->IsPartially(Cursor_ClassPath) ||
			cur->IsPartially(Cursor_TypeDecl) ||
			cur->IsPartially(Cursor_ValueDecl) ||
			cur->IsPartially(Cursor_MetaTypeDecl) ||
			cur->IsPartially(Cursor_MetaValueDecl))
			path[count++] = cur;
		cur = cur->val.owner ? cur->val.owner->FindExt<AstNode>() : 0;
	}
	
	String s;
	s.Cat('[');
	for(int i = count-1, j = 0; i >= 0; i--, j++) {
		if (j) s += ", \"";
		else s.Cat('\"');
		s.Cat(path[i]->GetName());
		s.Cat('\"');
	}
	s.Cat(']');
	return s;
}

bool AstNode::IsPartially(CodeCursor t) const {
	return ::Upp::IsPartially(src, t);
}

bool AstNode::IsPartially(CodeCursor src, CodeCursor t) const {
	return ::Upp::IsPartially(src, t);
}

bool IsPartially(CodeCursor src, CodeCursor t) {
	if (src == t)
		return true;
	switch (t) {
		case Cursor_ValueDecl:
		switch (src) {
			case Cursor_VarDecl:
			case Cursor_ParmDecl:
			case Cursor_Literal:
			return true;
			default: return false;
		}
		
		case Cursor_TypeDecl:
		switch (src) {
			case Cursor_Builtin:
			case Cursor_TypedefDecl:
			case Cursor_ClassDecl:
			case Cursor_ClassTemplate:
			case Cursor_TypePointer:
			case Cursor_TypeLref:
			return true;
			default: return false;
		}
		
		case Cursor_Function:
		switch (src) {
			case Cursor_StaticFunction:
			case Cursor_CXXMethod:
			case Cursor_FunctionBuiltin:
			return true;
			default: return false;
		}
		
		case Cursor_Undefined:
		switch (src) {
			case Cursor_Null:
			case Cursor_NamePart:
			return true;
			default: return false;
		}
		
		case Cursor_ClassPath_ParmDecl:
		switch (src) {
			case Cursor_ParmDecl:
			case Cursor_NamePart:
			return true;
			default: return false;
		}
		
		case Cursor_ClassPath_VarDecl:
		switch (src) {
			case Cursor_VarDecl:
			case Cursor_NamePart:
			return true;
			default: return false;
		}
		
		case Cursor_ClassPath:
		switch (src) {
			case Cursor_ClassPath_ParmDecl:
			case Cursor_ClassPath_VarDecl:
			case Cursor_Namespace:
			case Cursor_Function:
			return true;
			default: return false;
		}
		
		case Cursor_Compounding:
		switch (src) {
			case Cursor_TranslationUnit:
			case Cursor_Namespace:
			case Cursor_CompoundStmt:
			return true;
			default: return false;
		}
		
		case Cursor_WithRvalReturn:
		if (IsPartially(src, Cursor_ExprOp) ||
			IsPartially(src, Cursor_Literal))
			return true;
		switch (src) {
			case Cursor_Rval:
			case Cursor_Resolve:
			case Cursor_ArgumentList:
			case Cursor_Ctor:
			case Cursor_Object:
			return true;
			default: return false;
		}
		
		case Cursor_EcsStmt:
		switch (src) {
			case Cursor_EngineStmt:
			case Cursor_WorldStmt:
			case Cursor_EntityStmt:
			case Cursor_ComponentStmt:
			case Cursor_SystemStmt:
			case Cursor_PoolStmt:
			return true;
			default: return false;
		}
		
		case Cursor_OldEcsStmt:
		switch (src) {
			case Cursor_MachineDecl:
			case Cursor_MachineStmt:
			case Cursor_ChainDecl:
			case Cursor_ChainStmt:
			case Cursor_LoopDecl:
			case Cursor_DriverStmt:
			case Cursor_LoopStmt:
			case Cursor_StateStmt:
			case Cursor_AtomStmt:
			return true;
			default: return false;
		}
		
		case Cursor_MetaValueDecl:
		switch (src) {
			case Cursor_MetaVariable:
			case Cursor_MetaParameter:
			return true;
			default: return false;
		}
		
		case Cursor_MetaTypeDecl:
		switch (src) {
			case Cursor_MetaBuiltin:
			return true;
			default: return false;
		}
		
		case Cursor_MetaFunction:
		switch (src) {
			case Cursor_MetaStaticFunction:
			//case Cursor_MetaFunction_METHOD:
			//case Cursor_MetaFunction_BUILTIN:
			return true;
			default: return false;
		}
		
		case Cursor_ClassPath_MetaParam:
		switch (src) {
			case Cursor_MetaParameter:
			case Cursor_NamePart:
			return true;
			default: return false;
		}
		
		case Cursor_ClassPath_MetaVar:
		switch (src) {
			case Cursor_MetaVariable:
			case Cursor_NamePart:
			return true;
			default: return false;
		}
		
		case Cursor_MetaDecl:
		switch (src) {
			case Cursor_NamePart:
			case Cursor_MetaValueDecl:
			case Cursor_MetaTypeDecl:
			case Cursor_MetaFunction:
			case Cursor_MetaRval:
			case Cursor_MetaCtor:
			case Cursor_MetaResolve:
			return true;
			default: return false;
		}
		
		case Cursor_ClassPath_MetaDecl:
		switch (src) {
			case Cursor_ClassPath_MetaParam:
			case Cursor_ClassPath_MetaVar:
			case Cursor_MetaFunction:
			case Cursor_MetaClass:
			return true;
			default: return false;
		}
		
		
		case Cursor_MetaStmt:
		switch (src) {
			case Cursor_MetaIfStmt:
			case Cursor_MetaElseStmt:
			case Cursor_MetaDoStmt:
			case Cursor_MetaWhileStmt:
			case Cursor_MetaForStmt:
			case Cursor_MetaForStmt_Conditional:
			case Cursor_MetaForStmt_Post:
			case Cursor_MetaForStmt_Range:
			case Cursor_MetaBreakStmt:
			case Cursor_MetaContinueStmt:
			case Cursor_MetaCaseStmt:
			case Cursor_MetaDefaultStmt:
			case Cursor_MetaReturnStmt:
			case Cursor_MetaSwitchStmt:
			case Cursor_MetaBlockStmt:
			case Cursor_MetaExprStmt:
			return true;
			default: return false;
		}
		
		case Cursor_Stmt:
		switch (src) {
			case Cursor_Null:
			case Cursor_IfStmt:
			case Cursor_ElseStmt:
			case Cursor_DoStmt:
			case Cursor_WhileStmt:
			case Cursor_ForStmt:
			case Cursor_ForStmt_Conditional:
			case Cursor_ForStmt_PostOp:
			case Cursor_ForStmt_Range:
			case Cursor_BreakStmt:
			case Cursor_ContinueStmt:
			case Cursor_CaseStmt:
			case Cursor_DefaultStmt:
			case Cursor_ReturnStmt:
			case Cursor_SwitchStmt:
			case Cursor_BlockExpr:
			case Cursor_AtomConnectorStmt:
			case Cursor_CtorStmt:
			case Cursor_ExprStmt:
			case Cursor_MetaIfStmt:
			case Cursor_MetaElseStmt:
			case Cursor_MetaDoStmt:
			case Cursor_MetaWhileStmt:
			case Cursor_MetaForStmt:
			case Cursor_MetaForStmt_Conditional:
			case Cursor_MetaForStmt_Post:
			case Cursor_MetaForStmt_Range:
			case Cursor_MetaBreakStmt:
			case Cursor_MetaContinueStmt:
			case Cursor_MetaCaseStmt:
			case Cursor_MetaDefaultStmt:
			case Cursor_MetaReturnStmt:
			case Cursor_MetaSwitchStmt:
			case Cursor_MetaBlockStmt:
			case Cursor_MetaExprStmt:
			return true;
			default: return false;
		}
		
		case Cursor_Op:
		switch (src) {
			case Cursor_Op_NULL:
			case Cursor_Op_INC:
			case Cursor_Op_DEC:
			case Cursor_Op_POSTINC:
			case Cursor_Op_POSTDEC:
			case Cursor_Op_NEGATIVE:
			case Cursor_Op_POSITIVE:
			case Cursor_Op_NOT:
			case Cursor_Op_NEGATE:
			case Cursor_Op_ADD:
			case Cursor_Op_SUB:
			case Cursor_Op_MUL:
			case Cursor_Op_DIV:
			case Cursor_Op_MOD:
			case Cursor_Op_LSH:
			case Cursor_Op_RSH:
			case Cursor_Op_GREQ:
			case Cursor_Op_LSEQ:
			case Cursor_Op_GREATER:
			case Cursor_Op_LESS:
			case Cursor_Op_EQ:
			case Cursor_Op_INEQ:
			case Cursor_Op_BWAND:
			case Cursor_Op_BWXOR:
			case Cursor_Op_BWOR:
			case Cursor_Op_AND:
			case Cursor_Op_OR:
			case Cursor_Op_COND:
			case Cursor_Op_ASSIGN:
			case Cursor_Op_ADDASS:
			case Cursor_Op_SUBASS:
			case Cursor_Op_MULASS:
			case Cursor_Op_DIVASS:
			case Cursor_Op_MODASS:
			case Cursor_Op_CALL:
			case Cursor_Op_SUBSCRIPT:
			return true;
			default: return false;
		}
		
		case Cursor_Literal:
		switch (src) {
			case Cursor_Literal_BOOL:
			case Cursor_Literal_INT32:
			case Cursor_Literal_INT64:
			case Cursor_Literal_DOUBLE:
			case Cursor_Literal_STRING:
			return true;
			default: return false;
		}
		
		case Cursor_ExprCastable:
		switch (src) {
			case Cursor_VarDecl:
			case Cursor_ParmDecl:
			case Cursor_Literal:
			case Cursor_Object:
			case Cursor_Resolve:
			case Cursor_Rval:
			case Cursor_ArgumentList:
			case Cursor_Function:
			//case Cursor_Op:
			return true;
			default: return false;
		}
		
		case Cursor_ExprOp:
		return	IsPartially(src, Cursor_ExprCastable) ||
				IsPartially(src, Cursor_Op);
		
		default:
		return false;
	}
}


Value EvaluateAstNodeValue(AstNode& n) {
	Value o;
	if (IsPartially(n.src, Cursor_Op)) {
		switch (n.src) {
			
		case Cursor_Op_POSITIVE:
			o = EvaluateAstNodeValue(*n.arg[0]);
			break;
			
		case Cursor_Op_NEGATIVE:
			o = EvaluateAstNodeValue(*n.arg[0]);
			o = -1 * (double)o;
			break;
			
		default:
			TODO
		}
	}
	else if (IsPartially(n.src, Cursor_Literal)) {
		n.CopyToValue(o);
	}
	else TODO
	return o;
}

END_UPP_NAMESPACE
