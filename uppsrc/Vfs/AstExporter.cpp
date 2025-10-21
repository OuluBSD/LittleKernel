#include "Vfs.h"

NAMESPACE_UPP


AstExporter::AstExporter() :
	ErrorSource("AstExporter")
{
	
}

bool AstExporter::Process(const AstNode& n) {
	output.Clear();
	PushScope(n);
	Visit(n);
	PopScope();
	
	return true;
}

String AstExporter::GetIndentString(int offset) const {
	ASSERT(indent >= 0);
	ASSERT(!inline_mode);
	String s;
	s.Cat('\t', max(0, indent + offset));
	return s;
}

void AstExporter::PushScope(const AstNode& n, bool skip_indent) {
	Scope& scope = scopes.Add();
	scope.pop_this = !n.IsPartially(Cursor_Undefined);
	scope.n = &n;
	scope.skip_indent = skip_indent;
	if (n.src == Cursor_CompoundStmt && !skip_indent)
		++indent;
}

void AstExporter::PopScope() {
	Scope& scope = scopes.Top();
	if (scope.n->src == Cursor_CompoundStmt && !scope.skip_indent) {
		ASSERT(indent > 0);
		--indent;
	}
	scopes.SetCount(scopes.GetCount()-1);
}

void AstExporter::PushInlineScope() {
	InlineScope& is = inline_scopes.Add();
	
}

void AstExporter::PopInlineScope() {
	ASSERT(!inline_scopes.IsEmpty());
	inline_scopes.SetCount(inline_scopes.GetCount()-1);
}

void AstExporter::Visit(const AstNode& n, bool force, bool declare) {
	
	switch (n.src) {
	case Cursor_Builtin:
		if (force) {
			VisitBuiltin(n);
			return;
		}
	case Cursor_FunctionBuiltin:
	case Cursor_MetaBuiltin:
	case Cursor_MetaStaticFunction:
	case Cursor_MetaRval:
	case Cursor_MetaParameter:
	case Cursor_LoopStmt:
	case Cursor_ChainStmt:
	case Cursor_AtomStmt:
	case Cursor_MachineStmt:
	case Cursor_WorldStmt:
	case Cursor_SystemStmt:
	case Cursor_PoolStmt:
	case Cursor_EntityStmt:
	case Cursor_ComponentStmt:
		return;
	
	case Cursor_MetaResolve:
		ASSERT(n.rval);
		if (n.rval)
			Visit(*n.rval);
		return;
		
	case Cursor_TranslationUnit:
	case Cursor_NamePart:
		for (const AstNode& s : n.val.Sub<AstNode>()) {
			if (s.src == Cursor_Rval)
				continue;
			
			CHECK_SCOPES_BEGIN
			
			PushScope(s);
			Visit(s, false, true);
			PopScope();
			
			CHECK_SCOPES_END
		}
		return;
		
	case Cursor_CompoundStmt:
		for (const AstNode& s : n.val.Sub<AstNode>()) {
			if (s.src != Cursor_CompoundStmt &&
				!s.IsPartially(Cursor_MetaDecl))
				continue;
			
			// merge statement blocks which meta created with current block
			if (s.src == Cursor_CompoundStmt && s.i64 == 1) {
				Visit(s, false, true);
				continue;
			}
			
			CHECK_SCOPES_BEGIN
			
			PushScope(s);
			Visit(s, false, true);
			PopScope();
			
			CHECK_SCOPES_END
		}
		return;
	
	case Cursor_StaticFunction:
		VisitFunction(n);
		break;
	
	case Cursor_ParmDecl:
		VisitParameter(n);
		break;
	
	case Cursor_Stmt:
		VisitStatement(n);
		break;
		
	case Cursor_VarDecl:
		VisitVariable(n, declare);
		break;
	
	case Cursor_MetaVariable:
		ASSERT_(0, "meta code should be executed before this, and non-existent here");
		break;
	
	case Cursor_Argument:
		VisitArgument(n);
		break;
		
	case Cursor_Resolve:
		VisitResolve(n, true);
		break;
	
	case Cursor_Rval:
		VisitRval(n);
		break;
	
	case Cursor_Null:
	case Cursor_Namespace:
	case Cursor_TypedefDecl:
	case Cursor_ClassDecl:
	case Cursor_ClassTemplate:
	case Cursor_CXXMethod:
		TODO
		break;
		
	case Cursor_ArgumentList:
		VisitArgumentList(n);
		break;
		
	case Cursor_Ctor:
		VisitConstructor(n);
		break;
		
	case Cursor_ArraySize:
		VisitArraySize(n);
		break;
		
	case Cursor_Object:
		break;
		
	default:
		if (IsPartially(n.src, Cursor_ExprOp)) {
			VisitExpression(n, 0);
			break;
		}
		if (IsPartially(n.src, Cursor_Literal)) {
			VisitLiteral(n);
			break;
		}
		TODO
	}
	
}

void AstExporter::Visit(const AstNode& n, CodeCursor t) {
	for(const AstNode& sub : n.val.Sub<AstNode>()) {
		if (sub.IsPartially(t)) {
			PushScope(sub);
			Visit(sub);
			PopScope();
		}
	}
}

void AstExporter::VisitStmt(const AstNode& n, CodeCursor t) {
	ASSERT(IsPartially(t, Cursor_Stmt));
	for(const AstNode& sub : n.val.Sub<AstNode>()) {
		if (IsPartially(sub.src, t)) {
			PushScope(sub);
			Visit(sub);
			PopScope();
		}
	}
}

void AstExporter::VisitCtorExpr(const AstNode& n) {
	for(const AstNode& sub : n.val.Sub<AstNode>()) {
		if (IsPartially(sub.src, Cursor_Ctor) ||
			sub.src == Cursor_ExprStmt) {
			PushScope(sub);
			Visit(sub);
			PopScope();
		}
	}
}

void AstExporter::VisitBuiltin(const AstNode& n) {
	output << GetCPath(n);
}

void AstExporter::VisitFunction(const AstNode& n) {
	ASSERT(n.src == Cursor_StaticFunction);
	
	output << GetIndentString();
	
	if (n.type) {
		Visit(*n.type, true);
		output << " ";
	}
	
	output << GetCPath();
	
	PushInlineScope();
	output << "(";
	Visit(n, Cursor_ClassPath_ParmDecl);
	output << ")";
	PopInlineScope();
	
	const AstNode* block = n.Find(Cursor_CompoundStmt);
	if (!block) {
		output << ";\n";
	}
	else {
		output << " {\n";
		Visit(n, Cursor_CompoundStmt);
		output << GetIndentString() << "}\n";
	}
}

void AstExporter::VisitParameter(const AstNode& n) {
	ASSERT(inline_scopes.GetCount());
	InlineScope& is = inline_scopes.Top();
	
	if (is.count)
		output << ", ";
	
	if (n.type)
		output << GetCPath(*n.type) << " ";
	
	output << GetCPath(n);
	
	is.count++;
}

void AstExporter::VisitStatement(const AstNode& n) {
	AstNode* p = 0;
	
	switch (n.src) {
	case Cursor_Null:
		break;
		
	case Cursor_ForStmt:
		ASSERT(inline_scopes.IsEmpty());
		PushInlineScope();
		output << GetIndentString() << "for (";
		VisitCtorExpr(n);
		output << "; ";
		VisitStmt(n, Cursor_ForStmt_Conditional);
		output << "; ";
		VisitStmt(n, Cursor_ForStmt_PostOp);
		output << ") {\n";
		PopInlineScope();
		Visit(n, Cursor_CompoundStmt);
		output << GetIndentString() << "}\n";
		break;
		
	case Cursor_IfStmt:
		ASSERT(inline_scopes.IsEmpty());
		PushInlineScope();
		output << GetIndentString() << "if (";
		Visit(n, Cursor_ExprOp);
		output << ") {\n";
		PopInlineScope();
		Visit(n, Cursor_CompoundStmt);
		output << GetIndentString() << "}\n";
		break;
		
	case Cursor_ElseStmt:
		ASSERT(inline_scopes.IsEmpty());
		PushInlineScope();
		output << GetIndentString() << "else {\n";
		PopInlineScope();
		Visit(n, Cursor_CompoundStmt);
		output << GetIndentString() << "}\n";
		break;
		
	case Cursor_ForStmt_Conditional:
	case Cursor_ForStmt_PostOp:
		Visit(n, Cursor_ExprOp);
		break;
		
	case Cursor_ExprStmt:
		output << GetIndentString() << "";
		for (auto it = n.val.Sub<AstNode>().rbegin(); it; it--) {
			const AstNode& s = it;
			if (s.IsPartially(Cursor_ExprOp) ||
				s.IsPartially(Cursor_Ctor)) {
				Visit(s);
				break;
			}
		}
		output << ";\n";
		break;
	
	case Cursor_ReturnStmt:
		output << GetIndentString() << "return";
		if (n.rval) {
			const AstNode& s = *n.rval;
			if (IsRvalReturn(s.src)) {
				output << " ";
				Visit(s);
			}
		}
		output << ";\n";
		break;
	
	case Cursor_CtorStmt:
		if (!n.rval)
			break;
		output << GetIndentString();
		VisitConstructor(*n.rval);
		output << ";\n";
		break;
		
	case Cursor_DoStmt:
	case Cursor_WhileStmt:
	case Cursor_ForStmt_Range:
	case Cursor_BreakStmt:
	case Cursor_ContinueStmt:
	case Cursor_CaseStmt:
	case Cursor_DefaultStmt:
	case Cursor_SwitchStmt:
	case Cursor_BlockExpr:
	case Cursor_MetaBlockExpr:
	default:
		TODO
		break;
	}
}

void AstExporter::VisitExpression(const AstNode& n, int depth) {
	ASSERT(IsPartially(n.src, Cursor_ExprOp));
	
	if (n.src == Cursor_VarDecl || n.src == Cursor_ParmDecl) {
		VisitVariable(n);
		return;
	}
	else if (IsPartially(n.src, Cursor_Literal) || n.src == Cursor_Object) {
		VisitLiteral(n);
		return;
	}
	else if (n.src == Cursor_Resolve) {
		VisitResolve(n);
		return;
	}
	else if (n.src == Cursor_Rval) {
		VisitRval(n);
		return;
	}
	else if (n.src == Cursor_ArgumentList) {
		VisitArgumentList(n);
		return;
	}
	else if (n.IsPartially(Cursor_Function)) {
		VisitFunctionRval(n);
		return;
	}
	else if (n.src == Cursor_MetaRval) {
		LOG(n.GetTreeString(0));
		TODO
	}
	
	ASSERT(IsPartially(n.src, Cursor_Op));
	
	if (depth > 0)
		output << "(";
	
	switch (n.src) {
	case Cursor_Op_NULL:
		break;
		
	case Cursor_Op_INC:
	case Cursor_Op_DEC:
	case Cursor_Op_NEGATIVE:
	case Cursor_Op_POSITIVE:
	case Cursor_Op_NOT:
	case Cursor_Op_NEGATE:
		ASSERT(n.arg[0]);
		output << GetOpCodeString(n.src);
		VisitExpression(*n.arg[0], depth+1);
		break;
		
	case Cursor_Op_POSTINC:
	case Cursor_Op_POSTDEC:
		ASSERT(n.arg[0]);
		VisitExpression(*n.arg[0], depth+1);
		output << GetOpCodeString(n.src);
		break;
	
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
		ASSERT(n.arg[0]);
		ASSERT(n.arg[1]);
		VisitExpression(*n.arg[0], depth+1);
		output << " " << GetOpCodeString(n.src) << " ";
		VisitExpression(*n.arg[1], depth+1);
		break;
		
	case Cursor_Op_ASSIGN:
	case Cursor_Op_ADDASS:
	case Cursor_Op_SUBASS:
	case Cursor_Op_MULASS:
	case Cursor_Op_DIVASS:
	case Cursor_Op_MODASS:
		ASSERT(n.arg[0]);
		ASSERT(n.arg[1]);
		VisitExpression(*n.arg[0], depth);
		output << " " << GetOpCodeString(n.src) << " ";
		VisitExpression(*n.arg[1], depth);
		break;
		
	case Cursor_Op_COND:
		ASSERT(n.arg[0]);
		ASSERT(n.arg[1]);
		ASSERT(n.arg[2]);
		output << "(";
		VisitExpression(*n.arg[0], depth+1);
		output << ") ? (";
		VisitExpression(*n.arg[1], depth+1);
		output << ") : (";
		VisitExpression(*n.arg[2], depth+1);
		output << ")";
		break;
		
	case Cursor_Op_CALL:
		ASSERT(n.arg[0]);
		ASSERT(n.arg[1]);
		VisitExpression(*n.arg[0], depth+1);
		VisitExpression(*n.arg[1], depth+1);
		break;
		
	case Cursor_Op_SUBSCRIPT:
		ASSERT(n.arg[0]);
		ASSERT(n.arg[1]);
		VisitExpression(*n.arg[0], depth+1);
		output << "[";
		VisitExpression(*n.arg[1], depth+1);
		output << "]";
		break;
		
	default:
		TODO
	}
	
	if (depth > 0)
		output << ")";
	
}

void AstExporter::VisitVariable(const AstNode& n, bool declare) {
	ASSERT(n.src == Cursor_VarDecl || n.src == Cursor_ParmDecl);
	
	if (declare) {
		if (n.type) {
			// use ctor instead
				//output << GetIndentString() << GetCPath(*n.type) << " " << GetCPath(n) << ";\n";
		}
	}
	else {
		output << GetCPath(n);
	}
	
}

void AstExporter::VisitArgument(const AstNode& n) {
	ASSERT(n.src == Cursor_Argument);
	
	ASSERT(n.rval);
	const AstNode& arg = *n.rval;
	
	InlineScope& is = inline_scopes.Top();
	if (is.count)
		output << ", ";
	
	if (IsPartially(arg.src, Cursor_Literal)) {
		VisitLiteral(arg);
	}
	else if (arg.IsPartially(Cursor_ValueDecl)) {
		output << GetCPath(arg);
	}
	else if (arg.src == Cursor_Rval) {
		VisitRval(arg);
	}
	else if (IsPartially(arg.src, Cursor_ExprOp)) {
		VisitExpression(arg, 0);
	}
	else {
		TODO
	}
	
	is.count++;
}

void AstExporter::VisitLiteral(const AstNode& n) {
	ASSERT(IsPartially(n.src, Cursor_Literal) || n.src == Cursor_Object);
	
	if (IsPartially(n.src, Cursor_Literal)) {
		switch (n.src) {
		case Cursor_Literal_BOOL:	output << (n.i64 ? "true" : "false"); break;
		case Cursor_Literal_INT32:	output << IntStr((int)n.i64); break;
		case Cursor_Literal_INT64:	output << IntStr64(n.i64); break;
		case Cursor_Literal_DOUBLE:	output << DblStr(n.dbl); break;
		case Cursor_Literal_STRING:	output << "\"" << n.str << "\""; break;
		default:
			TODO
			output << "<internal error>";
			break;
		}
	}
	else if (n.src == Cursor_Object) {
		output << n.obj.ToString();
	}
}

void AstExporter::VisitResolve(const AstNode& n, bool rval) {
	ASSERT(n.rval);
	if (n.rval) {
		const AstNode& l = *n.rval;
		if (l.IsPartially(Cursor_MetaFunction)) {
			output << "<error>";
		}
		else {
			if (l.IsPartially(Cursor_Function)) {
				DUMP(GetCodeCursorString(l.src));
			}
			output << GetCPath(l);
		}
	}
}

void AstExporter::VisitRval(const AstNode& n) {
	ASSERT(n.rval);
	ASSERT(n.rval != &n);
	if (n.rval) {
		AstNode& s = *n.rval;
		if (IsPartially(s.src, Cursor_Literal) || s.src == Cursor_Object)
			VisitLiteral(s);
		else if (s.IsPartially(Cursor_ValueDecl))
			output << GetCPath(*n.rval);
		else if (s.src == Cursor_MetaParameter || s.src == Cursor_MetaVariable) {
			TODO // this s.src shouldn't appear in this phase anymore
		}
		else
			Visit(*n.rval);
	}
	else {
		AddError(n.loc, "internal error: expected link");
	}
}

void AstExporter::VisitArgumentList(const AstNode& n) {
	PushInlineScope();
	output << "(";
	Visit(n, Cursor_Argument);
	output << ")";
	PopInlineScope();
}

void AstExporter::VisitFunctionRval(const AstNode& n) {
	output << GetCPath(n);
}

void AstExporter::VisitConstructor(const AstNode& n) {
	ASSERT(n.src == Cursor_Ctor);
	
	if (n.type) {
		output << GetCPath(*n.type) << " ";
	}
	
	if (n.rval)
		output << GetCPath(*n.rval);
	
	ASSERT(n.arg[0]);
	for(int i = 0; i < AstNode::ARG_COUNT; i++) {
		if (n.arg[i])
			Visit(*n.arg[i]);
	}
}

void AstExporter::VisitArraySize(const AstNode& n) {
	output << "[";
	ASSERT(n.rval);
	Visit(*n.rval);
	output << "]";
}

String AstExporter::GetCPath() const {
	int c = scopes.GetCount();
	int begin = c-1;
	for(int i = c-2; i >= 0; i--) {
		const Scope& scope = scopes[i];
		if (scope.pop_this)
			break;
		begin = i;
	}
	String s;
	for(int i = begin, j = 0; i < c; i++, j++) {
		const Scope& scope = scopes[i];
		if (j) s.Cat('_');
		s.Cat(scope.n->val.id);
	}
	return s;
}

String AstExporter::GetCPath(const AstNode& n) const {
	int c = scopes.GetCount();
	int common_i = -1;
	const AstNode* parts[PathIdentifier::MAX_PARTS];
	int part_count = 0;
	for(int i = 0; i < c && common_i < 0; i++) {
		int j = c-1-i;
		const Scope& scope = scopes[j];
		const AstNode* iter = &n;
		part_count = 0;
		while (iter) {
			if (iter == scope.n) {
				common_i = j;
				break;
			}
			parts[part_count++] = iter;
			iter = iter->val.owner ? iter->val.owner->FindExt<AstNode>() : 0;
		}
	}
	if (common_i < 0) {
		TODO
	}
	else {
		String s;
		if (!part_count) {
			String name = n.val.id;
			ASSERT(name.GetCount());
			s.Cat(name);
		}
		else {
			for (int i = 0; i < part_count; i++) {
				const AstNode* part = parts[part_count - 1 -i];
				if (part->src == Cursor_TypePointer)
					s.Cat('*');
				else if (part->src == Cursor_TypeLref)
					s.Cat('&');
				else {
					if (i) s.Cat('_');
					s.Cat(part->val.id);
				}
			}
		}
		return s;
	}
	return String();
}





void InitHighExporter(AstExporterLanguage& l) {
	
}

void InitCppExporter(AstExporterLanguage& l) {
	
}



END_UPP_NAMESPACE
