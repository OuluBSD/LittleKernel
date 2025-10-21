#ifndef _ide_Shell_IdeShell_h_
#define _ide_Shell_IdeShell_h_

NAMESPACE_UPP

struct IdeShell;
class ConsoleCtrl;

struct IdeShellHostBase {
	IdeShellHostBase();
	virtual ~IdeShellHostBase();
	virtual bool Command(IdeShell& shell, Value arg) = 0;
	virtual const String& GetOutput() const = 0;
	virtual const String& GetError() const = 0;
	virtual ConsoleCtrl* GetConsole() {return 0;}
	
	ArrayMap<String, EscValue> vars;
};

struct IdeShellHost : IdeShellHostBase {
	typedef IdeShellHost CLASSNAME;
	IdeShellHost();
	bool Command(IdeShell& shell, Value arg) override;
	const String& GetOutput() const override;
	const String& GetError() const override;
	void Put(const String& s);
	void PutLine(const String& s);
	void AddProgram(String cmd, Event<IdeShell&, Value> cb);
	void CurrentWorkingDirectory(IdeShell& shell, Value arg);
	void ListFiles(IdeShell& shell, Value arg);
	void ChangeDirectory(IdeShell& shell, Value arg);
	
	#ifdef flagNET
	void StartIntranet(IdeShell& shell, Value arg);
	#endif
	
	String out, err;
	ArrayMap<String, Event<IdeShell&,Value>> commands;
};

struct IdeShell : Upp::CodeEditor {
	typedef IdeShell CLASSNAME;
	IdeShell(IdeShellHostBase& h);
	void    Execute();
	void    PrintLineHeader();

	virtual bool Key(dword key, int count);
	virtual void LeftDouble(Point p, dword flags);
	virtual bool SetCurrentDirectory(const VfsPath& path);

	ArrayMap<String, EscValue> vars;
	IdeShellHostBase& host;
	VfsPath cwd;
	String line_header;

};

void InitShellHost(MetaEnvironment& env, IdeShellHost& host);
void EcsExt(MetaEnvironment& env, IdeShell& shell, Value value);
void ShellReg_MetaEnv(IdeShellHost& host);

END_UPP_NAMESPACE

#endif
