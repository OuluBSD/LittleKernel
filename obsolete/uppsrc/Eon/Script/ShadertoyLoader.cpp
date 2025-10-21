#include "Script.h"

NAMESPACE_UPP


ShadertoyContextLoader::ShadertoyContextLoader() {
	
}

void ShadertoyContextLoader::Clear() {
	id_counter = 0;
}

void ShadertoyContextLoader::OnError(TypeCls type, String fn, String msg) {
	last_error = msg;
	WhenError();
}

void ShadertoyContextLoader::OnError(String fn, String msg) {
	LOG("ShadertoyContextLoader::" << fn << ": error: " << msg);
	last_error = msg;
	WhenError();
}

bool ShadertoyContextLoader::Load(String path, Value& o) {
	LOG("ShadertoyContextLoader::Load: " << path);
	
	String ext = GetFileExt(path);
	if (ext == ".toy")
		return LoadFileToy(path, o);
	
	return false;
}

bool ShadertoyContextLoader::LoadFileToy(String path, Value& dst) {
	const char* fn_name = "LoadFileToy";
	Clear();
	
	if (!FileExists(path)) {
		String share_path = ShareDirFile(path);
		if (FileExists(share_path))
			path = share_path;
	}
	
	if (!FileExists(path)) {
		OnError(fn_name, "file doesn't exist: " + path);
		return false;
	}
	
	String file_dir = GetFileDirectory(path);
	bool fail = false;
	dst = ParseJSON(LoadFile(path), false);
	if (dst.IsError()) {
		RLOG("ShadertoyContextLoader::LoadFileToy: error: " + GetErrorText(dst));
		return false;
	}
	
	ASSERT(dst.Is<ValueMap>());
	ValueMap map = dst;
	for(int i = 0; i < 100; i++) {
		String stage = "stage" + IntStr(i);
		String stage_file = AppendFileName(file_dir, stage + ".glsl");
		if (!FileExists(stage_file))
			break;
		String glsl = LoadFile(stage_file);
		if (glsl.IsEmpty()) {
			fail = true;
			last_error = "empty shader for stage " + IntStr(i);
		}
		else {
			map.Add(stage + "_path", stage_file);
			map.Add(stage + "_content", glsl);
		}
	}
	dst = map;
	
	if (fail) {
		OnError(fn_name, last_error);
		return false;
	}
	
	LOG(AsJSON(dst, true));
	MakeUniqueIds(dst);
	
	return true;
}

void ShadertoyContextLoader::MakeUniqueIds(Value& v) {
	if (!v.Is<ValueMap>())
		return;
	//DLOG(GetValueTreeString(v));
	ValueMap map = v;
	Value stages = map.GetAdd("stages");
	if (!stages.Is<ValueArray>())
		return;
	VectorMap<int,int> ids;
	ValueArray st_arr = stages;
	VectorMap<int, Value> stage_ids;
	for(int i = 0; i < st_arr.GetCount(); i++) {
		int stage_id = -1;
		Value st_el = st_arr.Get(i);
		if (st_el.Is<ValueMap>()) {
			ValueMap st_map = st_el;
			Value inputs = st_map.GetAdd("inputs");
			if (inputs.Is<ValueArray>()) {
				ValueArray in_arr = inputs;
				for(int i = 0; i < in_arr.GetCount(); i++) {
					Value in_el = in_arr.Get(i);
					if (!in_el.Is<ValueMap>())
						continue;
					ValueMap in_map = in_el;
					int j = in_map.Find("id");
					if (j >= 0)
						in_map.SetAt(j, MakeUniqueId(ids, (int)in_map.GetValue(j)));
				}
			}
			
			Value outputs = st_map.GetAdd("outputs");
			if (outputs.Is<ValueArray>()) {
				ValueArray out_arr = outputs;
				for(int i = 0; i < out_arr.GetCount(); i++) {
					Value out_el = out_arr.Get(i);
					if (!out_el.Is<ValueMap>())
						continue;
					ValueMap out_map = out_el;
					int j = out_map.Find("id");
					if (j >= 0) {
						int id = MakeUniqueId(ids, (int)out_map.GetValue(j));
						if (!i)
							stage_id = id;
						out_map.SetAt(j, id);
					}
				}
				if (out_arr.IsEmpty()) {
					ValueMap map;
					stage_id = MakeUniqueId(ids, 10000 + i);
					map.Add("id", stage_id);
					out_arr.Add(map);
				}
			}
		}
		
		if (stage_id < 0)
			stage_id = MakeUniqueId(ids, 10000 + i);
		stage_ids.Add(stage_id);
	}
	
	if (0) {
		for(int i = 0; i < stage_ids.GetCount(); i++) {
			int st_pos = map.Find("stage" + IntStr(i) + "_content");
			ASSERT(st_pos >= 0);
			if (st_pos >= 0) {
				stage_ids[i] = map.GetValue(st_pos);
				map.Remove(st_pos);
			}
		}
	}
	
	if (0) {
		LOG("ShadertoyContextLoader::MakeUniqueIds: result");
		//DLOG(GetValueTreeString(v));
		for(int i = 0; i < stage_ids.GetCount(); i++) {
			LOG("\t" << stage_ids.GetKey(i) << ": <source>");
		}
	}
	
	if (0) {
		for(int i = 0; i < stage_ids.GetCount(); i++) {
			int id = stage_ids.GetKey(i);
			map.Add(IntStr(id), stage_ids[i]);
		}
	}
}

int ShadertoyContextLoader::MakeUniqueId(VectorMap<int,int>& ids, int orig_id) {
	if (orig_id < 0)
		return -1;
	int i = ids.Find(orig_id);
	if (i >= 0)
		return ids[i];
	int id = ++id_counter;
	ids.Add(orig_id, id);
	return id;
}













String SerialShadertoyLoader::LoadFile(String file_path) {
	ShadertoyContextLoader toy_loader;
	Value o;
	if (!toy_loader.Load(file_path, o)) {
		LOG("SerialShadertoyLoader::LoadFile: error: toy file loading failed: " << file_path);
		return String();
	}
	
	Eon::ToyLoader ser_loader;
	if (!ser_loader.Load(o)) {
		LOG("SerialShadertoyLoader::LoadFile: error: toy object loading failed");
		return String();
	}
	
	String eon_script = ser_loader.GetResult();
	
	return eon_script;
}


END_UPP_NAMESPACE
