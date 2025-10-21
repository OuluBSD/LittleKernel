#ifndef _AI_TextCtrl_TextDesigner_h_
#define _AI_TextCtrl_TextDesigner_h_


class SolverBaseIndicator : public ProgressIndicator {

	void SetProgress(int a, int t);

public:
	typedef SolverBaseIndicator CLASSNAME;
	SolverBaseIndicator();

	void Attach(SolverBase& sb);
	void AttachRemaining(Label& lbl);
};

class ToolAppCtrl : public Ctrl, public DatasetProvider {
	String data, data_includes, data_filepath;
	VfsValue* file_node = 0;
	
protected:
	String data_sha1;
	
	void SetFileNode(VfsValue* n) {file_node = n;}
public:
	SolverBaseIndicator prog;
	Label remaining;
	Callback WhenTitle;
	
public:
	typedef ToolAppCtrl CLASSNAME;
	virtual ~ToolAppCtrl();
	virtual void Data() = 0;
	virtual void ToolMenu(Bar& bar) { bar.Add("", MetaImgs::placeholder16(), Callback()); }
	virtual String GetStatusText() { return String(); }
	
	void AddMenu();
	Entity& GetEntity();
	Component& GetComponent();
	//EditorPtrs& GetPointers() const; // TODO rename EditorPtrs
	void GetDataset(DatasetPtrs&) const override;
	Script& GetScript();
	//const Index<String>& GetTypeclasses() const;
	//const Vector<ContentType>& GetContents() const;
	//const Vector<String>& GetContentParts() const;
	
	bool IsScript() const;
	bool HasPointers() const;
	String GetComponentTitle() const;
	String GetFilePath() const {ASSERT(data_filepath.GetCount()); return data_filepath;}
	String GetFileIncludes() const {return data_includes;}
	String GetFileData() const {return data;}
	VfsValue* GetFileNode() const {return file_node;}
	
	void MakeComponentParts(ArrayCtrl& parts);
	void GetAttrs(const VectorMap<String, String>& data, VectorMap<String, String>& v);
	void UpdateMenu();
	
	virtual void Realize(const String& includes, const String& filename);
	virtual bool Load(const String& includes, const String& filename, Stream& in, byte charset);
	virtual bool LoadDirectory(const String& includes, const String& filename, const String& dirpath, byte charset);
	virtual void Save(Stream& s, byte charset);
	virtual void SaveDirectory(String dirpath, byte charset);
	virtual void EditPos(JsonIO& jio);
	virtual void SetPickUndoData(LineEdit::UndoData undodata);
	virtual void Visit(Vis& v) = 0;
	
	template <class T>
	void DoT(int fn)
	{
		DatasetPtrs p;
		GetDataset(p);
		T& sdi = T::Get(p);
		prog.Attach(sdi);
		sdi.WhenRemaining <<
			[this](String s) { PostCallback([this, s]() { remaining.SetLabel(s); }); };
		if(fn == 0)
			sdi.Start();
		else
			sdi.Stop();
	}
};


#endif
