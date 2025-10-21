#ifndef _AI_Core_ProjectWizard_h_
#define _AI_Core_ProjectWizard_h_




struct ProjectWizardView;
struct ConfigurationNode;
struct FileNode;


struct ConfigurationOption {
	typedef enum {
		UNDEFINED,
		FIXED,
		BUTTON,
		BUTTON_REFRESH,
		USER_INPUT_TEXT,
		VALUE_ARRAY,
		PROMPT_INPUT,
		PROMPT_INPUT_OPTIONS_FIXED,
		PROMPT_INPUT_OPTIONS,
		PROMPT_RESPONSE,
		PROMPT_INPUT_USER_TEXT,
	} Type;
	
	Type type = UNDEFINED;
	Value value;
	void(ProjectWizardView::*fn)(const FileNode*) = 0;
};

struct ConfigurationNode {
	String path;
	String title;
	Array<ConfigurationOption> options;
	bool read_options = false;
	bool is_dynamic = false;
	bool skip_value = false;
	
	ConfigurationNode& SkipValue();
	ConfigurationNode& DefaultReadOptions();
	ConfigurationNode& OptionFixed(Value v);
	ConfigurationNode& OptionButton(Value v, void(ProjectWizardView::*fn)(const FileNode* n), bool is_refresh=false);
	ConfigurationNode& OptionRefresh();
	ConfigurationNode& OptionValueArray();
	ConfigurationNode& OptionUserInputText();
	ConfigurationNode& PromptInput(String path);
	ConfigurationNode& PromptInputAllPrevious();
	ConfigurationNode& PromptInputOptionsLocalFixed();
	ConfigurationNode& PromptInputOptions(String path);
	ConfigurationNode& PromptResponse(String title, bool have_refresh=true);
	ConfigurationNode& PromptInputUserText(String title);
};

struct FileNode {
	const ConfigurationNode& conf;
	String path;
	String title;
	
	FileNode(String path, String title, const ConfigurationNode& conf) : conf(conf), path(path), title(title) {}
	bool IsDynamic() const {return conf.is_dynamic;}
	String GetFilePath() const;
	String GetItemPath() const;
	String GetItemArg() const;
	String GetAnyUserInputString() const;
	String GetAnyUserPromptInputString() const;
	
};

struct ProjectWizardView : Component {
	
private:
	String error;
	
	
public:
	ArrayMap<String, FileNode> nodes;
	Vector<String> MakeItems(String file);
	
	VectorMap<String,Value> data;
public:
	COMPONENT_CONSTRUCTOR(ProjectWizardView)
	
	void Data();
	void Visit(Vis& v) override {
		v.Ver(1)
		(1)	("data", data);
	}
	static VectorMap<String,String>& GetCategories() {static VectorMap<String,String> m; return m;}
	static void RegisterCategory(String key, String desc) {GetCategories().GetAdd(key) = desc;}
	static ArrayMap<String, ConfigurationNode>& GetConfs() {static ArrayMap<String, ConfigurationNode> m; return m;}
	static ConfigurationNode& Register(String path, String title=String());
	static const ConfigurationNode* FindConfigurationNode(const String& path);
	static ConfigurationNode& RegisterDynamic(String path, String title=String());
	
	void DefaultDynamicPath(String path);
	void DefaultDynamic(const FileNode* n);
	void ClearAllDynamic(const FileNode* n);
	void SplitComponents(const FileNode* n);
	void SplitSubComponents(const FileNode* n);
	void SplitDependencies(const FileNode* n);
	void SplitTechnologyCategories(const FileNode* n);
	void SplitUniqueComponents(const FileNode* n);
	void GetAllClasses(const FileNode* n);
	void GetAllComponents(const FileNode* n);
	void SplitItems(const FileNode* n, String key);
	void SplitPackages(const FileNode* n);
	void ParseVirtualPackageData(const FileNode* n);
	void ClearAssembly(const FileNode* n);
	void ReadNodeTree(const FileNode* n);
	void PressReadyButton(const FileNode& n);
	bool OnlyReadyButton(const FileNode& n);
	bool MakeArgs(GenericPromptArgs& args, const FileNode& n);
	bool MakeArgsOptions(GenericPromptArgs& args, const FileNode& n, const ConfigurationOption& o);
	bool MakeArgsOptionsNode(GenericPromptArgs& args, bool skip_dynamic_values, const String& path, const FileNode& n0);
	VfsValue& RealizeNodeType(const String& path, hash_t type_hash=0, hash_t dir_type_hash = 0);
	
	ValueMap& GetFile(const String& path);
	Value& GetFileValue(const String& path);
	Value& GetItemValue(const String& path);
	ValueMap& GetItem(const String& path);
	ValueArray& GetItemOpts(const String& path);
	FileNode& RealizeFileNode(const String& path, const ConfigurationNode* cf=0);
	FileNode* FindFileNode(const String& path);
	
	Event<> WhenFile;
	Event<> WhenTree;
	Event<> WhenOptions;
	Event<> WhenCallbackReady;
};

INITIALIZE(ProjectWizardView)




#endif
