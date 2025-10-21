#include <AI/Ctrl/Ctrl.h>

NAMESPACE_UPP

ProjectWizardCtrl::ProjectWizardCtrl() {
	Add(hsplit.SizePos());
	
	hsplit.Horz() << vsplit << items << main;
	hsplit.SetPos(1500,0);
	hsplit.SetPos(7000,1);
	
	vsplit.Vert() << dirs << files;
	
	cwd = "/";
	
	dirs.AddColumn("Directory");
	dirs.WhenCursor << THISBACK(DataDirectory);
	
	files.AddColumn("File");
	files.WhenCursor << THISBACK(DataFile);
	
	items.AddColumn("Item");
	items.AddColumn("Value");
	items.ColumnWidths("2 3");
	items.WhenCursor << THISBACK(DataItem);
	items.SetLineCy(45);
	
	
	optsplit.Vert() << options << option;
	optsplit.SetPos(7500);
	
	options.AddColumn("Option");
	options.WhenCursor << THISBACK(OnOption);
	options.SetLineCy(25);
	options.NoWantFocus();
	
	main.Add(optsplit.SizePos());
	
}

String ProjectWizardCtrl::GetViewName(int i) {
	switch (i) {
		case VIEW_REQUIREMENTS:		return "Define project requirements and scope";
		case VIEW_DELIVERABLES:		return "Create a project plan and timeline";
		case VIEW_TECHNOLOGIES:		return "Identify necessary tools and technologies";
		case VIEW_DEVELOPMENT:		return "Set up a development environment";
		case VIEW_LANGUAGE:			return "Choose a programming language";
		case VIEW_STRUCTURE:		return "Determine project structure and organization";
		case VIEW_DEPENDENCIES:		return "Install and configure necessary dependencies and libraries";
		case VIEW_DOCUMENTATION:	return "Write design documentation";
		case VIEW_ARCHITECTURE:		return "Consider program architecture and design patterns";
		case VIEW_HEADERS:			return "Consider data structures, classes, and functions needed";
		case VIEW_PERFORMANCE:		return "Consider performance and efficiency";
		case VIEW_USER_INTERFACE:	return "Consider user interface design and user experience";
		case VIEW_DATA_SECURITY:	return "Consider data security and privacy";
		case VIEW_INTEGRATION:		return "Consider integration with other systems or platforms";
		case VIEW_MAINTENANCE:		return "Consider maintenance and future updates";
		case VIEW_ERROR_HANDLING:	return "Consider error handling and debugging strategies";
		case VIEW_SOURCE:			return "Create source code files";
		default: return "Error";
	}
}

void ProjectWizardCtrl::Data() {
	ProjectWizardView& view = GetView();
	view.WhenFile = [this]{PostCallback(THISBACK(DataFile));};
	view.WhenOptions = [this]{PostCallback(THISBACK(DataItem));};
	view.WhenCallbackReady = THISBACK(OnCallbackReady);
	view.WhenTree = THISBACK(OnTreeChange);
	
	Index<String> dir_list = GetDirectories(cwd);
	
	int row = 0;
	for(int i = 0; i < dir_list.GetCount(); i++) {
		dirs.Set(row++, 0, dir_list[i]);
	}
	INHIBIT_CURSOR(dirs);
	dirs.SetCount(row);
	int cursor = GetHistoryCursor(cwd);
	if (cursor >= 0 && cursor < dirs.GetCount())
		dirs.SetCursor(cursor);
	
	
	DataDirectory();
	//PostCallback([this]{dirs.SetFocus();});
}

void ProjectWizardCtrl::DataDirectory() {
	if (!dirs.IsCursor())
		return;
	
	SetHistoryCursor(cwd, dirs.GetCursor());
	
	String sub_dir = dirs.Get(0);
	file_dir = AppendUnixFileName(cwd, sub_dir);
	Index<String> file_list = GetFiles(file_dir);
	
	int row = 0;
	for(int i = 0; i < file_list.GetCount(); i++) {
		files.Set(row++, 0, file_list[i]);
	}
	INHIBIT_CURSOR(files);
	files.SetCount(row);
	int cursor = GetHistoryCursor(file_dir);
	if (cursor >= 0 && cursor < files.GetCount())
		files.SetCursor(cursor);
	
	DataFile();
	//PostCallback([this]{files.SetFocus();});
}

void ProjectWizardCtrl::DataFile() {
	if (!dirs.IsCursor() || !files.IsCursor())
		return;
	
	// Navigator
	SetHistoryCursor(file_dir, files.GetCursor());
	
	String sub_file = files.Get(0);
	file_path = AppendUnixFileName(file_dir, sub_file);
	Vector<String> item_list = GetView().MakeItems(file_path);
	
	
	// List items
	int row = 0;
	for(int i = 0; i < item_list.GetCount(); i++) {
		items.Set(row, 0, item_list[i]);
		items.Set(row, 1, Value());
		row++;
	}
	INHIBIT_CURSOR(items);
	items.SetCount(row);
	int cursor = GetHistoryCursor(file_path);
	if (cursor >= 0 && cursor < items.GetCount())
		items.SetCursor(cursor);
	
	
	
	// Data
	ProjectWizardView& n = GetView();
	Value& val = n.data.GetAdd(file_path);
	
	
	if (val.Is<ValueMap>()) {
		ValueMap& map = ValueToMap(val);
		for(int i = 0; i < items.GetCount(); i++) {
			String item = items.Get(i,0);
			int j = map.Find(item);
			if (j < 0)
				items.Set(i, 1, Value());
			else {
				ValueMap item = map.GetValue(j);
				items.Set(i, 1,  item.GetAdd("value").ToString());
			}
		}
	}
	
	
	DataItem();
	//PostCallback([this]{items.SetFocus();});
	
}

void ProjectWizardCtrl::DataItem() {
	if (!dirs.IsCursor() || !files.IsCursor() || !items.IsCursor())
		return;
	
	ProjectWizardView& view = GetView();
	SetHistoryCursor(file_path, items.GetCursor());
	
	String sub_item = items.Get(0);
	item_path = file_path + ":" + sub_item;
	const auto& confs = ProjectWizardView::GetConfs();
	int i = view.nodes.Find(item_path);
	
	if (i < 0) {
		options.Clear();
		return;
	}
	
	const FileNode& cf = view.nodes[i];
	
	if (main_type == MAIN_OPTION_LIST) {
		String value;
		
		
		// Data
		ValueMap& item = view.GetItem(item_path);
		Value& val = view.GetItemValue(item_path);
		value = val.ToString();
		
		INHIBIT_CURSOR(options);
		options.Clear();
		int cursor = 0;
		int row = 0;
		for(int i = 0; i < cf.conf.options.GetCount(); i++) {
			const ConfigurationOption& opt = cf.conf.options[i];
			String s = opt.value.ToString();
			if (opt.type == ConfigurationOption::FIXED) {
				options.Set(row++, 0, s);
				if (s == value)
					cursor = i;
			}
			else if (opt.type == ConfigurationOption::USER_INPUT_TEXT) {
				EditString* e = new EditString();
				String s = val.ToString();
				e->WhenAction = [this, e, &val]{
					String s = e->GetData();
					val = s;
					items.Set(1, s);
				};
				options.Set(row, 0, s);
				options.SetCtrl(row++, 0, e);
			}
			else if (opt.type == ConfigurationOption::BUTTON ||
					 opt.type == ConfigurationOption::BUTTON_REFRESH) {
				Button* btn = new Button();
				btn->SetLabel(s);
				btn->WhenAction = callback1(&view, opt.fn, &cf);
				options.Set(row, 0, s);
				options.SetCtrl(row++, 0, btn);
			}
			else if (opt.type == ConfigurationOption::VALUE_ARRAY ||
					 opt.type == ConfigurationOption::PROMPT_RESPONSE
					) {
				ValueArray& opts = view.GetItemOpts(item_path);
				for(int j = 0; j < opts.GetCount(); j++) {
					String s = opts.Get(j).ToString();
					if (s == value)
						cursor = row;
					options.Set(row++, 0, s);
				}
			}
			else if (opt.type == ConfigurationOption::PROMPT_INPUT_USER_TEXT) {
				String lbl_str = opt.value.ToString();
				LabeledEditString* e = new LabeledEditString();
				e->lbl.SetLabel(lbl_str + ":");
				e->edit.SetData(item.GetAdd(lbl_str).ToString());
				e->edit.WhenAction = [this, &item, lbl_str, e]{
					String s = e->edit.GetData();
					Value& v = item.GetAdd(lbl_str);
					v = s;
				};
				options.SetCtrl(row++, 0, e);
			}
		}
		options.SetCount(row);
		if (cursor >= 0 && cursor < row)
			options.SetCursor(cursor);
	}
	
	DataOption();
}

void ProjectWizardCtrl::DataOption() {
	if (!options.IsCursor()) {
		option.SetData("");
	}
	else {
		option.SetData(options.Get(0));
	}
	//PostCallback([this]{items.SetFocus();});
}

void ProjectWizardCtrl::OnOption() {
	if (!dirs.IsCursor() || !files.IsCursor() || !items.IsCursor())
		return;
	
	ProjectWizardView& view = GetView();
	
	if (main_type == MAIN_OPTION_LIST) {
		
		if (!options.IsCursor())
			return;
		
		String key = items.Get(0);
		String value = options.Get(0);
		
		// Data
		Value& val = view.GetItemValue(item_path);
		val = value;
		
		items.Set(1, value);
	}
	
	DataOption();
}

void ProjectWizardCtrl::OnCallbackReady() {
	if (cb_queue.IsEmpty())
		return;
	auto cb = cb_queue[0];
	cb_queue.Remove(0);
	PostCallback(cb);
}

void ProjectWizardCtrl::OnTreeChange() {
	TODO
	#if 0
	if (org)
		PostCallback(callback(org, &OrganizationCtrl::Data));
	#endif
}

Index<String> ProjectWizardCtrl::GetDirectories(String dir) {
	Index<String> res;
	ProjectWizardView& view = GetView();
	if (dir.IsEmpty()) dir = "/";
	if (dir != "/")
		res.Add("..");
	const char* k0 = dir.Begin();
	int c = dir.GetCount();
	const auto& confs = view.GetConfs();
	for (const String& key : confs.GetKeys()) {
		if (key.GetCount() >= c) {
			const char* k1 = key.Begin();
			if (strncmp(k0, k1, c) == 0) {
				String left = key.Mid(c);
				if (left.IsEmpty()) continue;
				if (left[0] == '/') left = left.Mid(1);
				int a = left.Find("/",1);
				if (a > 0) {
					left = left.Left(a);
					res.FindAdd(left);
				}
			}
		}
	}
	return res;
}

Index<String> ProjectWizardCtrl::GetFiles(String dir) {
	Index<String> res;
	ProjectWizardView& view = GetView();
	if (dir.IsEmpty()) dir = "/";
	const char* k0 = dir.Begin();
	int c = dir.GetCount();
	const auto& confs = view.GetConfs();
	for (const String& key : confs.GetKeys()) {
		if (key.GetCount() >= c) {
			const char* k1 = key.Begin();
			if (strncmp(k0, k1, c) == 0) {
				String s = key.Mid(c);
				if (s.IsEmpty()) continue;
				if (s[0] == '/') s = s.Mid(1);
				int a = s.Find("/",1);
				if (a < 0) {
					int a = s.Find(":");
					if (a >= 0)
						s = s.Left(a);
					res.FindAdd(s);
				}
			}
		}
	}
	return res;
}

void ProjectWizardCtrl::ToolMenu(Bar& bar) {
	bar.Add(t_("Refresh"), MetaImgs::RedRing(), THISBACK1(Do, 0)).Key(K_F5);
	bar.Add(t_("Additional button function"), MetaImgs::RedRing(), THISBACK1(Do, 1)).Key(K_F6);
	bar.Separator();
	bar.Add(t_("Press all 'Refresh' buttons in this file"), MetaImgs::RedRing(), THISBACK1(Do, 2)).Key(K_F9);
}

ProjectWizardView& ProjectWizardCtrl::GetView() {
	return GetExt<ProjectWizardView>();
}

void ProjectWizardCtrl::Do(int fn) {
	if (fn == 0) {
		auto& nodes = GetView().nodes;
		int i = nodes.Find(item_path);
		if (i < 0)
			return;
		const FileNode& cf = nodes[i];
		for(const ConfigurationOption& opt : cf.conf.options) {
			if (opt.type == ConfigurationOption::BUTTON_REFRESH) {
				PostCallback(callback1(&GetView(), opt.fn, &cf));
				PostCallback([this]{items.SetFocus();});
				return;
			}
		}
	}
	else if (fn == 1) {
		auto& nodes = GetView().nodes;
		int i = nodes.Find(item_path);
		if (i < 0)
			return;
		const FileNode& cf = nodes[i];
		for(const ConfigurationOption& opt : cf.conf.options) {
			if (opt.type == ConfigurationOption::BUTTON) {
				PostCallback(callback1(&GetView(), opt.fn, &cf));
				PostCallback([this]{items.SetFocus();});
				return;
			}
		}
	}
	else if (fn == 2) {
		auto& view = GetView();
		cb_queue.Clear();
		Vector<String> item_list = view.MakeItems(file_path);
		for(String sub_item : item_list) {
			String item_path = file_path + ":" + sub_item;
			FileNode& cf = view.RealizeFileNode(item_path);
			for(const ConfigurationOption& opt : cf.conf.options) {
				if (opt.type == ConfigurationOption::BUTTON_REFRESH) {
					cb_queue.Add(callback1(&view, &ProjectWizardView::DefaultDynamicPath, item_path));
					break;
				}
			}
			
		}
		if (cb_queue.GetCount()) {
			auto cb = cb_queue[0];
			cb_queue.Remove(0);
			PostCallback(cb);
		}
	}
}

void ProjectWizardCtrl::EditPos(JsonIO& json) {
	json ("cursor_history", cursor_history);
}

int ProjectWizardCtrl::GetHistoryCursor(String path) {
	int j = cursor_history.Find(path);
	if (j >= 0)
		return cursor_history.GetValue(j);
	return 0;
}

void ProjectWizardCtrl::SetHistoryCursor(String path, int i) {
	cursor_history.Set(path, i);
}




LabeledEditString::LabeledEditString() {
	int w = 200;
	Add(lbl.LeftPos(0, w-5).VSizePos());
	Add(edit.HSizePos(w,0).VSizePos());
	lbl.AlignRight();
	lbl.NoWantFocus();
	edit.NoWantFocus();
}


INITIALIZER_COMPONENT_CTRL(ProjectWizardView, ProjectWizardCtrl)

END_UPP_NAMESPACE
