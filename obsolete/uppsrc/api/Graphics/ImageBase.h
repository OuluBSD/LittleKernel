#ifndef _IGraphics_Image_h_
#define _IGraphics_Image_h_

NAMESPACE_UPP


template <class Gfx>
struct ImageBaseAtomT :
	Atom
{
	Vector<Image>				imgs;
	String						last_error;
	dword						seq = 0;
	bool						cubemap = false;
	bool						vflip = false;
	bool						swap_top_bottom = false;
	
public:
	using CLASSNAME = ImageBaseAtomT<Gfx>;
	TypeCls GetTypeCls() const override {return typeid(CLASSNAME);}
	COPY_PANIC(ImageBaseAtomT)
	
	ImageBaseAtomT(VfsValue& n);
	
	void			Visit(Vis& v) override {VIS_THIS(Atom);}
	bool			Initialize(const WorldState& ws) override;
	bool			PostInitialize() override;
	void			Uninitialize() override;
	bool			Send(RealtimeSourceConfig& cfg, PacketValue& out, int src_ch) override;
	bool			IsReady(PacketIO& io) override;
	
	Size			GetResolution() const;
	
	bool			IsOpen() const {return !imgs.IsEmpty();}
	bool			Open(int) {return !imgs.IsEmpty();}
	void			Close() {imgs.Clear();}
	bool			ReadFrame() {return true;}
	bool			ProcessFrame() {return true;}
	void			ClearPacketData() {}
	
	String GetLastError() const {return last_error;}
	
	
};


#if defined flagSDL2 && defined flagOGL
using SdlOglImageBase = ImageBaseAtomT<SdlOglGfx>;
#endif


END_UPP_NAMESPACE

#endif
