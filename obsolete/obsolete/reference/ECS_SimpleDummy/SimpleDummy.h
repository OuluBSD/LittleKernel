#ifndef _SimpleDummy_SimpleDummy_h_
#define _SimpleDummy_SimpleDummy_h_

#include <CompleteDummy/CompleteDummy.h>
using namespace Upp;

NAMESPACE_UPP



class DummyGenerator :
	public Component<DummyGenerator>
{
	VIS_COMP_0_0
	
	RefT_Entity<DummySoundGeneratorComponent>	gen;
	RefT_Entity<DummyAudioSinkComponent>		audio;
	
public:
	typedef DummyGenerator CLASSNAME;
	DummyGenerator() {}
	void OnError();
	void Initialize() override;
	void Uninitialize() override;
	void Visit(RuntimeVisitor& vis) override {COMP_DEF_VISIT_; vis & gen & audio;}
	
	COPY_PANIC(DummyGenerator);
	
};

PREFAB_BEGIN(DummyGeneratorPrefab)
	DummyAudioSinkComponent,
	DummySoundGeneratorComponent,
	DummyGenerator
PREFAB_END



END_UPP_NAMESPACE

#endif
