#ifndef _ide_Shell_Widget_h_
#define _ide_Shell_Widget_h_

NAMESPACE_UPP


template <class T>
struct EditPosCached {
	static VectorMap<String,Value>& EditPosCache()
	{
		static VectorMap<String,Value> cache;
		return cache;
	}
	
	static void SaveEditPosCache()
	{
		static StaticMutex mtx;
		String dir = ConfigFile("cfg");
		String clsname = typeid(T).name();
		String filename = clsname + ".edit_cache";
		String path0 = AppendFileName(dir, filename);
		String path1 = path0 + ".1";
		
		mtx.Enter();
		if (FileExists(path1))
			DeleteFile(path1);
		if (FileExists(path0))
			FileMove(path0, path1);
		FileOut fout(path0);
		fout % EditPosCache();
		mtx.Leave();
	}
	
	static void LoadEditPosCache()
	{
		String dir = ConfigFile("cfg");
		String clsname = typeid(T).name();
		String filename = clsname + ".edit_cache";
		String path0 = AppendFileName(dir, filename);
		String path1 = path0 + ".1";
		bool p0 = FileExists(path0);
		bool p1 = FileExists(path1);
		
		// Check for invalid file
		String load_path;
		if (p0 && p1) {
			FileIn fin(path0);
			if (fin.GetSize() == 0)
				load_path = path1;
			else
				load_path = path0;
		}
		else if (p0)
			load_path = path0;
		else if (p1)
			load_path = path1;
		
		if (!load_path.IsEmpty()) {
			FileIn fin(load_path);
			fin % EditPosCache();
		}
	}
};

END_UPP_NAMESPACE

#endif
