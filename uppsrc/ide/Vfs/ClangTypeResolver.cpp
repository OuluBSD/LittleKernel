#include "Vfs.h"


NAMESPACE_UPP

ClangTypeResolver::ClangTypeResolver() {
	
}
	
bool ClangTypeResolver::Process(ClangNode& cn) {
	MetaEnvironment& env = MetaEnv();
	
	type_translation.Clear();
	type_nodes.Clear();
	scope.Clear();
	
	if (!Visit(cn))
		return false;
	
	if (0) {
		DumpFoundTypes();
		DumpFoundScopePaths();
		DumpUnresolvedHashes();
	}
	
	return true;
}

void ClangTypeResolver::DumpFoundTypes() {
	LOG("---- Dumping found types ----");
	for(int i = 0; i < type_nodes.GetCount(); i++) {
		auto& v = type_nodes[i];
		LOG(Format("[%d]: Type-nodes for hash %X: count %d", i, (int64)type_nodes.GetKey(i), v.GetCount()));
		for(int j = 0; j < v.GetCount(); j++) {
			ClangNode& n = *v[j];
			LOG(Format("\t[%d][%d]: %s: %s(%s)", i, j, VfsValue::AstGetKindString(n.kind), n.id, n.type));
		}
	}
}

void ClangTypeResolver::DumpFoundScopePaths() {
	LOG("---- Dumping found scope paths ----");
	for(int i = 0; i < type_scopepaths.GetCount(); i++) {
		auto& v = type_scopepaths[i];
		LOG(Format("[%d]: Type-paths for hash %X: count %d", i, (int64)type_scopepaths.GetKey(i), v.GetCount()));
		for(int j = 0; j < v.GetCount(); j++) {
			const String& str = v[j];
			LOG(Format("\t[%d][%d]: %s", i, j, str));
		}
	}
}

void ClangTypeResolver::DumpUnresolvedHashes() {
	LOG("---- Dumping unresolved hashes ----");
	for(int i = 0; i < type_nodes.GetCount(); i++) {
		auto hash = type_nodes.GetKey(i);
		int j = type_scopepaths.Find(hash);
		if (j < 0) {
			auto& v = type_nodes[i];
			LOG(Format("[%d]: error: unknown type: hash %X: count %d", i, (int64)type_nodes.GetKey(i), v.GetCount()));
			for(int j = 0; j < v.GetCount(); j++) {
				ClangNode& n = *v[j];
				LOG(Format("\t[%d][%d]: %s: %s(%s)", i, j, VfsValue::AstGetKindString(n.kind), n.id, n.type));
			}
		}
	}
}

bool ClangTypeResolver::Visit(ClangNode& cn) {
	if (cn.type_hash) {
		bool is_tmpl = false, is_typeref = false;
		for (auto& s : cn.sub) {
			if (s.kind == CXCursor_TemplateRef) {
				is_tmpl = true;
			}
			else if (s.kind == CXCursor_TypeRef) {
				is_typeref = true;
			}
		}
		
		hash_t used_hash = cn.type_hash;
		type_nodes.GetAdd(used_hash).Add(&cn);
		
		if (is_tmpl) {
			String path = GetTemplateScopePath(cn);
			if (path.IsEmpty())
				return false;
			type_scopepaths.GetAdd(used_hash).FindAdd(path);
		}
		else if (IsTypeDecl(cn.kind) || cn.kind == CXCursor_MacroDefinition) {
			String path = GetScopePath(cn);
			type_scopepaths.GetAdd(used_hash).FindAdd(path);
		}
		else if (cn.is_type_builtin && !is_typeref && cn.type.GetCount()) {
			String t = cn.type;
			if (t.Find("const char[") == 0) {
				int a = t.Find("[");
				int b = t.Find("]", a);
				ASSERT(b >= 0);
				t = t.Left(a+1) + t.Mid(b);
			}
			String path = "/builtin/" + t;
			type_scopepaths.GetAdd(used_hash).FindAdd(path);
		}
	}
	
	if (!VisitSub(cn))
		return false;
	
	return true;
}

bool ClangTypeResolver::VisitSub(ClangNode& cn) {
	Push(cn);
	bool succ = true;
	for (ClangNode& s : cn.sub) {
		succ = Visit(s);
		if (!succ)
			break;
	}
	Pop();
	return succ;
}

String ClangTypeResolver::GetScopePath(ClangNode& cn) const {
	if (cn.kind == CXCursor_MacroExpansion || cn.kind == CXCursor_MacroDefinition)
		return "$" + cn.id;
	
	String str;
	for (ClangNode* s : scope)
		str << "/" << s->id;
	str << "/" << cn.id;
	if (cn.kind == CXCursor_ClassTemplate) {
		int args = 0;
		str << "<";
		for (const auto& s : cn.sub) {
			if (s.kind == CXCursor_TemplateTypeParameter) {
				if (args) str << ",";
				//str << s.id;
				str << "A" << IntStr(args);
				args++;
			}
			else if (s.kind == CXCursor_TemplateTemplateParameter) {
				if (args++) str << ",";
				str << s.id;
			}
		}
		str << ">";
	}
	return str;
}

String ClangTypeResolver::GetTemplateScopePath(ClangNode& cn) {
	String str;
	int args = 0;
	for (const auto& s : cn.sub) {
		/*if (s.kind == CXCursor_TemplateRef)*/ {
			int i = this->type_scopepaths.Find(s.type_hash);
			if (i < 0) {
				SetError("Can't find template path for " + s.id);
				return String();
			}
			String path = type_scopepaths[i][0];
			if (args++) str << ",";
			str << path;
		}
		/*else {
			if (args++) str << ",";
			str << s.id;
		}*/
	}
	return str;
}

bool MetaEnvironment::MergeResolver(ClangTypeResolver& ctr)
{
	const VectorMap<hash_t, Index<String>>& scope_paths = ctr.GetScopePaths();
	auto& translation = ctr.GetTypeTranslation();

	for(int i = 0; i < scope_paths.GetCount(); i++) {
		hash_t src_hash = scope_paths.GetKey(i);
		const Index<String>& idx = scope_paths[i];
		String path = idx[0];
		hash_t dst_hash = RealizeTypePath(path);
		translation.GetAdd(src_hash, dst_hash);
	}

	return true;
}

END_UPP_NAMESPACE
