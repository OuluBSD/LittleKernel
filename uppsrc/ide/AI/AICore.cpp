#include "AI.h"

NAMESPACE_UPP


String GetAiPathCandidate(const String& includes_, String dir)
{
	Vector<String> ai_dirs = GetAiDirsRaw();
	Vector<String> upp_dirs = GetUppDirs();
	String dummy_cand, def_cand, any_ai_cand, preferred_ai_cand;
	int def_cand_parts = INT_MAX;
	String filename = META_FILENAME;
	dummy_cand = dir + DIR_SEPS + filename;
	
	if(!ai_dirs.IsEmpty()) {
		for(const String& upp_dir : upp_dirs) {
			if(dir.Find(upp_dir) != 0)
				continue;
			String rel_path = dir.Mid(upp_dir.GetCount());
			for(const String& ai_dir : ai_dirs) {
				String ai_dir_cand = AppendFileName(ai_dir, rel_path);
				String path = AppendFileName(ai_dir_cand, filename);
				if(any_ai_cand.IsEmpty())
					any_ai_cand = path;
				if(preferred_ai_cand.IsEmpty()) {
					if (META_EXISTS_FN(path))
						preferred_ai_cand = path;
				}
			}
		}
	}
	if(!preferred_ai_cand.IsEmpty())
		return preferred_ai_cand;
	else if(!any_ai_cand.IsEmpty())
		return any_ai_cand;

	if(!ai_dirs.IsEmpty()) {
		String ai_dir, rel_dir;
		if(MakeRelativePath(includes_, dir, ai_dir, rel_dir)) {
			String abs_dir = AppendFileName(ai_dir, rel_dir);
			def_cand = AppendFileName(abs_dir, filename);
		}
	}

	if(!def_cand.IsEmpty())
		return def_cand;
	else
		return dummy_cand;
}





END_UPP_NAMESPACE
