#include "Vfs.h"

NAMESPACE_UPP

MetaCodeGenerator::MetaCodeGenerator() {
	
}

String MetaCodeGenerator::GetResult(int pkg, int file) const {
	PkgFile key(pkg,file);
	int i = files.Find(key);
	if (i >= 0) return files[i].code;
	return "";
}

const MetaCodeGenerator::File* MetaCodeGenerator::GetResultFile(int pkg, int file) const {
	PkgFile key(pkg,file);
	int i = files.Find(key);
	if (i >= 0) return &files[i];
	return 0;
}

bool MetaCodeGenerator::Process(const VfsValueSubset& np) {
	VfsValue& n = *np.n;
	ASSERT(!n.only_temporary);
	
	IdeMetaEnvironment& ienv = IdeMetaEnv();
	MetaEnvironment& env = ienv.env;
	files.Clear();
	
	// Find all unique files
	VectorMap<hash_t,VectorMap<hash_t,int>> pkgfiles;
	FindFiles(np, pkgfiles);
	
	// Find most visible file
	VectorMap<PkgFile,int> pkgfile_counts;
	int pkg_i = 0;
	for (auto& pkg : pkgfiles) {
		int file_i = 0;
		for (auto& file_count : pkg) {
			if (file_count > 0)
				pkgfile_counts.Add(PkgFile(pkg_i, file_i), file_count);
			file_i++;
		}
		pkg_i++;
	}
	SortByValue(pkgfile_counts, StdGreater<int>());
	
	// Create file headers for files which were found
	for(int i = 0; i < pkgfile_counts.GetCount(); i++)
		files.Add(pkgfile_counts.GetKey(i));
	
	// Create file contents
	for(int i = 0; i < files.GetCount(); i++) {
		const PkgFile& key = files.GetKey(i);
		File& f = files[i];
		
		// Find all parent nodes of the file
		Vector<VfsValue*> nodes;
		FindValues(np, key, nodes);
		
		// Sort nodes based on code position
		struct VfsValuePtrSorter {
			bool operator()(const VfsValue* a, const VfsValue* b) const {
				const AstValue* av = *a;
				const AstValue* bv = *b;
				ASSERT(av && bv);
				if (av && bv)
					return IsFilePosLess(av->begin, bv->begin);
				return false;
			}
		};
		Sort(nodes, VfsValuePtrSorter());
		
		// Remove overlapping nodes
		for(int j = 0; j < nodes.GetCount(); j++) {
			const VfsValue& n0 = *nodes[j];
			const AstValue* a0 = n0;
			if (!a0)
				continue;
			for(int k = j+1; k < nodes.GetCount(); k++) {
				const VfsValue& n1 = *nodes[k];
				const AstValue* a1 = n1;
				if (a1 && IsRangesOverlapping(a0->begin, a0->end, a1->begin, a1->end)) {
					if (IsSubset(a0->begin, a0->end, a1->begin, a1->end))
						nodes.Remove(k--);
					else {
						nodes.Remove(j--);
						break;
					}
				}
			}
		}
		
		// Load original file
		const VfsSrcPkg& pkg = ienv.GetAddPackage(key.pkg_hash);
		ASSERT(pkg.dir.GetCount());
		String path = pkg.GetFullPath(key.file_hash);
		String content = LoadFile(path);
		
		// Write code
		VectorMap<int,String> lines;
		Vector<VfsValue*> comments;
		Vector<VfsValue*> comment_to_node;
		for (VfsValue* n : nodes) {
			AstValue* a = *n;
			ASSERT(a);
			if (!a) continue;
			
			// TODO namespaces
			
			TextRange range;
			range.begin = a->begin;
			range.end = a->end;
			//range.begin = Point(0,lines.GetCount());
			Vector<String> area_lines = GetStringArea(content, a->begin, a->end);
			for(int i = 0; i < area_lines.GetCount(); i++)
				lines.Add(a->begin.y + i, area_lines[i]);
			//range.end = Point(0,lines.GetCount());
			
			// Add empty line
			if (lines.GetCount())
				lines.Add(lines.TopKey());
			
			n->AstFindAllDeep(METAKIND_COMMENT, comments);
			
			f.code_nodes.Add(range, const_cast<VfsValue*>(n));
		}
		
		// Add comments
		for (VfsValue* comment : comments) {
			AstValue* a = *comment;
			ASSERT(a);
			if (!a) continue;
			
			int line0 = a->begin.y;
			int best_pos = 0, best_line = 0;
			for(int i = 0; i < lines.GetCount(); i++) {
				int line1 = lines.GetKey(i);
				if (line1 > line0)
					break;
				best_pos = i;
				best_line = line1;
			}
			lines.Insert(best_pos, best_line, "// " + comment->id);
			
			if (best_pos >= comment_to_node.GetCount())
				comment_to_node.SetCount(best_pos+1,0);
			comment_to_node[best_pos] = comment;
		}
		
		// Write
		f.code = Join(lines.GetValues(), "\n");
		for (const VfsValue* n : nodes) {
			const AstValue* a = *n;
			ASSERT(a);
			if (!a) continue;
			
			f.range_nodes.Add(TextRange(a->begin,a->end), const_cast<VfsValue*>(n));
		}
		f.editor_to_line <<= lines.GetKeys();
		f.comment_to_node <<= comment_to_node;
	}
	
	return true;
}

void MetaCodeGenerator::FindFiles(const VfsValueSubset& np, VectorMap<hash_t, VectorMap<hash_t,int>>& pkgfiles) {
	const VfsValue& n = *np.n;
	if (n.pkg_hash != 0 && n.file_hash != 0) {
		VectorMap<hash_t,int>& file = pkgfiles.GetAdd(n.pkg_hash);
		file.GetAdd(n.file_hash,0)++;
	}
	for (const VfsValueSubset& s : np.sub)
		FindFiles(s, pkgfiles);
}

void MetaCodeGenerator::FindValues(const VfsValueSubset& np, const PkgFile& key, Vector<VfsValue*>& nodes) {
	VfsValue& n = *np.n;
	const AstValue* a = n;
	if (a && IsMergeable(a->kind)) {
		for (const VfsValueSubset& s : np.sub) {
			FindValues(s, key, nodes);
		}
	}
	else {
		if (n.pkg_hash == key.pkg_hash && n.file_hash == key.file_hash)
			nodes.Add(&n);
	}
}

END_UPP_NAMESPACE
