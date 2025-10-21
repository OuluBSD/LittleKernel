#include "EcsCtrlDemo.h"


NAMESPACE_UPP


EcsCtrlDemo::EcsCtrlDemo(Engine& mach) : mach(mach) {
	MaximizeBox().Sizeable();
	Title("EcsCtrlDemo");
	
	tc.Set(-1, THISBACK(MachineUpdater));
	
	Add(tabs.SizePos());
	
	tabs.Add<EntityCtrl>("Entities").SetMachine(mach);
	tabs.Add<InterfaceSystemCtrl>("Interfaces").SetMachine(mach);
	tabs.Add<InterfaceConnectionCtrl>("Connections").SetMachine(mach);
	
	Ctrl* c = this;
	data_tc.Set(-500, callback(c, &Ctrl::Update));
	
}

bool EcsCtrlDemo::InitializeDefault() {
	PoolRef p = GetPool();
	p->Add<ConnectAllInterfaces<AudioSpec>>();
	
	EntityPtr reader = p->CreateEmpty();
	reader->SetPrefab("Manual debug sound input");
	DummySoundGeneratorComponentRef sg = reader->Add<DummySoundGeneratorComponent>();
	sg->SetPreset(0);
	
	EntityPtr output = p->CreateEmpty();
	output->SetPrefab("Manual Sound output");
	PortaudioSinkComponentRef audio_out = output->Add<PortaudioSinkComponent>();
	
	return true;
}

void EcsCtrlDemo::OnError() {
	mach.SetNotRunning();
}

void EcsCtrlDemo::MachineUpdater() {
	mach.Update(ResetSeconds(ts));
}

void EcsCtrlDemo::Updated() {
	tabs.Updated();
}


END_UPP_NAMESPACE

