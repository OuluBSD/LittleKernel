#ifndef FORM_HPP
#define FORM_HPP

#include <CtrlLib/CtrlLib.h>
#include <GridCtrl/GridCtrl.h>
using namespace Upp;

#include "Container.hpp"
#include "FormLayout.hpp"

class Form : public ParentCtrl
{
	typedef Form CLASSNAME;

public:
	 Form();
	~Form();

	bool Load(const String& file);
	bool LoadString(const String& xml, bool compression);
	bool Layout(const String& layout, Font font = StdFont());
	bool Generate(Font font = StdFont());

	Ctrl* GetCtrl(const String& var);
	Value GetData(const String& var);


	String Script;
	Callback3<const String&, const String&, const String& > SignalHandler;

	ArrayMap<String, Ctrl>& GetCtrls() { return _Ctrls; }
	const ArrayMap<String, Ctrl>& GetCtrls() const { return _Ctrls; }

	void Clear(bool all = true);

	bool IsLayout() { return _Current != -1; }
	int HasLayout(const String& layout);

	void Xmlize(XmlIO xml) { xml("layouts", _Layouts); }

	Array<FormLayout>& GetLayouts() { return _Layouts; }
	const Array<FormLayout>& GetLayouts() const { return _Layouts; }

protected:
	friend class FormWindow;
	
	void OnAction(const String& action);
	bool SetCallback(const String& action, Callback c);

	Vector<String> _Acceptors;
	Vector<String> _Rejectors;
	Array<FormLayout> _Layouts;
	ArrayMap<String, Ctrl> _Ctrls;

	Event<> WhenGenerate;
	
private:
	int _Current;
	String _File;
};

class FormWindow : public TopWindow {
	Form form;
	
public:
	typedef FormWindow CLASSNAME;
	FormWindow();
	String ExecuteForm();
	
	bool Exit(const String& action);
	bool Acceptor(const String& action);
	bool Rejector(const String& action);

	bool Load(const String& file) {return form.Load(file);}
	bool LoadString(const String& xml, bool compression) {return form.LoadString(xml, compression);}
	bool Layout(const String& layout, Font font = StdFont()) {return form.Layout(layout, font);}
	Form& GetForm() {return form;}
protected:
	void Generate();
	
};


#endif // .. FORM_HPP
