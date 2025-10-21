#include "clang.h"

// #define DUMPTREE

class ClangCursorInfo {
	CXCursor     cursor;
	CXCursorKind cursorKind;
	CXCursor     parent;
	CXCursorKind parentKind;
	CXPrintingPolicy pp_id;
 

	bool         hasraw_id = false;
	String       raw_id;

	bool         hasscope = false;
	String       scope;
	String       nspace;

	bool         hastype = false;
	String       type;

	bool         hasname = false;
	String       name;

	bool         hasid = false;
	String       id;

	bool         hastypehash = false;
	bool         typebuiltin = false;
	hash_t       typehash = 0;

public:
	int          Kind()                      { return cursorKind; }
	String       RawId();
	String       Scope();
	String       Nspace()                    { Scope(); return nspace; }
	String       Type();
	String       TypeDeclaration();
	String       ParentTypeDeclaration();
	String       Name();
	String       Id();
	String       Bases();
	hash_t       TypeHash();
	bool         TypeBuiltin();
	
	CXCursor     GetCursor()                 { return cursor; }

	ClangCursorInfo(CXCursor cursor, CXPrintingPolicy pp_id);
};

ClangCursorInfo::ClangCursorInfo(CXCursor cursor, CXPrintingPolicy pp_id)
:	cursor(cursor), pp_id(pp_id) {
	cursorKind = clang_getCursorKind(cursor);
	parent = clang_getCursorSemanticParent(cursor);
	parentKind = clang_getCursorKind(parent);
}

force_inline
String ClangCursorInfo::RawId()
{
	if(!hasraw_id) {
		raw_id = FetchString(clang_getCursorPrettyPrinted(cursor, pp_id));
		hasraw_id = true;
	}
	return raw_id;
}

force_inline
String ClangCursorInfo::Type()
{
	if(!hastype) {
		type = GetTypeSpelling(cursor);
		hastype = true;
	}
	return type;
}

force_inline
bool ClangCursorInfo::TypeBuiltin()
{
	TypeHash(); // refresh 'typebuiltin' value
	return typebuiltin;
}

force_inline
hash_t ClangCursorInfo::TypeHash()
{
	if(!hastypehash) {
		if (IsTypeDecl(cursorKind) || cursorKind == CXCursor_MacroDefinition) {
			typehash = clang_hashCursor(cursor);
		}
		else if (IsTypeRef(cursorKind) || cursorKind == CXCursor_MacroExpansion) {
			CXCursor ref = clang_getCursorReferenced(cursor);
			if (IsTypeDecl(ref.kind) ||
				ref.kind == CXCursor_MacroDefinition ||
				ref.kind == CXCursor_LabelStmt) {
				typehash = clang_hashCursor(ref);
			}
			else if (IsVariable(ref.kind)) {
				CXType type = clang_getCursorType(cursor);
				CXCursor type_decl = clang_getTypeDeclaration(type);
				typehash = clang_hashCursor(type_decl);
			}
			else if (IsErrorKind(ref.kind)) {
				// Invalid macro usage can't pass, and can be used as "very unique type"
				typehash = clang_hashCursor(cursor);
			}
			else {
				#ifdef flagDEBUG
				TODO;
				#else
				RLOG("ClangCursorInfo::TypeHash: error: invalid clang kind: " << GetCursorSpelling(ref) << ": (" << (int)ref.kind << ")");
				#endif
			}
		}
		else {
			CXType type = clang_getCursorType(cursor);
			CXCursor type_decl = clang_getTypeDeclaration(type);
			if (type.kind == CXType_Invalid)
				typehash = 0;
			else if (type_decl.kind == CXCursor_NoDeclFound && type.kind != CXType_Invalid) {
				CombineHash c;
				c.Put(type.kind).Put((int64)type.data[0]).Put((int64)type.data[1]);
				typehash = (hash_t)c;
				typebuiltin = true;
			}
			else if (IsTypeDecl(type_decl.kind))
				typehash = clang_hashCursor(type_decl);
			else {
				String type_str = Type();
				if (type_str.IsEmpty())
					typehash = 0;
				else
					typehash = type_str.GetHashValue();
				typebuiltin = true;
			}
		}
		hastypehash = true;
	}
	return typehash;
}

force_inline
String ClangCursorInfo::TypeDeclaration()
{
	return GetTypeDeclarationSpelling(cursor);
}

force_inline
String ClangCursorInfo::ParentTypeDeclaration()
{
	return GetTypeDeclarationSpelling(parent);
}

force_inline
String ClangCursorInfo::Name()
{
	if(!hasname) {
		name = GetCursorSpelling(cursor);
		hasname = true;
	}
	return name;
}

String ClangCursorInfo::Scope()
{
	if(!hasscope) {
		CXCursor p = parent;
		for(;;) {
			CXCursorKind k = clang_getCursorKind(p);
			if(k != CXCursor_EnumDecl) {
				if(findarg(k, CXCursor_Namespace, CXCursor_ClassTemplate, CXCursor_StructDecl, CXCursor_UnionDecl, CXCursor_ClassDecl) < 0)
					break;
				String q = GetCursorSpelling(p);
				scope = scope.GetCount() ? q + "::" + scope : q;
				if(k == CXCursor_Namespace)
					nspace = nspace.GetCount() ? q + "::" + nspace : q;
			}
			p = clang_getCursorSemanticParent(p);
		}
		int q = scope.Find('('); // 'Struct::(unnamed enum at C:\u\upp.src\upptst\Annotations\main.cpp:47:2)'
		if(q >= 0)
			scope.Trim(q);
		if(scope.GetCount() && *scope.Last() != ':')
			scope << "::";
		hasscope = true;
	}
	return scope;
}

String ClangCursorInfo::Id()
{
	if(!hasid) {
		String m, s;
		const char *p;
		int q = 0;
		switch(cursorKind) {
		case CXCursor_StructDecl:
		case CXCursor_ClassDecl:
		case CXCursor_UnionDecl:
		case CXCursor_TypedefDecl:
		case CXCursor_TypeAliasDecl:
		case CXCursor_EnumDecl:
			m = Type();
			break;
		case CXCursor_FunctionDecl:
		case CXCursor_Constructor:
		case CXCursor_Destructor:
		case CXCursor_FunctionTemplate:
		case CXCursor_CXXMethod:
#ifdef UBUNTU2204_WORKAROUND
			s = RawId();
			p = s;
			while(*p == '&' || *p == '*') // fix CleanupId("&Accel(int (*filter)(int))") -> Accel(int(*filter)())
				p++;
			m = CleanupId(p);
			{ // remove any scope
				int q = m.Find('(');
				if(q >= 0) {
					q = m.ReverseFind("::", q);
					if(q >= 0)
						m = m.Mid(q + 2);
				}
			}
			while(findarg(m[q], ':', '*', '&', '(', ')', ' ') >= 0)
				q++;
			id = Scope();
			id.Cat(~m + q, m.GetCount() - q);
			hasid = true;
			return id;
#else
			m = RawId();
#endif
			break;
		case CXCursor_ClassTemplate:
		case CXCursor_VarDecl:
		case CXCursor_ParmDecl:
		case CXCursor_FieldDecl:
			m << Scope() << Name();
			break;
		case CXCursor_ConversionFunction:
			m << Scope() << "operator " << Type();
			break;
		case CXCursor_MacroDefinition:
			id = Name();
			hasid = true;
			return id;
		case CXCursor_EnumConstantDecl:
			m << Scope() << Name();
			break;

/*
		case CXCursor_ParmDecl:
		case CXCursor_Namespace:
		case CXCursor_UnexposedDecl:
		case CXCursor_NamespaceAlias:
				break;
*/
		default:;
		}
		id = CleanupId(m);
		hasid = true;
	}
	return id;
}

String ClangCursorInfo::Bases()
{
	Tuple<CXPrintingPolicy, String> q;
	q.a = pp_id;
	if(findarg(cursorKind, CXCursor_StructDecl, CXCursor_ClassDecl, CXCursor_ClassTemplate) >= 0) {
		clang_visitChildren(cursor,
			[](CXCursor cursor, CXCursor p, CXClientData clientData) -> CXChildVisitResult {
				if(findarg(clang_getCursorKind(cursor), CXCursor_CXXBaseSpecifier) >= 0) {
					Tuple<CXPrintingPolicy, String>& q = *(Tuple<CXPrintingPolicy, String>*)clientData;
					CXCursor ref = clang_getCursorReferenced(cursor);
					if(!clang_Cursor_isNull(ref)) {
						ClangCursorInfo rf(ref, q.a);
						MergeWith(q.b, ";", rf.Id());
					}
				}
				return CXChildVisit_Continue;
			},
			&q
		);
	}
	if(cursorKind == CXCursor_TypedefDecl) {
		CXType type = clang_getTypedefDeclUnderlyingType(cursor);
		ClangCursorInfo cci(clang_getTypeDeclaration(type), pp_id);
		q.b = CleanupId(cci.Type());
	}
	return q.b;
}

ClangVisitor::CXLocation ClangVisitor::GetLocation(CXSourceLocation cxlocation)
{
	CXFile file;
	CXLocation p;
	unsigned line;
	unsigned column;
	unsigned offset;
	clang_getExpansionLocation(cxlocation, &file, &line, &column, &offset);
	p.pos.x = int(column - 1);
	p.pos.y = int(line - 1);
	p.file = file;
	return p;
}

ClangVisitor::CXRange ClangVisitor::GetRange(CXSourceRange cxrange)
{
	CXSourceLocation cxlocation0 = clang_getRangeStart(cxrange);
	CXSourceLocation cxlocation1 = clang_getRangeEnd(cxrange);
	CXFile file;
	CXRange r;
	unsigned line;
	unsigned column;
	unsigned offset;
	clang_getExpansionLocation(cxlocation0, &file, &line, &column, &offset);
	r.p0.pos.x = int(column - 1);
	r.p0.pos.y = int(line - 1);
	r.p0.file = file;
	clang_getExpansionLocation(cxlocation1, &file, &line, &column, &offset);
	r.p1.pos.x = int(column - 1);
	r.p1.pos.y = int(line - 1);
	r.p1.file = file;
	return r;
}

SourceLocation ClangVisitor::GetSourceLocation(const CXLocation& p, const CXRange& r)
{
	SourceLocation l;
	l.pos = p.pos;
	l.begin = r.p0.pos;
	l.end = r.p1.pos;
	int q = cxfile.Find(p.file);
	if(q >= 0)
		l.path = cxfile[q];
	else {
		l.path = NormalizePath(FetchString(clang_getFileName(p.file)));
		cxfile.Add(p.file, l.path);
	}
	return l;
}

bool ClangVisitor::ProcessNode(CXCursor cursor)
{
	CXSourceLocation cxlocation = clang_getCursorLocation(cursor);
	CXSourceRange cxrange = clang_getCursorExtent(cursor);
	
	ClangCursorInfo ci(cursor, pp_id);

#ifdef DUMPTREE
	_DBG_
//	if(GetCursorSpelling(cursor).Find("DeleteFile") >= 0)
	{
		CXCursor ref = clang_getCursorReferenced(cursor);
		ClangCursorInfo cir(ref, pp_id);

		if(ci.Kind() == CXCursor_TypedefDecl) {
			CXType type = clang_getTypedefDeclUnderlyingType(cursor);
			ClangCursorInfo cit(clang_getTypeDeclaration(type), pp_id);
		}
	}
/*
	{
		CXCursor ref = clang_getCursorReferenced(cursor);


		String rs = FetchString(clang_getCursorPrettyPrinted(ref, pp_id));
		if(rs.GetCount()) {
		}
	}
*/
#endif


	CXLocation loc = GetLocation(cxlocation);
	CXRange ran = GetRange(cxrange);

	SourceLocation sl;
	bool sl_loaded = false;
	auto LoadSourceLocation = [&] {
		if(sl_loaded) return;
		sl = GetSourceLocation(loc, ran);
		sl_loaded = true;
	};

	if(findarg(ci.Kind(), CXCursor_CXXMethod, CXCursor_FunctionTemplate) >= 0) {
		LoadSourceLocation();
		tfn.GetAdd(sl).cursor = cursor;
	}

	bool active_file;
	int q = do_file.Find(loc.file);
	if(q >= 0)
		active_file = do_file[q];
	else {
		LoadSourceLocation();
		active_file = WhenFile(sl.path);
		do_file.Add(loc.file, active_file);
	}

	if(WhenFile)
		LoadSourceLocation();

	if(!active_file)
		return findarg(ci.Kind(), CXCursor_FunctionTemplate, CXCursor_FunctionDecl, CXCursor_Constructor,
		                          CXCursor_Destructor, CXCursor_CXXMethod, CXCursor_UsingDeclaration, CXCursor_VarDecl, CXCursor_EnumConstantDecl,
		                          CXCursor_TypeAliasTemplateDecl, CXCursor_EnumDecl, CXCursor_ConversionFunction,
		                          CXCursor_CallExpr, CXCursor_ReturnStmt) < 0;
	
	
	CXCursor ref = clang_getCursorReferenced(cursor);

	String id = ci.Id();
	String name = ci.Name();
	String type = ci.Type();
	int kind = ci.Kind();
	
	ClangNode* np = 0;
	if (kind != CXCursor_OverloadCandidate) {
		ClangNode& n = *scope.Top();
		np = &n;
		LoadSourceLocation();
		n.kind = kind;
		if (n.kind == CXCursor_TemplateRef) {
			ClangCursorInfo tmpl_ci(ref, pp_id);
			CXSourceLocation cxlocation = clang_getCursorLocation(ref);
			CXSourceRange cxrange = clang_getCursorExtent(ref);
			CXLocation loc = GetLocation(cxlocation);
			CXRange ran = GetRange(cxrange);
			SourceLocation tmpl_sl = GetSourceLocation(loc, ran);
			n.id = tmpl_ci.Id();
			n.type = tmpl_ci.Type();
			n.type_hash = ci.TypeHash();
			ASSERT(n.type_hash);
			n.filepos_hash = tmpl_sl.GetHashValue();
		}
		else {
			if (id.GetCount())
				n.id = id;
			else if (name.GetCount())
				n.id = name;
			if (type.GetCount())
				n.type = type;
			else
				n.type = ci.TypeDeclaration();
			n.filepos_hash = sl.GetHashValue();
			n.type_hash = ci.TypeHash();
		}
		n.type_hash = ci.TypeHash();
		n.is_type_builtin = ci.TypeBuiltin();
		n.begin = sl.begin;
		n.end = sl.end;
		n.is_definition = clang_isCursorDefinition(cursor);
		if (!n.is_definition) {
			if (n.kind == CXCursor_ClassTemplate)
				n.is_definition = true;
		}
	}
	
	if (kind == CXCursor_ReturnStmt) {
		LoadSourceLocation();
		StatementItem rm;
		rm.kind = ci.Kind();
		rm.begin = sl.begin;
		rm.end = sl.end;
		info.GetAdd(sl.path).stmts.Add(rm);
	}
	if(id.GetCount()) {
		LoadSourceLocation();
		CppFileInfo& f = info.GetAdd(sl.path);
		AnnotationItem& r = locals ? f.locals.Add() : f.items.Add();
		r.kind = kind;
		r.name = ci.Name();
		if (r.kind == CXCursor_CXXMethod ||
			r.kind == CXCursor_Constructor ||
			r.kind == CXCursor_Destructor ||
			r.kind == CXCursor_ClassDecl) {
			r.parent_type = ci.ParentTypeDeclaration();
		}
		if (r.kind == CXCursor_VarDecl)
			r.type = ci.TypeDeclaration();
		else
			r.type = ci.Type();
		r.pos = loc.pos;
		r.begin = ran.p0.pos;
		r.end = ran.p1.pos;
		r.id = id;
		r.pretty0 = kind == CXCursor_MacroDefinition ? r.name
	                : FetchString(clang_getCursorPrettyPrinted(cursor, pp_pretty));
		r.pretty = kind == CXCursor_MacroDefinition ? r.name : CleanupPretty(r.pretty0);
		r.definition = clang_isCursorDefinition(cursor);
		r.nspace = ci.Nspace();
		r.bases = ci.Bases();
		r.isvirtual = kind == CXCursor_CXXMethod && clang_CXXMethod_isVirtual(cursor);
		r.isstatic = (IsFunction(r.kind) || IsVariable(r.kind)) && !IsPreprocessingCursor(cursor) && clang_Cursor_getStorageClass(cursor) == CX_SC_Static;

		if(findarg(r.kind, CXCursor_Constructor, CXCursor_Destructor) >= 0) {
			int q = r.id.Find('(');
			if(q >= 0) {
				q = r.id.ReverseFind("::", q);
				if(q >= 0)
					r.nest = r.id.Mid(0, q);
			}
		}
		else {
			static String op = "operator";
			int q = FindId(r.id, r.kind == CXCursor_ConversionFunction ? "operator" : r.name);
			if(q >= 0) {
				r.nest = r.id.Mid(0, q);
				r.nest.TrimEnd("::");
			}
			if(IsStruct(kind))
				MergeWith(r.nest, "::", r.name);
		}
		r.uname = ToUpper(r.name);
		r.unest = ToUpper(r.nest);
		ReferenceItem rm; // prevent self-references
		rm.pos = loc.pos;
		rm.id = r.id;
		ref_done.GetAdd(sl.path).FindAdd(rm);
	}

	if(!clang_Cursor_isNull(ref)) {
		LoadSourceLocation();
		auto AddRef = [&](CXCursor ref) {
			SourceLocation ref_loc = GetSourceLocation(GetLocation(clang_getCursorLocation(ref)), GetRange(clang_getCursorExtent(ref)));
			int q = -1;
			if(findarg(ci.Kind(), CXCursor_CXXBaseSpecifier, CXCursor_TemplateRef) < 0) // suppress template untyping for : WithDlgLayout<TopWindow>
				q = tfn.Find(ref_loc);
			ClangCursorInfo ref_ci(q >= 0 ? tfn[q].cursor : ref, pp_id);
			ReferenceItem rm;
			rm.kind = ci.Kind();
			rm.pos = sl.pos;
			rm.id = ref_ci.Id();
			rm.ref_pos = ref_loc.pos;
		#if 0
			DLOG("=======");
			DDUMP(sl.pos);
			DDUMP(ref_loc.pos);
			DDUMP(ref_loc.path);
			DDUMP(rm.id);
			DDUMP(ref_ci.Name());
			DDUMP(ref_ci.Kind());
			DDUMP(ref_ci.RawId());
			DDUMP(ref_ci.Type());
			DDUMP(ref_ci.Scope());
			DDUMP(ref_ci.Nspace());
		#endif
			Index<ReferenceItem>& rd = ref_done.GetAdd(ref_loc.path);
			if(rm.id.GetCount() && rd.Find(rm) < 0) {
				rd.Add(rm);
				info.GetAdd(sl.path).refs.Add(rm);
				if (np) {
					np->is_ref = true;
					np->filepos_hash = ref_loc.GetHashValue();
				}
			}
		};
		
		if(clang_getCursorKind(ref) == CXCursor_OverloadedDeclRef)
			for(unsigned int i = 0; i < clang_getNumOverloadedDecls(ref); i++)
				AddRef(clang_getOverloadedDecl(ref, i));
		else
			AddRef(ref);
	}

	if(findarg(kind, CXCursor_FunctionTemplate, CXCursor_FunctionDecl, CXCursor_Constructor,
		                  CXCursor_Destructor, CXCursor_CXXMethod, CXCursor_ConversionFunction) >= 0)
		locals = true;

	return true;
}

CXChildVisitResult clang_visitor(CXCursor cursor, CXCursor p, CXClientData clientData) {
#ifdef DUMPTREE
	LOGBEGIN();
#endif
	ClangVisitor *v = (ClangVisitor *)clientData;
	bool bak_locals = v->locals;
	ClangNode* owner = v->scope.Top();
	One<ClangNode> n;
	n.Create();
	v->scope.Add(&*n);
	bool visit = v->ProcessNode(cursor);
	if (n->kind >= 0)
		owner->sub.Add(n.Detach());
	if (visit) {
		clang_visitChildren(cursor, clang_visitor, clientData);
	}
	v->scope.Pop();
	v->locals = bak_locals;
#ifdef DUMPTREE
	LOGEND();
#endif
	return CXChildVisit_Continue;
}

void ClangVisitor::Do(CXTranslationUnit tu)
{
	scope.Clear();
	
	if(!HasLibClang())
		return;

	if(!tu) return;
	CXCursor cursor = clang_getTranslationUnitCursor(tu);
	pp_id = clang_getCursorPrintingPolicy(cursor);
	pp_pretty = clang_getCursorPrintingPolicy(cursor);
	for(int i = 0; i <= CXPrintingPolicy_LastProperty; i++) {
		clang_PrintingPolicy_setProperty(pp_id, (CXPrintingPolicyProperty)i, 0);
		clang_PrintingPolicy_setProperty(pp_pretty, (CXPrintingPolicyProperty)i, 0);
	}

	for(CXPrintingPolicyProperty p : {
			CXPrintingPolicy_SuppressSpecifiers,
			CXPrintingPolicy_SuppressTagKeyword,
			CXPrintingPolicy_SuppressUnwrittenScope,
			CXPrintingPolicy_SuppressInitializers,
			CXPrintingPolicy_SuppressStrongLifetime,
			CXPrintingPolicy_SuppressLifetimeQualifiers,
			CXPrintingPolicy_SuppressTemplateArgsInCXXConstructors,
			CXPrintingPolicy_TerseOutput,
			CXPrintingPolicy_SuppressImplicitBase,
#ifndef UBUNTU2204_WORKAROUND
			CXPrintingPolicy_FullyQualifiedName,
#endif
			CXPrintingPolicy_Bool })
		clang_PrintingPolicy_setProperty(pp_id, p, 1);

	clang_PrintingPolicy_setProperty(pp_pretty, CXPrintingPolicy_TerseOutput, 1);
	clang_PrintingPolicy_setProperty(pp_pretty, CXPrintingPolicy_Bool, 1);
	clang_PrintingPolicy_setProperty(pp_pretty, CXPrintingPolicy_SuppressScope, 1);
	initialized = true;
	ast.Clear();
	ast.kind = CXCursor_Namespace;
	scope.Add(&ast);
	clang_visitChildren(cursor, clang_visitor, this);
	scope.Clear();

	for(CppFileInfo& f : info) { // sort by line because macros are first
		Sort(f.items, [](const AnnotationItem& a, const AnnotationItem& b) {
			return CombineCompare(a.pos.y, b.pos.y)(a.pos.x, b.pos.x) < 0;
		});
		// remove duplicates
		Vector<int> toremove;
		for(int i = 1; i < f.items.GetCount(); i++) {
			AnnotationItem& a = f.items[i - 1];
			AnnotationItem& b = f.items[i];
			if(a.pos == b.pos && a.id == b.id)
				toremove.Add(i);
		}
		f.items.Remove(toremove);
	}
}

ClangVisitor::~ClangVisitor()
{
	if(!HasLibClang())
		return;

	if(initialized) {
		clang_PrintingPolicy_dispose(pp_id);
		clang_PrintingPolicy_dispose(pp_pretty);
	}
}
