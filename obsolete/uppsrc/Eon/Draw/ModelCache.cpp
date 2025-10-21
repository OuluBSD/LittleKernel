#include "Draw.h"


NAMESPACE_UPP




ModelPtr ModelCache::GetAddModelFile(String path) {
	int i = model_cache.Find(path);
	if (i >= 0)
		return model_cache[i].GetModel();
	ModelLoader& l = model_cache.Add(path);
	l.LoadModel(path);
	return l.GetModel();
}

ModelPtr ModelCache::Attach(Model* mdl) {
	if (!mdl)
		return ModelPtr();
	
	if (mdl->path.IsEmpty())
		mdl->path = IntStr64(mdl->GetHashValue());
	
	int i = model_cache.Find(mdl->path);
	if (i >= 0) {
		delete mdl;
		return model_cache[i].GetModel();
	}
	
	ModelLoader& ml = model_cache.Add(mdl->path);
	ml.Attach(mdl);
	return ml.GetModel();
}


bool ModelCache::Initialize(const WorldState& ws) {
	return true;
}

bool ModelCache::Start() {
	return true;
}

void ModelCache::Update(double dt) {
	time += dt;
	
}

void ModelCache::Stop() {
	
}

void ModelCache::Uninitialize() {
	
}


END_UPP_NAMESPACE
