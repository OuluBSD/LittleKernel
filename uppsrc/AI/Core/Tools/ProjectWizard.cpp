#include "Tools.h"


NAMESPACE_UPP


String ParseItemArg(String path) {
	ASSERT(!path.IsEmpty());
	int a = path.Find("[");
	if (a >= 0) {
		a++;
		int b = path.ReverseFind("]");
		if (b >= 0)
			return path.Mid(a,b-a);
	}
	return "";
}


String FileNode::GetFilePath() const {
	ASSERT(!path.IsEmpty());
	int a = path.Find(":");
	if (a >= 0)
		return path.Left(a);
	return path;
}

String FileNode::GetItemPath() const {
	return path;
}

String FileNode::GetItemArg() const {
	ASSERT(!path.IsEmpty());
	int a = path.Find("[");
	if (a >= 0) {
		a++;
		int b = path.ReverseFind("]");
		if (b >= 0)
			return path.Mid(a,b-a);
	}
	return "";
}

String FileNode::GetAnyUserInputString() const {
	for(const auto& o : conf.options)
		if (o.type == ConfigurationOption::USER_INPUT_TEXT)
			return o.value;
	return "";
}

String FileNode::GetAnyUserPromptInputString() const {
	for(const auto& o : conf.options)
		if (o.type == ConfigurationOption::PROMPT_INPUT_USER_TEXT)
			return o.value;
	return "";
}

ConfigurationNode& ConfigurationNode::SkipValue() {
	skip_value = true;
	return *this;
}

ConfigurationNode& ConfigurationNode::DefaultReadOptions() {
	read_options = true;
	return *this;
}

ConfigurationNode& ConfigurationNode::OptionFixed(Value v) {
	ConfigurationOption& opt = options.Add();
	opt.type = ConfigurationOption::FIXED;
	opt.value = v;
	return *this;
}

ConfigurationNode& ConfigurationNode::OptionButton(Value v, void(ProjectWizardView::*fn)(const FileNode* n), bool is_refresh) {
	ConfigurationOption& opt = options.Add();
	opt.type = is_refresh ? ConfigurationOption::BUTTON_REFRESH : ConfigurationOption::BUTTON;
	opt.value = v;
	opt.fn = fn;
	return *this;
}

ConfigurationNode& ConfigurationNode::OptionRefresh() {
	for(const auto& o : options)
		if (o.type == ConfigurationOption::BUTTON_REFRESH)
			return *this;
	
	ConfigurationOption& opt = options.Add();
	opt.type = ConfigurationOption::BUTTON_REFRESH;
	opt.value = "Refresh";
	opt.fn = &ProjectWizardView::DefaultDynamic;
	return *this;
}

ConfigurationNode& ConfigurationNode::OptionValueArray() {
	ConfigurationOption& opt = options.Add();
	opt.type = ConfigurationOption::VALUE_ARRAY;
	return *this;
}

ConfigurationNode& ConfigurationNode::OptionUserInputText() {
	ConfigurationOption& opt = options.Add();
	opt.type = ConfigurationOption::USER_INPUT_TEXT;
	return *this;
}

ConfigurationNode& ConfigurationNode::PromptInput(String path) {
	ConfigurationOption& opt = options.Add();
	opt.type = ConfigurationOption::PROMPT_INPUT;
	opt.value = path;
	return *this;
}

ConfigurationNode& ConfigurationNode::PromptInputAllPrevious() {
	int a = path.Find(":");
	ASSERT(a >= 0);
	String file = path.Left(a+1);
	int c = file.GetCount();
	auto& confs = ProjectWizardView::GetConfs();
	int pos = confs.Find(this->path);
	ASSERT(pos >= 0);
	for(int i = 0; i < pos; i++) {
		String conf_path = confs.GetKey(i);
		if (conf_path.GetCount() < c)
			continue;
		if (strncmp(conf_path.Begin(), file.Begin(), c) == 0) {
			ConfigurationOption& opt = options.Add();
			opt.type = ConfigurationOption::PROMPT_INPUT;
			opt.value = confs.GetKey(i);
		}
	}
	return *this;
}

ConfigurationNode& ConfigurationNode::PromptInputOptionsLocalFixed() {
	ConfigurationOption& opt = options.Add();
	opt.type = ConfigurationOption::PROMPT_INPUT_OPTIONS_FIXED;
	opt.value = this->path;
	return *this;
}

ConfigurationNode& ConfigurationNode::PromptInputOptions(String path) {
	ConfigurationOption& opt = options.Add();
	opt.type = ConfigurationOption::PROMPT_INPUT_OPTIONS;
	opt.value = path;
	return *this;
}

ConfigurationNode& ConfigurationNode::PromptResponse(String s, bool have_refresh) {
	ConfigurationOption& opt = options.Add();
	opt.type = ConfigurationOption::PROMPT_RESPONSE;
	opt.value = s;
	if (have_refresh)
		OptionRefresh();
	return *this;
}

ConfigurationNode& ConfigurationNode::PromptInputUserText(String title) {
	ConfigurationOption& opt = options.Add();
	opt.type = ConfigurationOption::PROMPT_INPUT_USER_TEXT;
	opt.value = title;
	return *this;
}





void ProjectWizardView::Data() {
	
}

ConfigurationNode& ProjectWizardView::Register(String path, String title) {
	ConfigurationNode& n = GetConfs().GetAdd(path);
	n.path = path;
	if (title.IsEmpty()) {
		int a = path.Find(":");
		if (a >= 0)
			title = path.Mid(a+1);
	}
	n.title = title;
	return n;
}

ConfigurationNode& ProjectWizardView::RegisterDynamic(String path, String title) {
	ConfigurationNode& n = Register(path, title);
	n.is_dynamic = true;
	return n;
}

void ProjectWizardView::DefaultDynamicPath(String path) {
	FileNode& n = RealizeFileNode(path);
	DefaultDynamic(&n);
}

void ProjectWizardView::DefaultDynamic(const FileNode* n) {
	LOG("ProjectWizardView::DefaultDynamic: \"" << n->path << "\"");
	
	String path = n->path;
	ValueArray& opts = GetItemOpts(path);
	opts.Clear();
	
	if (OnlyReadyButton(*n)) {
		PressReadyButton(*n);
		WhenCallbackReady();
		WhenFile();
		return;
	}
	
	GenericPromptArgs args;
	if (!MakeArgs(args, *n)) {
		LOG(error);
		return;
	}
	
	TaskMgr& m = AiTaskManager();
	m.GetGenericPrompt(args, [this, path](String res) {
		ValueArray& opts = GetItemOpts(path);
		//LOG(res);
		
		res.Replace("\r", "");
		int a = res.Find("\n\n");
		if (a >= 0)
			res = res.Left(a);
		
		if (res.Find("\n- 2.") >= 0 || res.Find("\n- 2)") >= 0) {
			RemoveEmptyLines3(res);
			RemoveEmptyLines2(res);
		}
		else if (res.Find("\n-") >= 0)
			RemoveEmptyLines3(res);
		else
			RemoveEmptyLines2(res);
		
		Vector<String> lines = Split(res, "\n");
		for(int i = 0; i < lines.GetCount(); i++) {
			String& s = lines[i];
			int a = s.Find("\"");
			int b = s.ReverseFind("\"");
			if (a >= 0 && b >= 0 && a < b) {
				Vector<String> parts = Split(s, "\"");
				if (parts.GetCount() == 1)
					RemoveQuotes(s);
			}
			s = TrimBoth(s);
			opts.Add(s);
		}
		
		if (lines.GetCount()) {
			Value& val = GetItemValue(path);
			String old_value = val.ToString();
			/*if (old_value.IsEmpty())*/ {
				val = lines[0];
				WhenCallbackReady();
				WhenFile();
				return;
			}
		}
		
		WhenCallbackReady();
		WhenOptions();
	});
}

void ProjectWizardView::ClearAllDynamic(const FileNode* n) {
	String file_path = n->GetFilePath();
	const auto& file = GetFile(file_path);
	ValueMap new_map;
	for(int i = 0; i < file.GetCount(); i++) {
		String key = file.GetKey(i);
		if (key.GetCount()) {
			if (key[key.GetCount()-1] == ']') {
				int a = key.Find("[");
				if (a >= 0)
					continue;
			}
		}
		const Value& v = file.GetValue(i);
		new_map.Add(key, v);
	}
	Value& file_val = GetFileValue(file_path);
	file_val = new_map;
	WhenFile();
}

void ProjectWizardView::SplitComponents(const FileNode* n) {
	SplitItems(n, "Components");
}

#if 0
void ProjectWizardView::SplitAllSubComponents(const FileNode* n) {
	const auto& confs = ProjectWizardView::GetConfs();
	String file_path = n->GetFilePath();
	ValueArray& arr0 = GetItemOpts(n->path);
	for(int i = 0; i < arr0.GetCount(); i++) {
		String s0 =  arr0[i].ToString();
		RemoveColonTrail(s0);
		s0 = TrimBoth(s0);
		for (int k = 0; k < 2; k++) {
			String sub_item0 = "Components[" + s0 + "]";
			String item_path0 = file_path + ":" + sub_item0;
			ValueArray& arr1 = GetItemOpts(item_path0);
			for(int j = 0; j < arr1.GetCount(); j++) {
				String s1 = arr1[j];
				RemoveColonTrail(s1);
				s1 = TrimBoth(s1);
				String key = k == 0 ? "Classes" : "Tasks";
				String sub_item1 = key + "[" + s1 + "]";
				String item_path1 = file_path + ":" + sub_item1;
				const FileNode& n0 = RealizeFileNode(item_path1);
				String lbl_str = n0.GetAnyUserPromptInputString();
				ASSERT(lbl_str.GetCount()); // rule requires PromptInputUserText
				ValueMap& map0 = GetItem(item_path1);
				map0.GetAdd("src-path") = item_path0;
				Value& user_input = map0.GetAdd(lbl_str);
				user_input = s1;
			}
		}
	}
	WhenFile();
}
#endif

void ProjectWizardView::SplitSubComponents(const FileNode* n) {
	const auto& confs = ProjectWizardView::GetConfs();
	String file_path = n->GetFilePath();
	ValueArray& arr = GetItemOpts(n->path);
	
	for(int i = 0; i < arr.GetCount(); i++) {
		String s =  arr[i].ToString();
		RemoveColonTrail(s);
		s = TrimBoth(s);
		{
			String sub_item = "Tasks[" + s + "]";
			String item_path = file_path + ":" + sub_item;
			const FileNode& n0 = RealizeFileNode(item_path);
			String lbl_str = n0.GetAnyUserPromptInputString();
			ASSERT(lbl_str.GetCount()); // rule requires PromptInputUserText
			ValueMap& map0 = GetItem(item_path);
			map0.GetAdd("src-path") = n->GetItemPath();
			Value& user_input = map0.GetAdd(lbl_str);
			user_input = s;
		}
		{
			String sub_item = "Classes[" + s + "]";
			String item_path = file_path + ":" + sub_item;
			const FileNode& n0 = RealizeFileNode(item_path);
			String lbl_str = n0.GetAnyUserPromptInputString();
			ASSERT(lbl_str.GetCount()); // rule requires PromptInputUserText
			ValueMap& map0 = GetItem(item_path);
			map0.GetAdd("src-path") = n->GetItemPath();
			Value& user_input = map0.GetAdd(lbl_str);
			user_input = s;
		}
	}
	WhenFile();
}

void ProjectWizardView::SplitDependencies(const FileNode* n) {
	const auto& confs = ProjectWizardView::GetConfs();
	String file_path = n->GetFilePath();
	ValueArray& arr0 = GetItemOpts("/Plan/Client program:Unique components");
	for(int i = 0; i < arr0.GetCount(); i++) {
		String s0 =  arr0[i].ToString();
		RemoveColonTrail(s0);
		s0 = TrimBoth(s0);
		for (int k = 0; k < 2; k++) {
			String sub_item0 = "UniqueComponents[" + s0 + "]";
			String item_path0 = "/Plan/Client program:" + sub_item0;
			ValueArray& arr1 = GetItemOpts(item_path0);
			for(int j = 0; j < arr1.GetCount(); j++) {
				String s1 = arr1[j];
				RemoveColonTrail(s1);
				s1 = TrimBoth(s1);
				String key = k == 1 ? "Classes" : "Tasks";
				String sub_item1 = key + "[" + s1 + "]";
				String item_path1 = "/Plan/Client program:" + sub_item1;
				String item_path2 = file_path + ":" + sub_item1;
				const FileNode& n0 = RealizeFileNode(item_path2);
				String lbl_str = n0.GetAnyUserPromptInputString();
				ASSERT(lbl_str.GetCount()); // rule requires PromptInputUserText
				ValueMap& map0 = GetItem(item_path2);
				map0.GetAdd("src-path") = item_path1;
				Value& user_input = map0.GetAdd(lbl_str);
				user_input = s1;
			}
		}
	}
	WhenFile();
}

void ProjectWizardView::SplitItems(const FileNode* n, String key) {
	const auto& confs = ProjectWizardView::GetConfs();
	String file_path = n->GetFilePath();
	ValueArray& arr = GetItemOpts(n->path);
	for(int i = 0; i < arr.GetCount(); i++) {
		String s =  arr[i].ToString();
		RemoveColonTrail(s);
		s = TrimBoth(s);
		String sub_item = key + "[" + s + "]";
		String item_path = file_path + ":" + sub_item;
		const FileNode& n0 = RealizeFileNode(item_path);
		String lbl_str = n0.GetAnyUserPromptInputString();
		ASSERT(lbl_str.GetCount()); // rule requires PromptInputUserText
		ValueMap& map0 = GetItem(item_path);
		map0.GetAdd("src-path") = n->GetItemPath();
		Value& user_input = map0.GetAdd(lbl_str);
		user_input = s;
	}
	WhenFile();
}

void ProjectWizardView::SplitTechnologyCategories(const FileNode* n) {
	SplitItems(n, "Libraries");
}

void ProjectWizardView::ClearAssembly(const FileNode* n) {
	TODO
	#if 0
	Node& root = RealizeNode("/assembly");
	root.sub.Clear();
	root.data.Clear();
	#endif
	
}

void ProjectWizardView::SplitPackages(const FileNode* n) {
	TODO
	#if 0
	const auto& confs = ProjectWizardView::GetConfs();
	String file_path = n->GetFilePath();
	ValueArray& arr = GetItemOpts("/Meta/Headers:Packages of the app");
	Vector<String> keys;
	keys << "Package includes";
	keys << "Virtual package";
	
	//keys << "External virtual modules";
	for (String key : keys) {
		VectorMap<String,int> seen_pkgs;
		for(int i = 0; i < arr.GetCount(); i++) {
			String s =  arr[i].ToString();
			RemoveCommentTrail(s);
			RemoveColonTrail(s);
			s = TrimBoth(s);
			int a = s.Find("->");
			Vector<String> deps;
			if (a >= 0) {
				deps = Split(s.Mid(a+2), ",");
				for (String& s : deps) s = TrimBoth(s);
				s = TrimBoth(s.Left(a));
			}
			seen_pkgs.GetAdd(s,0)++;
			String sub_item = key + "[" + s + "]";
			String item_path = file_path + ":" + sub_item;
			const FileNode& n0 = RealizeFileNode(item_path);
			String lbl_str = n0.GetAnyUserPromptInputString();
			ASSERT(lbl_str.GetCount()); // rule requires PromptInputUserText
			ValueMap& map0 = GetItem(item_path);
			map0.GetAdd("src-path") = n->GetItemPath();
			Value& user_input = map0.GetAdd(lbl_str);
			user_input = s;
			
			Node& pkg = RealizeNode("/assembly/" + s, NODE_PACKAGE);
			ValueArray& pkg_deps = ValueToArray(pkg.data.GetAdd("deps"));
			pkg_deps.Clear();
			for (String& dep : deps) {
				pkg_deps.Add(dep);
				seen_pkgs.GetAdd(dep,0);
			}
		}
		for(int i = 0; i < seen_pkgs.GetCount(); i++) {
			if (seen_pkgs[i] > 0) continue;
			String s = seen_pkgs.GetKey(i);
			
			String sub_item = key + "[" + s + "]";
			String item_path = file_path + ":" + sub_item;
			const FileNode& n0 = RealizeFileNode(item_path);
			String lbl_str = n0.GetAnyUserPromptInputString();
			ASSERT(lbl_str.GetCount()); // rule requires PromptInputUserText
			ValueMap& map0 = GetItem(item_path);
			map0.GetAdd("src-path") = n->GetItemPath();
			Value& user_input = map0.GetAdd(lbl_str);
			user_input = s;
		}
	}
	WhenTree();
	#endif
}

void ProjectWizardView::ParseVirtualPackageData(const FileNode* n) {
	TODO
	#if 0
	Vector<String> items = MakeItems("/Meta/Headers");
	Node& root = RealizeNode("/assembly");
	
	String main_pkg_name = StringToName(root.owner->name);
	String exporter = "UppExporter";
	Node& prj_file = root.GetAddNode(exporter, NODE_EXPORTER);
	String dir = GetHomeDirFile("MyAssembly");
	RealizeDirectory(dir);
	prj_file.data.Add("assembly", dir);
	prj_file.data.Add("main-package", main_pkg_name);
	
	{
		Node& main_pkg = root.GetAddNode(main_pkg_name, NODE_PACKAGE);
		Node& main_fn = main_pkg.GetAddNode("main", NODE_FUNCTION);
		main_fn.data.GetAdd("ret") = "int";
		ValueArray& params = ValueToArray(main_fn.data.GetAdd("params"));
		params.SetCount(2);
		ValueMap& arg0 = ValueToMap(params.At(0));
		ValueMap& arg1 = ValueToMap(params.At(1));
		arg0.GetAdd("type") = "int";
		arg0.GetAdd("name") = "argc";
		arg1.GetAdd("type") = "char**";
		arg1.GetAdd("name") = "argv";
	}
	
	for (const String& item : items) {
		int src = 0;
		if (item.Find("Package includes[") == 0)
			src = 1;
		else if (item.Find("Virtual package[") == 0)
			src = 2;
		
		if (!src) continue;
		
		String pkg_name = ParseItemArg(item);
		String item_path = "/Meta/Headers:" + item;
		ValueArray& opts = GetItemOpts(item_path);
		String pkg_path = "/assembly/" + pkg_name;
		Node& pkg = RealizeNode(pkg_path, NODE_PACKAGE);
		ValueArray& includes = ValueToArray(pkg.data.GetAdd("includes"));
		if (src == 1)
			includes.Clear();
		
		for(int i = 0; i < opts.GetCount(); i++) {
			String s = opts[i];
			
			if (src == 1) {
				if (s.Find("#include") == 0 || s.Find("<") == 0) {
					int a = s.Find("<");
					int b = s.Find(">", a+1);
					if (a >= 0 && b >= 0)
						includes.Add(s.Mid(a+1, b-a-1));
				}
				else if (s.Find("<") < 0 && s.Find("\"") < 0 && s.Find(":") >= 0) {
					int b = s.Find(":");
					includes.Add(TrimBoth(s.Left(b)));
				}
				else if (s.Find("<") < 0 && s.Find("\"") < 0 && s.Find("->") >= 0) {
					int b = s.Find("->");
					includes.Add(TrimBoth(s.Left(b)));
				}
			}
			else if (src == 2) {
				MetaStatementString stmt;
				
				if (!Parser::ParseMetaStatementString(s, stmt))
					continue;
				
				//stmt.name = Capitalize(stmt.name);
				
				if (1) stmt.Dump();
				
				MetaInstructionType type;
				if (!Parser::ParseMetaInstructionType(stmt.meta_instruction, type))
					continue;
				
				String class_path = pkg_path;
				if (!stmt.class_path.IsEmpty()) {
					for (String& cp : stmt.class_path) {
						//cp = Capitalize(cp);
						class_path += "/" + cp;
					}
					RealizeNode(class_path, NODE_CLASS);
				}
				String node_path = class_path + "/" + stmt.name;
				
				Node* n = 0;
				switch (type) {
					case DECLARE_FUNCTION:
					case DECLARE_METHOD: {
						n = &RealizeNode(node_path, NODE_FUNCTION);
						n->data.GetAdd("ret") = stmt.ret;
						ValueArray& params = ValueToArray(n->data.GetAdd("params"));
						for(int i = 0; i < stmt.params.GetCount(); i++) {
							const auto& in = stmt.params[i];
							ValueMap out;
							out.Add("name", in.name);
							out.Add("type", in.type);
							params.Add(out);
						}
						break;}
					case DECLARE_CLASS:
						n = &RealizeNode(node_path, NODE_CLASS);
						break;
					case DECLARE_META_FUNCTION:
						n = &RealizeNode(node_path, NODE_META);
						break;
					default: TODO
				}
			}
		}
		
	}
	WhenTree();
	#endif
}

void ProjectWizardView::ReadNodeTree(const FileNode* n) {
	TODO
	#if 0
	Node& root = RealizeNode("/assembly");
	String main_pkg_name = StringToName(root.owner->name);
	
	Node* main_pkg = root.FindNode(main_pkg_name, NODE_PACKAGE);
	if (!main_pkg)
		return;
	
	Node* main_fn = main_pkg->FindNode("main", NODE_FUNCTION);
	if (!main_fn)
		return;
	
	String path = n->path;
	
	GenericPromptArgs args;
	if (!MakeArgs(args, *n)) {
		PromptOK(error);
		return;
	}
	
	//PromptOK("fix args"); return; // add info about the function
	
	
	AiTaskManager();
	m.GetGenericPrompt(args, [this, path](String res) {
		
		WhenCallbackReady();
		WhenOptions();
	});
	
	#endif
}

void ProjectWizardView::SplitUniqueComponents(const FileNode* n) {
	TODO
	#if 0
	const auto& confs = ProjectWizardView::GetConfs();
	String file_path = n->GetFilePath();
	ValueArray& arr0 = GetItemOpts("/Plan/Client program:Unique components");
	for(int i = 0; i < arr0.GetCount(); i++) {
		String s =  arr0[i].ToString();
		int a = s.Find(":");
		if (a < 0) continue;
		String comp_name = s.Left(a);
		String sub_item0 = "UniqueComponents[" + comp_name + "]";
		String item_path0 = "/Plan/Client program:" + sub_item0;
		
		Vector<String> classes = Split(s.Mid(a+1), ",");
		ValueArray& arr1 = GetItemOpts(item_path0);
		arr1.Clear();
		for(int j = 0; j < classes.GetCount(); j++) {
			String& cls_name = classes[j];
			cls_name = Capitalize(TrimBoth(cls_name));
			cls_name.Replace("/", " or ");
			cls_name.Replace("  ", " ");
			arr1.Add(cls_name);
		}
		for(int j = 0; j < arr1.GetCount(); j++) {
			String src_path = item_path0;
			for (int k = 0; k < 2; k++) {
				String s1 = arr1[j];
				RemoveColonTrail(s1);
				s1 = TrimBoth(s1);
				String key = k == 1 ? "Classes" : "Tasks";
				String sub_item1 = key + "[" + s1 + "]";
				String item_path1 = "/Plan/Client program:" + sub_item1;
				const FileNode& n0 = RealizeFileNode(item_path1);
				String lbl_str = n0.GetAnyUserPromptInputString();
				ASSERT(lbl_str.GetCount()); // rule requires PromptInputUserText
				ValueMap& map0 = GetItem(item_path1);
				map0.GetAdd("src-path") = src_path;
				Value& user_input = map0.GetAdd(lbl_str);
				user_input = s1;
				src_path = item_path1;
			}
		}
	}
	WhenFile();
	#endif
}

void ProjectWizardView::GetAllClasses(const FileNode* n) {
	Vector<String> items = MakeItems("/Plan/Client program");
	Index<String> cls_names;
	for (String& item : items) {
		if (item.Left(8) != "Classes[")
			continue;
		String item_path = "/Plan/Client program:" + item;
		ValueArray& opts = GetItemOpts(item_path);
		
		for(int i = 0; i < opts.GetCount(); i++) {
			String cls = opts[i];
			
			if (cls.IsEmpty()) continue;
			int chr = cls[0];
			if (!IsDigit(chr) && !IsAlpha(chr))
				continue;
			
			int a = cls.Find(":");
			if (a >= 0)
				cls = cls.Left(a);
			
			cls_names.FindAdd(cls);
		}
	}
	
	SortIndex(cls_names, StdLess<String>());
	
	ValueArray& opts = GetItemOpts("/Plan/Client program:All classes");
	opts.Clear();
	for(int i = 0; i < cls_names.GetCount(); i++)
		opts.Add(cls_names[i]);
	
	WhenFile();
}

void VisitDepth(int cur_depth, int& max_depth, String cur_cls, const VectorMap<String, Index<String>>& cls_deps, Vector<String>& visited) {
	if (VectorFind(visited, cur_cls) >= 0)
		return;
	
	max_depth = max(max_depth, cur_depth);
	if (max_depth >= 10)
		return;
	
	visited.Add(cur_cls);
	
	int i = cls_deps.Find(cur_cls);
	if (i >= 0) {
		const auto& deps = cls_deps[i];
		for(int i = 0; i < deps.GetCount(); i++) {
			VisitDepth(cur_depth+1, max_depth, deps[i], cls_deps, visited);
		}
	}
	
	//visited.Remove(visited.GetCount()-1);
}

void ProjectWizardView::GetAllComponents(const FileNode* n) {
	Vector<String> items = MakeItems("/Plan/Client program");
	
	ValueArray& arr = GetItemOpts("/File tree/Builder:All components");
	arr.Clear();
	for(String& item : items) {
		if (item.Find("UniqueComponents[") == 0) {
			String path = "/Plan/Client program:" + item;
			ValueArray& comps = GetItemOpts(path);
			for(int i = 0; i < comps.GetCount(); i++) {
				arr.Add(comps[i]);
			}
		}
	}
	
	WhenFile();
}

#if 0
Node& ProjectWizardView::RealizeNodeType(const String& path, hash_t type_hash, hash_t dir_type_hash) {
	if (!type_hash)
		type_hash = AsTypeHash<Engine>();
	Node* n = this->node;
	ASSERT(n);
	while (n->owner)
		n = n->owner;
	ASSERT(path.GetCount());
	if (path[0] == '/') {
		Vector<String> parts = Split(path, "/");
		for (String& part : parts)
			n = &n->GetAddNode(part, &part == &parts.Top() ? type : sub_type, true);
	}
	else TODO;
	return *n;
}
#endif

Value& ProjectWizardView::GetItemValue(const String& path) {
	Value& val = GetItem(path).GetAdd("value");
	return val;
}

ValueArray& ProjectWizardView::GetItemOpts(const String& path) {
	return ValueToArray(GetItem(path).GetAdd("opts"));
}

ValueMap& ProjectWizardView::GetItem(const String& path) {
	int a = path.Find(":");
	ASSERT(a >= 0);
	String file_path = path.Left(a);
	String sub_item = path.Mid(a+1);
	ValueMap& map = ValueToMap(data.GetAdd(file_path));
	ValueMap& item = ValueToMap(map.GetAdd(sub_item));
	return item;
}

Value& ProjectWizardView::GetFileValue(const String& file_path) {
	ASSERT(file_path.Find(":") < 0);
	return data.GetAdd(file_path);
}

ValueMap& ProjectWizardView::GetFile(const String& file_path) {
	ValueMap& map = ValueToMap(data.GetAdd(file_path));
	return map;
}

void ProjectWizardView::PressReadyButton(const FileNode& n) {
	for(const ConfigurationOption& o : n.conf.options)
		if (o.type == ConfigurationOption::BUTTON_REFRESH)
			callback1(this, o.fn, &n)();
}

bool ProjectWizardView::OnlyReadyButton(const FileNode& n) {
	bool has_prompt = false, has_skip_value = false;
	for(const ConfigurationOption& o : n.conf.options) {
		if (o.type == ConfigurationOption::BUTTON_REFRESH)
			has_skip_value = true;
		if (o.type == ConfigurationOption::PROMPT_INPUT)
			has_prompt = true;
	}
	return !has_prompt && has_skip_value;
}

bool ProjectWizardView::MakeArgs(GenericPromptArgs& args, const FileNode& n) {
	error.Clear();
	
	for(const ConfigurationOption& o : n.conf.options) {
		if (o.type == ConfigurationOption::PROMPT_INPUT) {
			String path = o.value;
			const FileNode& n0 = RealizeFileNode(path);
			if (n0.conf.read_options) {
				MakeArgsOptions(args, n, o);
			}
			else {
				if (n0.conf.skip_value)
					continue;
				if (n0.conf.is_dynamic) {
					TODO
				}
				Value& val = GetItemValue(path);
				String s = val.ToString();
				if (s.IsEmpty()) {
					error = "Value is empty at " + path;
					return false;
				}
				auto& arr = args.lists.GetAdd(n0.GetFilePath() + ": " + n0.title);
				arr.Clear();
				arr << s;
			}
		}
		else if (o.type == ConfigurationOption::PROMPT_INPUT_OPTIONS ||
				 o.type == ConfigurationOption::PROMPT_INPUT_OPTIONS_FIXED) {
			MakeArgsOptions(args, n, o);
		}
		else if (o.type == ConfigurationOption::PROMPT_RESPONSE) {
			if (n.conf.title.GetCount()) {
				args.response_path = n.conf.path + ": " + n.conf.title;
			}
			args.response_title = o.value.ToString();
		}
		else if (o.type == ConfigurationOption::PROMPT_INPUT_USER_TEXT) {
			ValueMap& map = GetItem(n.path);
			String lbl_str = o.value.ToString();
			String s = map.GetAdd(lbl_str).ToString();
			args.lists.Add(lbl_str).Add(s);
		}
	}
	
	if (args.response_title.IsEmpty()) {
		error = ("No response title");
		return false;
	}
	
	return true;
}

bool ProjectWizardView::MakeArgsOptions(GenericPromptArgs& args, const FileNode& n, const ConfigurationOption& o) {
	bool skip_dynamic_values = o.type == ConfigurationOption::PROMPT_INPUT_OPTIONS_FIXED;
	String path = o.value;
	const FileNode* np = &RealizeFileNode(path);
	if (np->conf.is_dynamic && path.Find("[") < 0) {
		bool success = false;
		ValueMap& item = GetItem(n.path);
		String src = item.GetAdd("src-path");
		if (!src.IsEmpty()) {
			int a = src.Find("[");
			String static_item_path = a >= 0 ? src.Left(a) : src;
			if (static_item_path == path) {
				np = FindFileNode(src);
				if (np) {
					path = src;
					success = true;
				}
			}
		}
		if (!success) {
			String arg = n.GetItemArg();
			if (arg.GetCount()) {
				String s = path + "[" + arg + "]";
				np = FindFileNode(s);
				if (np) {
					path = s;
					success = true;
				}
			}
		}
		if (!success) {
			int a = path.Find(":");
			if (a >= 0) {
				String file_path = path.Left(a);
				String item_path = path.Mid(a+1);
				Vector<String> items = MakeItems(file_path);
				String comp = item_path + "[";
				for (String item : items) {
					if (item.Find(comp) == 0) {
						String path0 = file_path + ":" + item;
						FileNode& n0 = RealizeFileNode(path0);
						if (!MakeArgsOptionsNode(args, skip_dynamic_values, path0, n0))
							return false;
					}
				}
				return true;
			}
		}
		if (!success) {
			error = "Cannot solve the dynamic path: " + path;
			return false;
		}
	}
	
	return MakeArgsOptionsNode(args, skip_dynamic_values, path, *np);
}

bool ProjectWizardView::MakeArgsOptionsNode(GenericPromptArgs& args, bool skip_dynamic_values, const String& path, const FileNode& n0) {
	auto& arr = args.lists.GetAdd(path);
	arr.Clear();
	
	for(const ConfigurationOption& o0 : n0.conf.options) {
		if (o0.type == ConfigurationOption::FIXED) {
			String s = o0.value.ToString();
			arr << s;
		}
		else if (o0.type == ConfigurationOption::USER_INPUT_TEXT) {
			Value& val = GetItemValue(path);
			String s = val.ToString();
			arr << s;
		}
		else if (!skip_dynamic_values && (
					o0.type == ConfigurationOption::VALUE_ARRAY ||
					o0.type == ConfigurationOption::PROMPT_RESPONSE)) {
			ValueArray& opts = GetItemOpts(path);
			if (opts.IsEmpty()) {
				error = ("Options array is empty");
				return false;
			}
			
			/*if (!n0) {
				error = ("No configuration node found with the path: " + path);
				return false;
			}*/
			if (n0.title.IsEmpty()) {
				error = ("Configuration node title is empty for the path: " + path);
				return false;
			}
			
			for(int i = 0; i < opts.GetCount(); i++) {
				arr.Add(opts[i].ToString());
			}
		}
	}
	return true;
}

const ConfigurationNode* ProjectWizardView::FindConfigurationNode(const String& path) {
	int i = GetConfs().Find(path);
	if (i >= 0)
		return &GetConfs()[i];
	return 0;
}

FileNode* ProjectWizardView::FindFileNode(const String& path) {
	String title;
	const ConfigurationNode* cf = 0;
	const auto& confs = GetConfs();
	
	int a = path.Find(":");
	if (a < 0)
		return 0;
	String file_path = path.Left(a);
	String sub_item = path.Mid(a+1);
	ValueMap& file = GetFile(file_path);
	
	// Check for dynamic path
	a = path.Find("[");
	if (a >= 0) {
		int i = file.Find(sub_item);
		if (i < 0)
			return 0;
		
		String static_item_path = path.Left(a);
		i = confs.Find(static_item_path);
		if (i < 0)
			return 0;
		cf = &confs[i];
		
		ValueMap& dyn_item = GetItem(path);
		title = dyn_item.GetAdd("title");
		if (title.IsEmpty())
			title = cf->title;
	}
	else {
		int i = confs.Find(path);
		if (i < 0)
			return 0;
		cf = &confs[i];
		title = cf->title;
	}
	
	ASSERT(path.Find("[") < 0 || cf->is_dynamic);
	
	FileNode& n = nodes.Add(path, new FileNode(path, title, *cf));
	return &n;
}

FileNode& ProjectWizardView::RealizeFileNode(const String& path, const ConfigurationNode* cf) {
	String title;
	
	if (!cf) {
		const auto& confs = GetConfs();
		
		// Check for dynamic path
		int a = path.Find("[");
		if (a >= 0) {
			String item_path = path.Left(a);
			int b = path.Find(":");
			ASSERT(b >= 0);
			String file_path = item_path.Left(b);
			ValueMap& file = GetFile(file_path);
			
			int i = confs.Find(item_path);
			ASSERT_(i >= 0, "Dynamic item path not registered: " + item_path);
			cf = &confs[i];
			
			ValueMap& dyn_item = GetItem(path);
			title = dyn_item.GetAdd("title");
			if (title.IsEmpty())
				title = cf->title;
		}
		else {
			int i = confs.Find(path);
			cf = &confs[i];
			title = cf->title;
		}
	}
	
	ASSERT(path.Find("[") < 0 || cf->is_dynamic);
	
	FileNode& n = nodes.Add(path, new FileNode(path, title, *cf));
	return n;
}

Vector<String> ProjectWizardView::MakeItems(String file_path) {
	VectorMap<String,String> res;
	if (file_path.IsEmpty()) file_path = "/";
	ASSERT(file_path.Find(":") < 0);
	ASSERT(file_path.Find("[") < 0);
	String static_cmp = file_path + ":";
	
	nodes.Clear();
	
	// Static nodes
	const char* k0 = static_cmp.Begin();
	int c0 = static_cmp.GetCount();
	const auto& confs = GetConfs();
	for(int i = 0; i < confs.GetCount(); i++) {
		const String& key = confs.GetKey(i);
		const ConfigurationNode& cf = confs[i];
		if (cf.is_dynamic)
			continue;
		
		if (key.GetCount() >= c0) {
			const char* k1 = key.Begin();
			if (strncmp(k0, k1, c0) == 0) {
				String s = key.Mid(c0);
				if (s.IsEmpty()) continue;
				res.GetAdd(s) = Format("%05d", i);
				
				RealizeFileNode(cf.path, &cf);
			}
		}
	}
	
	// Dynamic nodes
	const auto& file = GetFile(file_path);
	for(int i = 0; i < file.GetCount(); i++) {
		String key = file.GetKey(i);
		if (key.IsEmpty()) continue;
		
		if (key[key.GetCount()-1] == ']') {
			int a = key.Find("[");
			if (a < 0) continue;
			
			String rule_path = file_path + ":" + key.Left(a);
			int j = confs.Find(rule_path);
			if (j < 0) continue;
			
			String item_path = file_path + ":" + key;
			
			FileNode& n0 = RealizeFileNode(item_path, &confs[j]);
			for(int j = 0; j < confs.GetCount(); j++) {
				if (&confs[j] == &n0.conf) {
					res.GetAdd(key) = Format("%05d-%s", j, item_path);
					break;
				}
			}
		}
	}
	
	SortByValue(res, StdLess<String>());
	
	Vector<String> v;
	v <<= res.GetKeys();
	return v;
}



INITIALIZER_COMPONENT(ProjectWizardView, "ecs.project.wizard", "Ecs|Asset")

END_UPP_NAMESPACE
