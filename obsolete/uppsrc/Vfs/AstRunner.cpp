#include "Vfs.h"

NAMESPACE_UPP


AstRunner::AstRunner(VfsValue& v) :
	VfsValueExt(v),
	ErrorSource("AstRunner")
{
	
}

bool AstRunner::Execute(const AstNode& n) {
	
	InitDefault(false);
	
	AstNode& root = GetRoot();
	
	for (const AstNode& s : n.val.Sub<AstNode>()) {
		if (s.src == Cursor_Builtin || s.src == Cursor_MetaBuiltin) {
			AstNode* local = root.Find(s.val.id, s.src);
			if (local)
				local->prev = &s;
		}
	}
	
	if (!Visit(n))
		return false;
	
	return true;
}

String AstRunner::GetTreeString(int indent) const {
	AstNode& root = GetRoot();
	return root.GetTreeString(indent);
}

String AstRunner::GetCodeString(const CodeArgs2& args) const {
	AstNode& root = GetRoot();
	return root.GetCodeString(args);
}

String AstRunner::ToString() const {
	AstNode& root = GetRoot();
	return root.ToString();
}

AstNode& AstRunner::GetRoot() const {
	return const_cast<VfsValue&>(val).GetAdd<AstNode>("root");
}

AstNode& AstRunner::GetRoot() {
	return val.GetAdd<AstNode>("root");
}

AstNode* AstRunner::Merge(const AstNode& n) {
	AstNode& owner = GetNonLockedOwner();
	
	if (n.val.id.GetCount()) {
		AstNode* prev = owner.Find(n.val.id);
		if (prev) {
			if (!IsMergeable(*prev, n)) {
				AddError(n.loc, "cannot merge " + GetCodeCursorString(n.src) + " to " + GetCodeCursorString(prev->src));
				return 0;
			}
			return prev;
		}
	}
	else {
		AstNode* prev = owner.Find(n.src);
		if (prev) {
			if (!IsMergeable(*prev, n)) {
				AddError(n.loc, "cannot merge " + GetCodeCursorString(n.src) + " to " + GetCodeCursorString(prev->src));
				return 0;
			}
			return prev;
		}
	}
	
	AstNode& d = owner.Add(n.loc);
	d.CopyFrom(this, n);
	
	return &d;
}

AstNode* AstRunner::VisitMetaFor(const AstNode& n) {
	ASSERT(n.src == Cursor_MetaForStmt);
	
	const AstNode* block = 0;
	const AstNode* for_cond = 0;
	const AstNode* for_post = 0;
	const AstNode* for_range = 0;
	
	AstNode* closest_type = GetClosestType();
	
	AstNode* d = AddDuplicate(n);
	if (!d) return 0;
	
	PushScope(*d, true);
	
	for (const AstNode& s : n.val.Sub<AstNode>()) {
		if (s.src == Cursor_MetaForStmt_Conditional) {
			for_cond = &s;
		}
		else if (s.src == Cursor_MetaForStmt_Post) {
			for_post = &s;
		}
		else if (s.src == Cursor_MetaForStmt_Range) {
			for_range = &s;
		}
		else if (s.src == Cursor_Rval) {
			// pass
		}
		else if (s.src == Cursor_CompoundStmt) {
			block = &s;
		}
		else if (s.src == Cursor_Stmt) {
			if (!Visit(s))
				return 0;
		}
		else TODO
	}
	
	if (!block) {
		AddError(n.loc, "no statement-block in meta-for loop");
		return 0;
	}
	
	AstNode* dup_block = 0;
	
	if (for_range) {
		TODO
	}
	else {
		if (!for_cond || !for_post) {
			AddError(n.loc, "internal error: no for-cond or for-post statement");
			return 0;
		}
		
		
		int dbg_i = 0;
		while (1) {
			AstNode* cond_val = Evaluate(*for_cond);
			if (!cond_val)
				return 0;
			
			bool b = (bool)cond_val->obj;
			if (!b)
				break;
			
			CHECK_SPATH_BEGIN
			dup_block = AddDuplicate(*block);
			if (!dup_block) return 0;
			PushScope(*dup_block);
			dup_block->i64 = 1; // skip indent
			
			for (const AstNode& s : block->val.Sub<AstNode>()) {
				if (!Visit(s))
					return 0;
				if (IsRvalReturn(s.src))
					PopScope();
			}
			
			PopScope(); // dup_block
			CHECK_SPATH_END
			
			AstNode* post_val = Evaluate(*for_post);
			if (!post_val)
				return 0;
			
			dbg_i++;
		}
	}
	
	PopScope(); // d
	
	SetMetaBlockType(*d, 0, closest_type);
	
	return builtin_void;
}

AstNode* AstRunner::VisitMetaIf(const AstNode& n) {
	ASSERT(n.src == Cursor_MetaIfStmt);
	
	AstNode* d = AddDuplicate(n); // these can't be merge becaues of re-use
	d->src = Cursor_Rval;
	if (!d)
		return 0;
	
	const AstNode* cond = n.Find(Cursor_ExprOp);
	if (!cond) {
		AddError(n.loc, "internal error: expression not found");
		return 0;
	}
	
	AstNode* closest_type = GetClosestType();
	AstNode& owner_block = GetBlock();
	const AstNode* block = n.Find(Cursor_CompoundStmt);
	if (!block) {
		AddError(n.loc, "internal error: block not found");
		return 0;
	}
	
	AstNode* else_ = n.ctx_next;
	ASSERT(!else_ || (else_->src == Cursor_MetaElseStmt));
	
	AstNode* cond_val = Evaluate(*cond);
	if (!cond_val)
		return 0;
	
	d->type = closest_type;
	PushScope(*d, true);
	
	AstNode* dup_block = 0;
	bool b = (bool)cond_val->obj;
	if (b) {
		block->locked = true;
		
		dup_block = AddDuplicate(*block);
		if (!dup_block) return 0;
		dup_block->type = closest_type;
		PushScope(*dup_block, true);
		if (!VisitStatementBlock(*block, false))
			return 0;
		PopScope();
	}
	else if (else_) {
		block = else_->Find(Cursor_CompoundStmt);
		if (!block) {
			AddError(else_->loc, "internal error: block not found");
			return 0;
		}
		
		block->locked = true;
		
		dup_block = AddDuplicate(*block);
		if (!dup_block) return 0;
		dup_block->type = closest_type;
		PushScope(*dup_block, true);
		if (!VisitStatementBlock(*block, false))
			return 0;
		PopScope();
	}
	
	PopScope();
	
	SetMetaBlockType(*d, dup_block, closest_type);
	
	return d;
}

void AstRunner::SetMetaBlockType(AstNode& d, AstNode* dup_block, AstNode* closest_type) {
	if (dup_block)
		dup_block->i64 = 1; // skip indent
	
	if (!closest_type) {
		d.src = Cursor_CompoundStmt;
		d.i64 = 1; // skip indent
	}
	else if (closest_type == this->meta_builtin_expr)
		d.rval = dup_block->rval;
	else if (closest_type->IsPartially(Cursor_MetaBuiltin))
		; // pass
	else
		TODO
}

AstNode* AstRunner::MergeStatement(const AstNode& n) {
	AstNode& owner = GetNonLockedOwner();
	AstNode* d = 0;
	AstNode* sd = 0;
	AstNode* block = 0;
	
	ASSERT(IsPartially(n.src, Cursor_Stmt));
	if (!IsPartially(n.src, Cursor_Stmt))
		return 0;
	
	switch (n.src) {
	case Cursor_ReturnStmt:
		return VisitReturn(n);
		
	case Cursor_ExprStmt:
		d = AddDuplicate(n);
		PushScope(*d, true);
		ASSERT(n.val.Sub<AstNode>().GetCount());
		ASSERT(n.rval || n.val.Sub<AstNode>().GetCount() == 1);
		if (n.rval) {
			const AstNode& s = *n.rval;
			sd = Visit(s);
			if (!sd)
				return 0;
			d->rval = sd;
			if (IsRvalReturn(s.src))
				PopScope(); // expr rval
		}
		else {
			const AstNode& s = n.val.Sub<AstNode>().begin();
			sd = Visit(s);
			if (!sd)
				return 0;
			d->rval = sd;
			if (IsRvalReturn(s.src))
				PopScope(); // expr rval
		}
		PopScope();
		return d;
		
	case Cursor_MetaForStmt:
	case Cursor_MetaIfStmt:
	case Cursor_MetaElseStmt:
		break;
	
	case Cursor_AtomConnectorStmt:
	case Cursor_IfStmt:
		d = AddDuplicate(n);
		d->CopyFrom(this, n);
		if (n.val.Sub<AstNode>().GetCount()) {
			PushScope(*d);
			for (const AstNode& s : n.val.Sub<AstNode>()) {
				if (!Visit(s))
					return 0;
				if (IsRvalReturn(s.src))
					PopScope();
			}
			PopScope();
		}
		return d;
	
	case Cursor_CtorStmt:
		if (n.rval && n.rval->IsPartially(Cursor_MetaDecl))
			return VisitMetaCtor(*n.rval);
		d = AddDuplicate(n);
		if (!d)
			return 0;
		PushScope(*d);
		ASSERT(n.val.Sub<AstNode>().GetCount());
		ASSERT(n.rval);
		if (n.rval) {
			AstNode& s = *n.rval;
			d->rval = Visit(s);
			if (!d->rval)
				return 0;
			if (IsRvalReturn(s.src))
				PopScope(); // expr rval
		}
		PopScope();
		ASSERT(d->rval || n.rval->type->IsPartially(Cursor_MetaDecl));
		return d;
		
	case Cursor_Null:
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
	case Cursor_SwitchStmt:
	case Cursor_BlockExpr:
	case Cursor_MetaBlockExpr:
	default:
		TODO
		break;
		
	}
	
	AddError(n.loc, "internal error");
	return 0;
}

bool AstRunner::IsMergeable(const AstNode& prev, const AstNode& to_merge) const {
	if (to_merge.src != prev.src)
		return false;
	
	return true;
}

AstNode* AstRunner::Visit(const AstNode& n) {
	AstNode* d = 0;
	AstNode* sd = 0;
	AstNode* ad = 0;
	int pop_count;
	CHECK_SPATH_INIT
	
	#ifdef flagDEBUG_VFS
	LOG("AstRunner::Visit: " + GetCodeCursorString(n.src) + " (" + HexStrPtr(&n) + ")");
	if (0 && n.src == Cursor_ExprStmt) {
		#ifdef flagWIN32
		DebugBreak();
		#else
		raise(SIGTRAP);
		#endif
	}
	#endif
	
	switch (n.src) {
	case Cursor_TranslationUnit:
		ASSERT(spath.IsEmpty());
		d = &GetRoot();
		d->src = Cursor_TranslationUnit;
		PushScope(*d);
		for (const AstNode& s : n.val.Sub<AstNode>()) {
			if (s.src == Cursor_Rval)
				continue;
			sd = Visit(s);
			if (!sd)
				return 0;
			if (IsRvalReturn(s.src))
				PopScope();
		}
		PopScope();
		break;
		
	case Cursor_LoopStmt:
	case Cursor_ChainStmt:
	case Cursor_AtomStmt:
	case Cursor_MachineStmt:
	case Cursor_LoopDecl:
	case Cursor_WorldStmt:
	case Cursor_SystemStmt:
	case Cursor_PoolStmt:
	case Cursor_DriverStmt:
	case Cursor_StateStmt:
		d = Merge(n);
		PushScope(*d);
		for (const AstNode& s : n.val.Sub<AstNode>()) {
			sd = Visit(s);
			if (!sd)
				return 0;
			if (IsRvalReturn(s.src))
				PopScope();
		}
		PopScope();
		d->rval = builtin_void;
		break;
		
	case Cursor_EntityStmt:
	case Cursor_ComponentStmt:
		d = AddDuplicate(n);
		PushScope(*d);
		for (const AstNode& s : n.val.Sub<AstNode>()) {
			sd = Visit(s);
			if (!sd)
				return 0;
			if (IsRvalReturn(s.src))
				PopScope();
		}
		PopScope();
		d->rval = builtin_void;
		break;
		
	case Cursor_Builtin:
	case Cursor_FunctionBuiltin:
		d = Merge(n);
		break;
	
	case Cursor_MetaBuiltin:
		d = builtin_void;
		break;
	
	case Cursor_MetaVariable:
		d = DeclareMetaVariable(n);
		break;
		
	case Cursor_MetaCtor:
		d = VisitMetaCtor(n);
		break;
		
	case Cursor_MetaStaticFunction:
		d = VisitMetaStaticFunction(n);
		break;
		
	case Cursor_Unresolved:
	case Cursor_MetaParameter:
		d = AddDuplicate(n);
		if (!d)
			return 0;
		if (!n.val.Sub<AstNode>().IsEmpty()) {
			PushScope(*d);
			for (const AstNode& s : n.val.Sub<AstNode>()) {
				if (s.src == Cursor_Rval)
					continue;
				sd = Visit(s);
				if (!sd)
					return 0;
				if (IsRvalReturn(s.src))
					PopScope();
			}
			PopScope();
		}
		break;
		
	
	case Cursor_StaticFunction:
	case Cursor_VarDecl:
		d = Merge(n);
		if (!d)
			return 0;
		if (!n.val.Sub<AstNode>().IsEmpty()) {
			PushScope(*d);
			for (const AstNode& s : n.val.Sub<AstNode>()) {
				if (s.src == Cursor_Rval)
					continue;
				sd = Visit(s);
				if (!sd)
					return 0;
				if (IsRvalReturn(s.src))
					PopScope();
			}
			PopScope();
		}
		break;
		
	case Cursor_CompoundStmt:
		d = Merge(n);
		if (!d)
			return 0;
		if (!n.val.Sub<AstNode>().IsEmpty()) {
			PushScope(*d);
			if (!VisitStatementBlock(n, false))
				return 0;
			PopScope();
		}
		break;
		
	case Cursor_NamePart:
		if (n.val.id.IsEmpty()) {
			AddError(n.loc, "internal error: no name");
			return 0;
		}
		d = GetTopNode().Find(n.val.id);
		if (!d)
			d = AddDuplicate(n);
		PushScope(*d);
		for (const AstNode& s : n.val.Sub<AstNode>()) {
			if (!Visit(s))
				return 0;
		}
		PopScope();
		d->rval = builtin_void;
		break;
		
	case Cursor_ArgumentList:
		d = Merge(n);
		if (!d)
			return 0;
		if (!n.val.Sub<AstNode>().IsEmpty()) {
			PushScope(*d);
			
			CHECK_SPATH_BEGIN
			
			for (const AstNode& s : n.val.Sub<AstNode>()) {
				if (s.src == Cursor_Argument || s.IsPartially(Cursor_MetaDecl)) {
					sd = Visit(s);
					if (!sd)
						return 0;
					if (IsRvalReturn(s.src))
						PopScope();
					
					CHECK_SPATH_END
				}
			}
			
			CHECK_SPATH_END
			
			PopScope();
		}
		PushScopeRVal(*d);
		break;
		
	case Cursor_Argument:
		d = AddDuplicate(n);
		if (!d)
			return 0;
		ASSERT(n.rval);
		if (n.rval) {
			PushScope(*d);
			d->rval = Visit(*n.rval);
			if (!d->rval)
				return 0;
			if (IsRvalReturn(n.rval->src))
				PopScope();
			PopScope();
		}
		break;
		
	case Cursor_Ctor:
		if (n.type && n.type->IsPartially(Cursor_MetaTypeDecl))
			return VisitMetaCtor(n);
		d = AddDuplicate(n);
		if (!d)
			return 0;
		ASSERT(n.arg[0]);
		if (n.arg[0]) {
			PushScope(*d);
			ad = Visit(*n.arg[0]);
			if (!ad)
				return 0;
			d->arg[0] = ad;
			if (IsRvalReturn(n.arg[0]->src))
				PopScope();
			PopScope();
		}
		d->rval = FindStackWithPrevDeep(n.rval);
		PushScopeRVal(*d);
		ASSERT(d->arg[0]);
		ASSERT(d->rval);
		break;
		
	case Cursor_Rval:
		ASSERT(n.rval);
		if (n.rval->IsPartially(Cursor_MetaValueDecl)) {
			return VisitMetaRVal(n);
		}
		else {
			d = AddDuplicate(n);
			if (!d)
				return 0;
			if (n.rval) {
				AstNode* rval = n.rval;
				while (rval && (rval->src == Cursor_Rval || rval->src == Cursor_MetaCtor))
					rval = rval->rval;
				if (!rval) {
					AddError(n.loc, "internal error: invalid rval");
					return 0;
				}
				
				if (rval->src == Cursor_MetaResolve) {
					d->rval = VisitMetaResolve(*rval);
				}
				else {
					d->rval = FindStackWithPrevDeep(rval);
				}
				ASSERT(d->rval); // todo crash here might mean succesfull merge (see same named variables)
				if (!d->rval) {
					AddError(n.loc, "internal error: incomplete rval");
					return 0;
				}
				
				if (d->rval->src != Cursor_NamePart && d->rval->IsPartially(Cursor_MetaDecl))
					d->src = Cursor_MetaRval;
			}
			else TODO
			PushScopeRVal(*d);
			
		}
		break;
	
	case Cursor_ParmDecl:
		d = &GetTopNode().Add(n.loc, n.val.id);
		d->CopyFrom(this, n);
		d->src = Cursor_Object;
		break;
	
	case Cursor_Resolve:
		return VisitResolve(n);
	
	case Cursor_Null:
	case Cursor_Namespace:
	case Cursor_TypedefDecl:
	case Cursor_ClassDecl:
	case Cursor_ClassTemplate:
	case Cursor_CXXMethod:
	case Cursor_ArraySize:
		TODO
		break;
		
	case Cursor_MetaForStmt:
		return VisitMetaFor(n);
		
	case Cursor_MetaIfStmt:
		return VisitMetaIf(n);
		
	case Cursor_MetaElseStmt: ASSERT_(0, "never call"); break;
	
	default:
		if (IsPartially(n.src, Cursor_Stmt))
			return MergeStatement(n);
		
		if (IsPartially(n.src, Cursor_Literal)) {
			d = AddDuplicate(n);
			if (!d)
				return 0;
			PushScopeRVal(*d);
			break;
		}
		
		if (IsPartially(n.src, Cursor_ExprOp)) {
			d = AddDuplicate(n);
			if (!d)
				return 0;
			CHECK_SPATH_BEGIN_1
			pop_count = 0;
			for(int i = 0; i < n.i64; i++) {
				ASSERT(n.arg[i]);
				AstNode& a = *n.arg[i];
				AstNode* ad = Visit(a);
				if (!ad)
					return 0;
				if (IsRvalReturn(a.src))
					pop_count++;
				d->arg[i] = ad;
			}
			for(int i = 0; i < pop_count; i++) {
				PopScope();
			}
			PushScope(*d);
			CHECK_SPATH_END
			
			if (d->src == Cursor_Op_CALL && d->arg[0] && d->arg[0]->src == Cursor_MetaRval && d->arg[1]) {
				if (!VisitMetaCall(*d, *d->arg[0], *d->arg[1]))
					return 0;
			}
			else if (d->arg[0] && d->arg[0]->src == Cursor_MetaRval)
				TODO
			break;
		}
		TODO
	}
	
	ASSERT(d);
	return d;
}

AstNode* AstRunner::AddDuplicate(const AstNode& n) {
	AstNode& owner = GetNonLockedOwner();
	AstNode& d = owner.Add(n.loc);
	d.CopyFrom(this, n);
	return &d;
}

AstNode* AstRunner::VisitMetaRVal(const AstNode& n) {
	if (!n.rval || !n.rval->IsPartially(Cursor_MetaValueDecl)) {ASSERT(0); return 0;}
	
	const AstNode& ref = *n.rval;
	
	if (ref.src == Cursor_MetaVariable || ref.src == Cursor_MetaParameter) {
		ASSERT(ref.val.id.GetCount());
		AstNode* o = FindStackWithPrev(&ref);
		ASSERT(o);
		while (o && o->src == Cursor_Rval)
			o = o->rval;
		if (!o) {
			AddError(n.loc, "internal error: no object");
			return 0;
		}
		AstNode& owner = GetNonLockedOwner();
		AstNode& d = owner.Add(n.loc);
		
		if (IsPartially(o->src, Cursor_Literal)) {
			d.CopyFrom(this, *o);
		}
		else if (o->src == Cursor_Object) {
			if (o->obj.IsVoid()) {
				AddError(o->loc, "meta-object is not initalized yet");
				return 0;
			}
			d.CopyFromValue(n.loc, o->obj);
		}
		else
			TODO;
		
		PushScopeRVal(d);
		
		return &d;
	}
	else TODO;
	
	return 0;
}

AstNode* AstRunner::VisitMetaCtor(const AstNode& n) {
	ASSERT(n.src == Cursor_MetaCtor);
	ASSERT(n.type && n.type->IsPartially(Cursor_MetaTypeDecl));
	ASSERT(n.rval);
	
	AstNode& var = *n.rval;
	const AstNode* args = n.Find(Cursor_ArgumentList);
	if (!args) {
		TODO
	}
	
	ASSERT(var.val.id.GetCount());
	
	AstNode* o = FindStackValue(var.val.id);
	
	{
		// Kinda hack: to allow finding node with ".prev == &n"
		AstNode* d = Merge(n);
		d->src = Cursor_Rval;
		d->rval = o;
		d->val.id = var.val.id;
		ASSERT(o);
	}
	
	int arg_count = 0;
	const AstNode* arg = 0;
	for (const AstNode& s : args->val.Sub<AstNode>()) {
		if (s.src == Cursor_Argument) {
			arg = &s;
			arg_count++;
		}
	}
	
	if (arg_count == 1) {
		ASSERT(arg->rval);
		AstNode& argvar = *arg->rval;
		
		if (IsPartially(argvar.src, Cursor_Literal)) {
			argvar.CopyToValue(o->obj);
		}
		else if (IsPartially(argvar.src, Cursor_ExprOp)) {
			AstNode* argval = Evaluate(argvar);
			if (!argval)
				return 0;
			
			o->obj = argval->obj;
		}
		else TODO
	}
	else {
		TODO
	}
	
	return o;
}

AstNode* AstRunner::VisitMetaStaticFunction(const AstNode& n) {
	ASSERT(n.src == Cursor_MetaStaticFunction);
	
	AstNode* d = Merge(n);
	if (!d)
		return 0;
	
	return d;
}

AstNode* AstRunner::VisitResolve(const AstNode& n) {
	ASSERT(n.src == Cursor_Resolve);
	ASSERT(n.filter != Cursor_Null);
	ASSERT(!n.id.IsEmpty());
	
	AstNode* decl = FindDeclaration(n.id, n.filter);
	if (!decl) {
		AddError(n.loc, "could not find declaration '" + n.id.ToString() + "'");
		return 0;
	}
	
	if (decl->src == Cursor_MetaStaticFunction) {
		AstNode& rval = GetTopNode().Add(n.loc);
		rval.CopyFrom(this, n);
		rval.src = Cursor_MetaRval;
		rval.rval = decl;
		PushScopeRVal(rval);
		return &rval;
	}
	else TODO
	return 0;
}

bool AstRunner::VisitStatementBlock(const AstNode& n, bool req_rval) {
	AstNode& block = GetBlock();
	AstNode& owner = GetNonLockedOwner();
	ASSERT(block.src == Cursor_CompoundStmt);
	ASSERT(&n != &block);
	int dbg_count = 0;
	
	if (!n.val.Sub<AstNode>().IsEmpty()) {
		int dbg_i = 0;
		for (const AstNode& s : n.val.Sub<AstNode>()) {
			if (s.src == Cursor_ElseStmt || s.src == Cursor_MetaElseStmt)
				continue;
			
			int prev_count = block.val.Sub<AstNode>().GetCount();
			CHECK_SPATH_BEGIN
			
			AstNode* sd = Visit(s);
			if (!sd)
				return false;
			
			if (sd && sd->rval) {
				AstNode& r = *sd->rval;
				if (block.type == meta_builtin_expr) {
					if (IsRvalReturn(r.src)) {
						owner.rval = &r;
						ASSERT(&owner != owner.rval);
					}
				}
			}
			
			if (IsRvalReturn(s.src)) {
				PopScope();
			}
			
			bool added = block.val.Sub<AstNode>().GetCount() > prev_count;
			if (added) dbg_count++;
			
			if (s.src == Cursor_ReturnStmt) {ASSERT(added);}
			
			ASSERT(&GetBlock() == &block);
			CHECK_SPATH_END
			
			dbg_i++;
		}
	}
	
	if (req_rval && !owner.rval) {
		AddError(n.loc, "internal error: no rval");
		return false;
	}
	return true;
}

AstNode* AstRunner::VisitReturn(const AstNode& n) {
	AstNode& block = GetBlock();
	ASSERT(spath.Top().n == &block);
	int c = block.val.Sub<AstNode>().GetCount();
	
	AstNode* d = AddDuplicate(n);
	if (n.val.Sub<AstNode>().GetCount()) {
		CHECK_SPATH_BEGIN
		
		PushScope(*d);
		if (n.rval) {
			d->rval = Visit(*n.rval);
			if (!d->rval)
				return 0;
			if (IsRvalReturn(n.rval->src))
				PopScope();
		}
		PopScope();
		
		CHECK_SPATH_END
	}
	
	ASSERT(block.val.Sub<AstNode>().GetCount() > c);
	block.rval = d->rval;
	ASSERT(block.rval != &block);
	return d;
}

AstNode* AstRunner::VisitMetaResolve(const AstNode& n) {
	ASSERT(n.src == Cursor_MetaResolve);
	
	if (n.id.part_count == 0) {
		AddError(n.loc, "internal error: empty id");
		return 0;
	}
	
	Vector<String> parts;
	int part_i = 0;
	for (const Token* tk = n.id.begin ; tk != n.id.end; tk++) {
		if (tk->type == TK_ID) {
			if (n.id.is_meta[part_i]) {
				AstNode* a = FindStackName2(tk->str_value, n.filter, Cursor_Rval);
				while (a && a->src == Cursor_Rval)
					a = a->rval;
				if (!a) {
					AddError(n.loc, "meta-field '" + tk->str_value + "' not found");
					return 0;
				}
				if (a->src == Cursor_Object) {
					String s = a->obj.ToString();
					if (s.IsEmpty()) {
						AddError(n.loc, "meta-object '" + tk->str_value + "' gave empty string");
						return 0;
					}
					parts.Add(s);
				}
				else
					TODO
			}
			else {
				parts.Add(tk->str_value);
			}
			part_i++;
		}
	}
	
	
	AstNode* rval = FindDeclaration(parts);
	
	AstNode* d = AddDuplicate(n);
	d->src = Cursor_Rval;
	d->rval = rval;
	
	return d;
}

bool AstRunner::VisitMetaCall(AstNode& d, AstNode& rval, AstNode& args) {
	if (rval.src == Cursor_MetaRval && args.src == Cursor_ArgumentList) {
		const AstNode& fn = *rval.rval->prev;
		AstNode& ret_type = *fn.type;
		ASSERT(fn.src == Cursor_MetaStaticFunction);
		fn.locked = true;
		
		bool req_rval = false;
		if (fn.type == meta_builtin_expr)
			req_rval = true;
		
		AstNode* call = AddDuplicate(fn);
		call->src = Cursor_CompoundStmt;
		call->i64 = 1; // skip indent
		call->prev = 0; // to prevent wrong match
		PushScope(*call, true);
		
		
		PushScope(d, true);
		d.type = FindStackWithPrev(fn.type);
		if (!d.type) {
			AddError(fn.type->loc, "internal error: type not found in new scope");
			return false;
		}
		
		Vector<AstNode*> arg_ptrs;
		Vector<const AstNode*> param_ptrs;
		
		for (AstNode& arg : args.val.Sub<AstNode>()) {
			if (arg.src != Cursor_Argument) continue;
			arg_ptrs.Add(&arg);
		}
		for (const AstNode& param : fn.val.Sub<AstNode>()) {
			if (param.src != Cursor_MetaParameter) continue;
			param_ptrs.Add(&param);
		}
		if (arg_ptrs.GetCount() != param_ptrs.GetCount()) {
			AddError(args.loc, "got '" + IntStr(arg_ptrs.GetCount()) + "' arguments, but expected '" + IntStr(param_ptrs.GetCount()) + "'");
			return false;
		}
		
		for(int i = 0; i < arg_ptrs.GetCount(); i++) {
			AstNode& arg = *arg_ptrs[i];
			const AstNode& param = *param_ptrs[i];
			
			ASSERT(param.val.id.GetCount());
			AstNode& ao = call->Add(param.loc, param.val.id);
			ao.CopyFrom(this, param);
			ao.src = Cursor_Rval;
			
			if (!arg.rval) {
				AddError(arg.loc, "internal error: expected argument expression");
				return false;
			}
			AstNode& aexpr = *arg.rval;
			
			AstNode* aexpr_val = Evaluate(aexpr);
			if (!aexpr_val)
				return false;
			
			ao.rval = aexpr_val;
			ASSERT(ao.rval);
		}
		
		const AstNode* block = fn.Find(Cursor_CompoundStmt);
		if (!block) {
			AddError(fn.loc, "internal error: function has no statement block");
			return false;
		}
		
		AstNode* dup_block = AddDuplicate(*block);
		
		block->locked = true;
		dup_block->type = d.type;
		PushScope(*dup_block, true);
		
		if (!VisitStatementBlock(*block, req_rval))
			return false;
		
		PopScope(); // block
		PopScope(); // ret
		PopScope(); // call
		
		// Override meta-node type and link function return value
		if (spath.Top().n != &d) {
			AddError(d.loc, "internal error: stack did not unwind properly");
			return false;
		}
		
		CodeCursor filter = Cursor_Null;
		if (d.type) {
			if (d.type == meta_builtin_expr) {
				d.src = Cursor_Rval;
				d.rval = dup_block->rval;
				ASSERT(d.rval);
				
			}
			else if (d.type == meta_builtin_loopstmt) {
				d.src = Cursor_SymlinkStmt;
				d.rval = dup_block;
			}
			else if (!req_rval) {
				d.rval = builtin_void;
			}
			else TODO
		}
		else TODO;
	}
	else TODO
	
	return true;
}

AstNode* AstRunner::DeclareMetaVariable(const AstNode& n) {
	if (n.src == Cursor_MetaVariable) {
		ASSERT(n.val.id.GetCount());
		AstNode& block = GetBlock();
		if (block.Find(n.val.id, Cursor_Object)) {
			AddError(n.loc, "meta-variable '" + n.val.id + "' have already been declared");
			return 0;
		}
		
		AstNode& d = block.Add(n.loc, n.val.id);
		d.CopyFrom(this, n);
		d.src = Cursor_Object;
		return &d;
	}
	else TODO;
	return 0;
}

AstNode* AstRunner::Evaluate(const AstNode& n) {
	
	if (IsPartially(n.src, Cursor_Stmt)) {
		if (n.src == Cursor_ExprStmt ||
			n.src == Cursor_MetaForStmt_Conditional ||
			n.src == Cursor_MetaForStmt_Post) {
			for (auto it = n.val.Sub<AstNode>().rbegin(); it; it--) {
				const AstNode& e = it;
				if (IsPartially(e.src, Cursor_ExprOp)) {
					return Evaluate(e);
				}
				else TODO
			}
		}
		else {
			TODO
		}
	}
	else if (IsPartially(n.src, Cursor_Op)) {
		if (n.src == Cursor_Op_POSTINC || n.src == Cursor_Op_POSTDEC) {
			AstNode* a = n.arg[0];
			while (a && a->src == Cursor_Rval) {
				a = a->rval;
			}
			if (a) {
				AstNode* o = FindStackWithPrev(a); // from previous phase to AstRunner phase
				if (o && o->src == Cursor_Object) {
					AstNode* r = &GetTopNode().Add(n.loc);
					r->CopyFrom(this, n);
					r->src = Cursor_Literal;
					r->CopyFromValue(n.loc, o->obj);
					
					o->obj = (int64)o->obj + (int64)(n.src == Cursor_Op_POSTINC ? 1 : -1);
					
					return r;
				}
			}
		}
		
		AstNode* d = AddDuplicate(n);
		if (!d) return 0;
		d->src = Cursor_Object;
		Value& o = d->obj;
		
		Value a[AstNode::ARG_COUNT];
		
		ASSERT(n.i64 > 0);
		for(int i = 0; i < n.i64; i++) {
			if (!n.arg[i]) {
				AddError(n.loc, "expression failed");
				return 0;
			}
			AstNode* r = Evaluate(*n.arg[i]);
			if (!r)
				return 0;
			d->arg[i] = r;
			
			if (IsPartially(r->src, Cursor_Literal))
				r->CopyToValue(a[i]);
			else if (r->src == Cursor_Object)
				a[i] = r->obj;
			else
				TODO
		}
		
		switch (n.src) {
			case Cursor_Op_POSTINC:
			case Cursor_Op_POSTDEC:
			case Cursor_Op_POSITIVE:	o = a[0]; break;
			case Cursor_Op_NOT:		o = !a[0]; break;
			case Cursor_Op_NEGATIVE:	o = -(double)a[0]; break;
			case Cursor_Op_GREATER:	o = a[0] > a[1]; break;
			case Cursor_Op_LESS:		o = a[0] < a[1]; break;
			case Cursor_Op_ADD:		o = (double)a[0] + (double)a[1]; break;
			case Cursor_Op_SUB:		o = (double)a[0] - (double)a[1]; break;
			case Cursor_Op_MUL:		o = (double)a[0] * (double)a[1]; break;
			case Cursor_Op_DIV:		o = (double)a[0] / (double)a[1]; break;
			case Cursor_Op_LSH:		o = (int64)a[0] << (int64)a[1]; break;
			case Cursor_Op_RSH:		o = (int64)a[0] >> (int64)a[1]; break;
			case Cursor_Op_EQ:		o = a[0] == a[1]; break;
			case Cursor_Op_INEQ:		o = a[0] != a[1]; break;
			case Cursor_Op_AND:		o = a[0] && a[1]; break;
			case Cursor_Op_OR:		o = a[0] || a[1]; break;
			case Cursor_Op_GREQ:		o = a[0] >= a[1]; break;
			case Cursor_Op_LSEQ:		o = a[0] <= a[1]; break;
			
			case Cursor_Op_MOD:
			case Cursor_Op_NULL:
			case Cursor_Op_INC:
			case Cursor_Op_DEC:
			case Cursor_Op_NEGATE:
			case Cursor_Op_BWAND:
			case Cursor_Op_BWXOR:
			case Cursor_Op_BWOR:
			case Cursor_Op_ASSIGN:
			case Cursor_Op_ADDASS:
			case Cursor_Op_SUBASS:
			case Cursor_Op_MULASS:
			case Cursor_Op_DIVASS:
			case Cursor_Op_MODASS:
			case Cursor_Op_COND:
			case Cursor_Op_CALL:
			case Cursor_Op_SUBSCRIPT:
			default:
				TODO
		}
		
		return d;
	}
	else if (IsPartially(n.src, Cursor_Literal)) {
		AstNode* d = FindStackWithPrev(&n);
		if (d)
			return d;
		d = AddDuplicate(n);
		d->CopyFrom(this, n);
		return d;
	}
	else if (n.src == Cursor_Rval) {
		ASSERT(n.rval);
		AstNode* rval = FindStackWithPrev(n.rval);
		while (rval && rval->src == Cursor_Rval)
			rval = rval->rval;
		if (!rval) {
			AddError(n.loc, "internal error: incomplete rval");
			return 0;
		}
		else if (rval->src == Cursor_Object) {
			return rval;
		}
		else if (IsPartially(rval->src, Cursor_Literal)) {
			return rval;
		}
		else TODO
	}
	else TODO
	
	return 0;
}


END_UPP_NAMESPACE
