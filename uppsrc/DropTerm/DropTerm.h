#ifndef _DropTerm_DropTerm_h
#define _DropTerm_DropTerm_h

#include <signal.h>
#include <CtrlLib/CtrlLib.h>
#include <TabBar/TabBar.h>
#ifdef flagFTP
#include <PtyProcess/PtyProcess.h>
#endif
#include <RichEdit/RichEdit.h>
#include <PdfDraw/PdfDraw.h>
#include <CodeEditor/CodeEditor.h>
#include <Esc/Esc.h>

#ifndef flagGUI
	#error GUI flag is required
#endif

#ifndef flagV1
	#include <AI/Ctrl/Ctrl.h>
#endif
#include <ide/Shell/Shell.h>
using namespace Upp;


#ifdef flagNET
#include <Intranet/Intranet.h>
#endif

#define KEYGROUPNAME "Dropdown Terminal"
#define KEYNAMESPACE Dropdown
#define KEYFILE      <DropTerm/DropTerm.key>
#include             <CtrlLib/key_header.h>

using namespace Dropdown;

#define IMAGECLASS DropTermImg
#define IMAGEFILE <DropTerm/DropTerm.iml>
#include <Draw/iml_header.h>

#include "Word.h"

NAMESPACE_UPP

#ifdef flagOSX
class TrayApp : public TopWindow
#else
class TrayApp : public TrayIcon
#endif
{
	bool is_exit;
	
public:
	typedef TrayApp CLASSNAME;

	TrayApp();
	
	bool IsExit() const {return is_exit;}
	void Exit() {is_exit = true; Break();}
	void Close() {Break();}
	
	virtual void LeftDouble();
	virtual void LeftDown();
	virtual void Menu(Bar& bar);
};


class DropTerm : public TopWindow {
	MenuBar						menu;
	
protected:
	friend class IdeDropdownTerminal;
	ArrayMap<int, ConsoleCtrl>	cons;
	TabBar						tabs;
	double						alpha = 0.9;
	int							id_counter;
	bool						def_terminal = true;
	bool						is_exit = false;
	
	void ShowTabId(int i);
	void ShowTab();
	
public:
	typedef DropTerm CLASSNAME;
	DropTerm();
	
	ConsoleCtrl& NewConsole();
	ConsoleCtrl* GetActiveConsole();
	void SetSemiTransparent();
	void Quit();
	void LeaveProgram();
	void AddConsole() {NewConsole();}
	void CloseTab();
	void CloseTabId(int id);
	void PreviousTab();
	void NextTab();
	void TabClosed(Value tab);
	void MainMenu(Bar& menu);
	void AppMenu(Bar& menu);
	void ViewMenu(Bar& menu);
	void SetupMenu(Bar& menu);
	void RefreshMenu();
	void SetLang(int lang);
	void RefreshTitle(int id);
	void RefreshTitle1(String title, int id);
	void ViewChange(int id);
	void RemoveId(int id);
	bool IsExit() const {return is_exit;}
	
	virtual bool Key(dword key, int count);

	void PostClose() {PostCallback(THISBACK(Close0));}
	void Close0() {Close(); }
	void PostTopMost() {PostCallback(THISBACK(TopMost0));}
	void TopMost0();
	void SetLayout();
	void PostSetLayout() {PostCallback(THISBACK(SetLayout));}
	void PostSetSemiTransparent() {PostCallback(THISBACK(SetSemiTransparent));}
	
};


class IdeDropdownTerminal {
	bool enable_intranet = false;
	DropTerm cons;
	dword toggle_key = 0;
	dword ide_key = 0;
	bool enabled = false;
	bool is_tray = false;
	bool is_cons_toggled = false;
	bool is_ide_toggled = false;
	bool is_exit = false;
	One<TrayApp> tray;
	
public:
	typedef IdeDropdownTerminal CLASSNAME;
	IdeDropdownTerminal();
	~IdeDropdownTerminal();
	void Init();
	void ToggleWindow();
	void ToggleIde();
	void Reset();
	void Run();
	bool IsEnabled() const {return enabled && !is_ide_toggled;}
	bool IsRunning() const {return !is_exit;}
	bool IsIdeToggled() const {return is_ide_toggled;}
	
};

END_UPP_NAMESPACE

#endif
