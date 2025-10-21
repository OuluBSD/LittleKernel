#ifndef _Eon_Draw_ModelCache_h_
#define _Eon_Draw_ModelCache_h_


class ModelCache :
	public System
{
	
protected:
	//CalibrationData				calib;
	ArrayMap<String, ModelLoader> model_cache;
	double time = 0;
	
protected:
    bool Initialize(const WorldState&) override;
    bool Start() override;
    void Update(double dt) override;
    void Stop() override;
    void Uninitialize() override;
    
public:
    SYS_CTOR(ModelCache)
	SYS_DEF_VISIT
	
	ModelPtr Attach(Model* mdl);
	ModelPtr GetAddModelFile(String path);
	
	//void CalibrationEvent(GeomEvent& ev);
	
};

using ModelCachePtr = Ptr<ModelCache>;


#endif
