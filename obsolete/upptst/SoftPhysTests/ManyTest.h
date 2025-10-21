#ifndef _SoftPhysTests_ManyTest_h_
#define _SoftPhysTests_ManyTest_h_


NAMESPACE_UPP


class ManyTest : public TestBase {
	
protected:
	PhysicsSystem phys;
	Space space;
	Array<RigidbodyVolume> bodies;
	RigidbodyVolume gnd;

	bool size_imgui_window;
	
protected:
	void ResetDemo();
	
public:
	void Initialize() override;
	void Update(float dt) override;
	void Refresh(GfxDataState& s) override;
	
};


END_UPP_NAMESPACE


#endif
