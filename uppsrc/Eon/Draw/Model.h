#ifndef _Eon_Draw_Model_h_
#define _Eon_Draw_Model_h_


class Renderable : public Component {
	
public:
	ECS_COMPONENT_CTOR(Renderable);
	RGBA color;
	mat4 offset;
	float alpha_multiplier;
	
	
	void Visit(Vis& v) override;
	bool Initialize(const WorldState&) override;
	void Uninitialize() override;
	
	void ResetModel(mat4 offset = zero<mat4>()) {
		color = RGBAZero();
		this->offset = offset;
		alpha_multiplier = 0;
	}
	
    void operator=(const Renderable& e) {
        color = e.color;
        offset = e.offset;
        alpha_multiplier = e.alpha_multiplier;
    }
    
    
	Callback cb;
	
	
};

using RenderablePtr = Ptr<Renderable>;


typedef Tuple<Ptr<Model>, TransformPtr, RenderablePtr> RendModel;
typedef Vector<RendModel> VectorRendModel;


class ModelComponent :
	public Component
{
	
public:
	ECS_COMPONENT_CTOR(ModelComponent)
	void Visit(Vis& v) override;
	bool Initialize(const WorldState&) override;
	void Uninitialize() override;
	bool Arg(String key, Value value) override;
	void SetEnabled(bool enable) override;
    void operator=(const ModelComponent& src) {}
    
    bool Load(GfxDataState& state);
    bool LoadModel(String path);
    bool AddTextureFile(int mesh_i, TexType type, String path);
	Ptr<Model> GetModel() {return loader.GetModel();}
    ModelLoader& GetLoader() {return loader;}
    
	void GetModels(VectorRendModel& models);
	void Attach(Model* m) {loader.model = m;}
	void Clear();
	void SetPrefabModel(String prefab); // ResetModel
	
	
public:
	void SetRotation(float yaw, float pitch, float roll);
	void SetTranslation(const vec3& v);
	void SetScale(const vec3& v);
	void SetModelChanged() {model_changed = true;}
	void SetModel(ModelPtr m);
	void SetModelMatrix(const mat4& m);
	
	void Create();
	void MakeBall(const vec3& pos, float radius);
	void MakeCylinder(const vec3& pos, float radius, float length);
	
public:
	vec4 color;
	String prefab_name;
	String skybox_diffuse, skybox_irradiance;
	bool load_skybox = false;
	bool always_enabled = false;
	bool dbg = false;
	
protected:
	Ptr<Model> model;
	GfxDataState* gfx_state = 0;
	
	ModelLoader loader;
	
	hash_t gfx_hash = 0;
	vec3 offset = zero<vec3>();
	vec3 scale = one<vec3>();
	float pitch = 0, yaw = 0, roll = 0;
	mat4 ext_model;
	bool have_ext_model = false;
	bool model_changed = false;
	
	void RefreshExtModel();
	
	
};

using ModelComponentPtr = Ptr<ModelComponent>;


#endif
