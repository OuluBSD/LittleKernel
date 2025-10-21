#ifndef _AI_Ctrl_ProjectWizard_h_
#define _AI_Ctrl_ProjectWizard_h_


NAMESPACE_UPP



struct LabeledEditString : public Ctrl {
	Label lbl;
	EditString edit;
	LabeledEditString();
};

class ProjectWizardCtrl : public AiComponentCtrl {
	Splitter hsplit, vsplit, optsplit;
	ArrayCtrl dirs, files, items, options;
	DocEdit option;
	Ctrl main;
	int main_type = MAIN_OPTION_LIST;
	Array<Event<>> cb_queue;
	
	enum {
		MAIN_OPTION_LIST
	};
	
	String cwd, file_dir, file_path, item_path;
	
	enum {
		VIEW_REQUIREMENTS,
		VIEW_DELIVERABLES,
		VIEW_TECHNOLOGIES,
		VIEW_DEVELOPMENT,
		VIEW_LANGUAGE,
		VIEW_STRUCTURE,
		VIEW_DEPENDENCIES,
		VIEW_DOCUMENTATION,
		VIEW_ARCHITECTURE,
		VIEW_HEADERS,
		VIEW_PERFORMANCE,
		VIEW_USER_INTERFACE,
		VIEW_DATA_SECURITY,
		VIEW_INTEGRATION,
		VIEW_MAINTENANCE,
		VIEW_ERROR_HANDLING,
		VIEW_SOURCE,
		
		VIEW_COUNT
	};
	
	static String GetViewName(int i);
	
	ValueMap cursor_history;
	
public:
	typedef ProjectWizardCtrl CLASSNAME;
	ProjectWizardCtrl();
	
	void Data() override;
	void DataDirectory();
	void DataFile();
	void DataItem();
	void DataOption();
	void OnOption();
	void OnCallbackReady();
	void OnTreeChange();
	void ToolMenu(Bar& bar) override;
	void Do(int fn);
	void EditPos(JsonIO& json) override;
	
	Index<String> GetDirectories(String dir);
	Index<String> GetFiles(String dir);
	
	void SetView(int i);
	int GetHistoryCursor(String path);
	void SetHistoryCursor(String path, int i);
	ProjectWizardView& GetView();
	
};

INITIALIZE(ProjectWizardCtrl)


END_UPP_NAMESPACE


#endif
 
