#pragma once

#if 1


class PaintStrokeComponent :
	public Component {
	
public:
	ECS_COMPONENT_CTOR(PaintStrokeComponent)
	Vector<Square> squares;
	bool stroke_changed = false;
	
	
	void Visit(Vis& v) override;
	bool Initialize(const WorldState&) override;
	void Uninitialize() override;
	void AddPoint(const mat4& trans_mtx, float width);
	
};


class PaintStrokeSystemBase :
	public System
{
public:
	ECS_SYS_CTOR(PaintStrokeSystemBase);
	ECS_SYS_DEF_VISIT
	
	~PaintStrokeSystemBase() = default;
	
	void Attach(PaintStrokeComponent* comp);
	void Detach(PaintStrokeComponent* comp);
	
protected:
	void Update(double) override;
	bool Initialize(const WorldState&) override;
	
private:
	Vector<PaintStrokeComponent*> comps;
	WorldState ws_at_init;
	
};


#endif
