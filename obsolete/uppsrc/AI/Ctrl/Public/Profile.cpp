#include <AI/Ctrl/Ctrl.h>

NAMESPACE_UPP


ProfileInfoCtrl::ProfileInfoCtrl() {
	CtrlLayout(*this);
	
	name <<= THISBACK(OnValueChange);
	created <<= THISBACK(OnValueChange);
	description <<= THISBACK(OnValueChange);
	preferences <<= THISBACK(OnValueChange);
	
	languages.AddColumn(t_("Language"));
	languages.AddColumn(t_("Set"));
	languages.ColumnWidths("4 1");
	languages.SetCount(LNG_COUNT);
	for(int i = 0; i < LNG_COUNT; i++) {
		languages.Set(i, 0, Capitalize(GetLanguageKey(i)));
		Option* o = new Option;
		o->WhenAction << THISBACK(OnValueChange);
		languages.SetCtrl(i, 1, o);
	}
	languages <<= THISBACK(OnValueChange);
}

void ProfileInfoCtrl::Data() {
	Profile& a = GetExt<Profile>();
	
	Clear();
	
	this->name						.SetData(a.name);
	this->created					.SetData(a.created);
	this->description				.SetData(a.description);
	this->preferences				.SetData(a.preferences);
	
	for(int i = 0; i < LNG_COUNT; i++) {
		Ctrl* c = languages.GetCtrl(i, 1);
		if (!c) continue;
		Option* opt = dynamic_cast<Option*>(c);
		ASSERT(opt);
		opt->Set(a.languages.Find(i) >= 0);
	}
}

void ProfileInfoCtrl::Clear() {
	this->name						.Clear();
	this->created					.Clear();
	this->description				.Clear();
	this->preferences				.Clear();
	for(int i = 0; i < LNG_COUNT; i++) {
		Ctrl* c = languages.GetCtrl(i, 1);
		Option* opt = dynamic_cast<Option*>(c);
		if (opt) opt->Set(0);
	}
}

void ProfileInfoCtrl::OnValueChange() {
	Profile& o = GetExt<Profile>();
	
	o.name						= this->name.GetData();
	o.created					= this->created.GetData();
	o.description				= this->description.GetData();
	o.preferences				= this->preferences.GetData();
	
	o.languages.Clear();
	for(int i = 0; i < LNG_COUNT; i++) {
		Option* opt = dynamic_cast<Option*>(languages.GetCtrl(i, 1));
		ASSERT(opt);
		if (opt->Get())
			o.languages.Add(i);
	}
}

INITIALIZER_COMPONENT_CTRL(Profile, ProfileInfoCtrl)


END_UPP_NAMESPACE
