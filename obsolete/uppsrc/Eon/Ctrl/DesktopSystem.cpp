#include "EonCtrl.h"


NAMESPACE_UPP


void DesktopSuiteSystem::Visit(Vis& vis) {
	
}

bool DesktopSuiteSystem::Arg(String key, Value value) {
	
	if (key == "program") {
		String program = value;
		Vector<String> progs = Split(program, ";");
		for (String prog : progs) {
			auto item = DesktopFactory::Find(prog);
			if (!item) {
				LOG("DesktopSuiteSystem::Arg: error: no program '" << prog << "' available");
				return false;
			}
			TopWindow* tw = item->New();
			apps.Add(tw);
		}
	}
	
	return true;
}

bool DesktopSuiteSystem::Initialize(const WorldState& ws) {
	
	
	return true;
}

void DesktopSuiteSystem::Start() {
	
}

void DesktopSuiteSystem::Update(double dt) {
	
}

void DesktopSuiteSystem::Stop() {
	
}

void DesktopSuiteSystem::Uninitialize() {
	apps.Clear();
}


END_UPP_NAMESPACE
