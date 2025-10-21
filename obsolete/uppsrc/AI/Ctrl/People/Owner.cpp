#include <AI/Ctrl/Ctrl.h>

NAMESPACE_UPP


OwnerInfoCtrl::OwnerInfoCtrl() {
	CtrlLayout(*this);
	
	name <<= THISBACK(OnValueChange);
	born <<= THISBACK(OnValueChange);
	description <<= THISBACK(OnValueChange);
	environment <<= THISBACK(OnValueChange);
}

void OwnerInfoCtrl::Data() {
	Clear();
		
	Owner& o = GetExt<Owner>();
	this->name						.SetData(o.name);
	this->born						.SetData(o.born);
	this->description				.SetData(o.description);
	this->environment				.SetData(o.environment);
}

void OwnerInfoCtrl::Clear() {
	this->name						.Clear();
	this->born						.Clear();
	this->description				.Clear();
	this->environment				.Clear();
}

void OwnerInfoCtrl::OnValueChange() {
	Owner& o = GetExt<Owner>();
	o.name						= this->name.GetData();
	o.born						= this->born.GetData();
	o.description				= this->description.GetData();
	o.environment				= this->environment.GetData();
}

INITIALIZER_COMPONENT_CTRL(Owner, OwnerInfoCtrl)


END_UPP_NAMESPACE
