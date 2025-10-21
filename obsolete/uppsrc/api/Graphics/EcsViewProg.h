#ifndef _IGraphics_EcsViewProg_h_
#define _IGraphics_EcsViewProg_h_

NAMESPACE_UPP


struct EcsViewProg :
	public BinderIfaceVideo
{
	//RTTI_DECL1(EcsViewProg, BinderIfaceVideo)
	
	EcsViewProg();
	void operator=(const EcsViewProg& t) {Panic("Can't copy EcsViewProgT");}
	void Visit(Vis& v) override {}
	bool Initialize(const WorldState&) override;
	void Uninitialize() override;
	bool Render(Draw& draw) override;
	bool Arg(const String& key, const String& value) override;
	
};


END_UPP_NAMESPACE

#endif
