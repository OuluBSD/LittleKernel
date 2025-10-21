#include "Vfs.h"

NAMESPACE_UPP


String EonStd::GetPathString() const {
	String s;
	for (const Scope& scope : spath) {
		if (!s.IsEmpty())
			s.Cat('.');
		s << scope.n->GetName();
	}
	return s;
}

AstNode* EonStd::AddBuiltinType(String name) {
	AstNode& root = GetRoot();
	AstNode& sn = root.Add(FileLocation(), name);
	sn.src = Cursor_Builtin;
	return &sn;
}

AstNode* EonStd::AddMetaBuiltinType(String name) {
	AstNode& root = GetRoot();
	AstNode& sn = root.Add(FileLocation(), name);
	sn.src = Cursor_MetaBuiltin;
	return &sn;
}

String EonStd::GetRelativePartStringArray(const AstNode& n) const {
	ASSERT(!spath.IsEmpty());
	AstNode* top = spath.Top().n;
	AstNode* root = spath[0].n;
	ASSERT(top != &n);
	
	const AstNode* nodes[PathIdentifier::MAX_PARTS];
	int node_count = 0;
	const AstNode* iter = &n;
	while (iter && iter != top && iter != root) {
		bool found = false;
		for (const Scope& s : spath) {
			if (s.n == iter) {
				found = true;
				break;
			}
		}
		if (found)
			break;
		
		if (iter->IsPartially(Cursor_ClassPath) || iter->IsPartially(Cursor_ClassPath_MetaDecl))
			nodes[node_count++] = iter;
		iter = iter->val.owner ? iter->val.owner->FindExt<AstNode>() : 0;
	}
	
	String s = "[";
	for(int i = 0; i < node_count; i++) {
		if (i) s.Cat(',');
		s.Cat('\"');
		const AstNode& node = *nodes[node_count-1-i];
		if (node.IsPartially(Cursor_MetaDecl))
			s.Cat('$');
		const String& n = node.val.id;
		ASSERT(n.GetCount());
		s.Cat(n);
		s.Cat('\"');
	}
	s.Cat(']');
	return s;
}

void EonStd::InitDefault(bool add_root) {
	builtin_void =			AddBuiltinType("void");
	#if 0
	builtin_int =			AddBuiltinType("int");
	builtin_long =			AddBuiltinType("long");
	builtin_uint =			AddBuiltinType("uint");
	builtin_ulong =			AddBuiltinType("ulong");
	builtin_float =			AddBuiltinType("float");
	builtin_double =		AddBuiltinType("double");
	builtin_byte =			AddBuiltinType("byte");
	builtin_char =			AddBuiltinType("char");
	builtin_short =			AddBuiltinType("short");
	builtin_ushort =		AddBuiltinType("ushort");
	builtin_cstring =		AddBuiltinType("cstring");
	
	meta_builtin_void =			AddMetaBuiltinType("void");
	meta_builtin_int =			AddMetaBuiltinType("int");
	meta_builtin_double =		AddMetaBuiltinType("double");
	meta_builtin_cstring =		AddMetaBuiltinType("cstring");
	meta_builtin_stmt =			AddMetaBuiltinType("stmt");
	
	meta_builtin_machstmt =		AddMetaBuiltinType("machstmt");
	meta_builtin_chainstmt =	AddMetaBuiltinType("chainstmt");
	
	meta_builtin_atomstmt =		AddMetaBuiltinType("atomstmt");
	meta_builtin_worldstmt =	AddMetaBuiltinType("worldstmt");
	meta_builtin_systemstmt =	AddMetaBuiltinType("systemstmt");
	meta_builtin_poolstmt =		AddMetaBuiltinType("poolstmt");
	meta_builtin_entitystmt =	AddMetaBuiltinType("entitystmt");
	meta_builtin_compstmt =		AddMetaBuiltinType("compstmt");
	meta_builtin_params =		AddMetaBuiltinType("params");
	#endif
	meta_builtin_expr =			AddMetaBuiltinType("expr");
	meta_builtin_loopstmt =		AddMetaBuiltinType("loopstmt");
	
	{
		AstNode& logger = GetRoot().Add(FileLocation(), "LOG");
		logger.src = Cursor_FunctionBuiltin;
	}
	
	GetRoot().src = Cursor_TranslationUnit;
	
	if (add_root)
		spath.Add().Set(&GetRoot(),true);
}

bool EonStd::ForwardUserspace(AstNode*& n) {
	if (!n) return false;
	
	if (n->IsPartially(Cursor_Function)) {
		n = n->Find(Cursor_CompoundStmt);
		return n != NULL;
	}
	if (n->IsPartially(Cursor_Stmt)) {
		switch (n->src) {
			case Cursor_ForStmt:
			case Cursor_IfStmt:
			case Cursor_ElseStmt:
				n = n->Find(Cursor_CompoundStmt);
				break;
			
			case Cursor_ForStmt_Conditional:
			case Cursor_ForStmt_PostOp:
			case Cursor_ForStmt_Range:
			case Cursor_BlockExpr:
			case Cursor_MetaBlockExpr:
			case Cursor_ReturnStmt:
				return false;
				
			case Cursor_DoStmt:
			case Cursor_WhileStmt:
			case Cursor_BreakStmt:
			case Cursor_ContinueStmt:
			case Cursor_CaseStmt:
			case Cursor_DefaultStmt:
			case Cursor_SwitchStmt:
				TODO
				
			default:
			case Cursor_Null:
				break;
		}
	}
	return false;
}

AstNode* EonStd::FindDeclaration(const PathIdentifier& id, CodeCursor accepts) {
	if (id.part_count == 0)
		return 0;
	
	for (int i = spath.GetCount()-1; i >= 0; i--) {
		Scope& s = spath[i];
			
		AstNode* n = GetDeclaration(s.n, id, accepts);
		if (n)
			return n;
	}
	return 0;
}

AstNode* EonStd::FindDeclaration(const Vector<String>& id, CodeCursor accepts) {
	if (id.IsEmpty())
		return 0;
	
	for (int i = spath.GetCount()-1; i >= 0; i--) {
		Scope& s = spath[i];
			
		AstNode* n = GetDeclaration(s.n, id, accepts);
		if (n)
			return n;
	}
	return 0;
}

AstNode* EonStd::GetDeclaration(const PathIdentifier& id, CodeCursor accepts) {
	if (id.part_count == 0 || spath.IsEmpty())
		return 0;
	
	Scope& s = spath[0];
	return GetDeclaration(s.n, id, accepts);
}

AstNode* EonStd::GetDeclaration(AstNode* owner, const PathIdentifier& id, CodeCursor accepts) {
	AstNode* cur = owner;
	AstNode* next = 0;
	AstNode* prev = 0;
	for(int i = 0; i < id.part_count; i++) {
		bool last = i == id.part_count-1;
		next = 0;
		for (int tries = 0; tries < 100; tries++) {
			const Token* t = id.parts[i];
			if ((t->IsType(TK_ID) || t->IsType(TK_INTEGER)) && !t->str_value.IsEmpty()) {
				if (id.is_meta[i]) {
					CodeCursor a = last ? (accepts & Cursor_MetaDecl ? accepts : Cursor_MetaDecl) : Cursor_MetaDecl;
					next = cur->Find(t->str_value, a);
				}
				else {
					CodeCursor a = last ? accepts : Cursor_Null;
					next = cur->Find(t->str_value, a);
				}
			}
			else if (t->IsType('#')) {
				next = cur;
				// pass, next = &cur->GetAdd(Cursor_TypePointer);
			}
			else if (t->IsType('&')) {
				next = cur;
				// pass, next = &cur->GetAdd(Cursor_TypeLref);
			}
			else {
				TODO
			}
			
			if (!next) {
				if (ForwardUserspace(cur)) {
					ASSERT(cur);
					continue;
				}
				else
					return 0;
			}
			else break;
		}
		
		ASSERT(next);
		prev = cur;
		cur = next;
	}
	
	if (id.head_count > 0 && id.tail_count > 0) {
		return 0;
	}
	
	if (cur && id.head_count > 0) {
		TODO
	}
	else if (cur && id.tail_count > 0) {
		if (cur->IsPartially(Cursor_TypeDecl)) {
			for(int i = 0; i < id.tail_count; i++) {
				switch (id.tail[i]) {
				case PathIdentifier::PTR:
					cur = &cur->GetAdd(id.end->loc, Cursor_TypePointer);
					break;
				
				case PathIdentifier::LREF:
					cur = &cur->GetAdd(id.end->loc, Cursor_TypeLref);
					break;
				
				default:
					return 0;
				}
			}
		}
		else {
			TODO
		}
	}
	
	CodeCursor a = id.is_meta[id.part_count-1] ? Cursor_MetaDecl : accepts;
	
	if (cur && accepts == Cursor_Null || cur->IsPartially(a))
		return cur;
	
	return 0;
}

AstNode* EonStd::GetDeclaration(AstNode* owner, const Vector<String>& id, CodeCursor accepts) {
	AstNode* cur = owner;
	AstNode* next = 0;
	AstNode* prev = 0;
	
	for(int i = 0; i < id.GetCount(); i++) {
		bool last = i == id.GetCount()-1;
		next = 0;
		String name = id[i];
		for (int tries = 0; tries < 100; tries++) {
			CodeCursor a = last ? accepts : Cursor_Null;
			next = cur->Find(name, a);
			
			if (!next) {
				if (ForwardUserspace(cur)) {
					ASSERT(cur);
					continue;
				}
				else
					return 0;
			}
			else break;
		}
		
		ASSERT(next);
		prev = cur;
		cur = next;
	}
	
	CodeCursor a = accepts;
	
	if (cur && accepts == Cursor_Null || cur->IsPartially(a))
		return cur;
	
	return 0;
}

AstNode& EonStd::Declare(AstNode& owner, const PathIdentifier& id, bool insert_before) {
	AstNode* cur = &owner;
	for(int i = 0; i < id.part_count; i++) {
		const Token* t = id.parts[i];
		if (t->IsType(TK_ID) || t->IsType(TK_INTEGER)) {
			String id = t->str_value;
			ASSERT(id.GetCount());
			if (insert_before) {
				AstNode* next = cur->Find(id);
				if (next)
					cur = next;
				else
					cur = &cur->Add(t->loc, id, max(0, cur->val.Sub<TokenNode>().GetCount()-2));
			}
			else cur = &cur->GetAdd(t->loc, id);
			if (cur->src == Cursor_Null)
				cur->src = Cursor_NamePart;
		}
		else {
			TODO
		}
	}
	
	return *cur;
}

AstNode& EonStd::DeclareRelative(const PathIdentifier& id) {
	ASSERT(id.part_count > 0);
	return Declare(*spath.Top().n, id);
}

AstNode* EonStd::GetClosestType(bool skip_locked) {
	for(int i = spath.GetCount()-1; i >= 0; i--) {
		Scope& scope = spath[i];
		if (skip_locked && scope.n->locked)
			continue;
		if (scope.n->IsPartially(Cursor_Function))
			return 0;
		if (scope.n->type)
			return scope.n->type;
	}
	return 0;
}
AstNode& EonStd::GetBlock() {
	for(int i = spath.GetCount()-1; i >= 0; i--) {
		Scope& scope = spath[i];
		if (scope.n->locked)
			continue;
		if (scope.n->IsPartially(Cursor_Compounding))
			return *scope.n;
	}
	return GetRoot();
}


AstNode& EonStd::GetNonLockedOwner() {
	for(int i = spath.GetCount()-1; i >= 0; i--) {
		Scope& scope = spath[i];
		if (scope.n->locked)
			continue;
		return *scope.n;
	}
	return GetRoot();
}

void EonStd::PushScope(AstNode& n, bool non_continuous) {
	if (spath.IsEmpty() || non_continuous) {
		ASSERT(&n == &GetRoot() || non_continuous);
		Scope& s = spath.Add();
		s.n = &n;
		s.pop_this = true;
	}
	else {
		thread_local static Vector<AstNode*> tmp;
		VfsValue* cur = &spath.Top().n->val;
		
		tmp.SetCount(0);
		VfsValue* iter = &n.val;
		int dbg_i = 0;
		while (iter && iter != cur) {
			AstNode* an = iter->FindExt<AstNode>();
			if (an)
				tmp.Add(an);
			iter = iter->owner;
			dbg_i++;
		}
		ASSERT(iter == cur);
		
		for (int i = tmp.GetCount()-1, j = 0; i >= 0; i--, j++) {
			Scope& s = spath.Add();
			s.n = tmp[i];
			s.pop_this = j == 0;
		}
	}
}

// Returns links to variables in expression statements etc.
// The scope path is not continous, unlike with LVal.
// ~ The scope stack grows from left, and sometimes meet's it's end from the right side. ~
void EonStd::PushScopeRVal(AstNode& n) {
	Scope& s = spath.Add();
	s.n = &n;
	s.pop_this = true;
}

AstNode* EonStd::PopScope() {
	int rm_i = 0;
	for (int i = spath.GetCount()-1; i >= 0; i--) {
		Scope& s = spath[i];
		if (s.pop_this) {
			rm_i = i;
			break;
		}
	}
	int c = spath.GetCount() - rm_i;
	AstNode* ret = 0;
	if (c) ret = spath[rm_i].n;
	spath.Remove(rm_i, c);
	return ret;
}

String EonStd::GetTypeInitValueString(AstNode& n) const {
	if (n.src == Cursor_MetaBuiltin) {
		if (n.val.id == "int")
			return "0";
		TODO
	}
	else {
		TODO
	}
	return String();
}

AstNode* EonStd::FindStackName(String name, CodeCursor accepts) {
	for (int i = spath.GetCount()-1; i >= 0; i--) {
		Scope& s = spath[i];
		for (AstNode& ss : s.n->val.Sub<AstNode>()) {
			if (ss.val.id == name && (accepts == Cursor_Null || ss.IsPartially(accepts)))
				return &ss;
		}
		if (s.n->val.id == name && (accepts == Cursor_Null || s.n->IsPartially(accepts)))
			return s.n;
	}
	return 0;
}

AstNode* EonStd::FindStackName2(String name, CodeCursor accepts1, CodeCursor accepts2) {
	ASSERT(accepts1 != Cursor_Null);
	ASSERT(accepts2 != Cursor_Null);
	for (int i = spath.GetCount()-1; i >= 0; i--) {
		Scope& s = spath[i];
		for (AstNode& ss : s.n->val.Sub<AstNode>()) {
			if (ss.val.id == name && (ss.IsPartially(accepts1) || ss.IsPartially(accepts2)))
				return &ss;
		}
		if (s.n->val.id == name && (s.n->IsPartially(accepts1) || s.n->IsPartially(accepts2)))
			return s.n;
	}
	return 0;
}

AstNode* EonStd::FindStackValue(String name) {
	for (int i = spath.GetCount()-1; i >= 0; i--) {
		Scope& s = spath[i];
		for (AstNode& ss : s.n->val.Sub<AstNode>()) {
			if (ss.val.id == name)
				return &ss;
		}
		if (s.n->val.id == name)
			return s.n;
	}
	return 0;
}

AstNode* EonStd::FindStackWithPrev(const AstNode* prev) {
	for (int i = spath.GetCount()-1; i >= 0; i--) {
		Scope& s = spath[i];
		for (AstNode& ss : s.n->val.Sub<AstNode>()) {
			if (ss.prev == prev)
				return &ss;
		}
		if (s.n->prev == prev)
			return s.n;
	}
	return 0;
}

AstNode* EonStd::FindStackWithPrevDeep(const AstNode* prev) {
	for (int i = spath.GetCount()-1; i >= 0; i--) {
		Scope& s = spath[i];
		const AstNode* iter = prev;
		while (iter) {
			if (s.n->prev == iter) {
				return s.n->FindWithPrevDeep(prev);
			}
			iter = iter->val.owner ? iter->val.owner->FindExt<AstNode>() : 0;
		}
	}
	if (!spath.IsEmpty())
		return spath[0].n->FindWithPrevDeep(prev);
	return 0;
}


END_UPP_NAMESPACE
