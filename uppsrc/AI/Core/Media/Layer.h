#ifndef _AICore_Layer_h_
#define _AICore_Layer_h_



struct ImageLayer : Component {
	Image img;
	
	CLASSTYPE(ImageLayer)
	ImageLayer(VfsValue& owner) : Component(owner) {}
	~ImageLayer(){}
	String StoreString();
	void LoadString(const String& bz_enc);
	void Visit(Vis& v) override;
	
};

INITIALIZE(ImageLayer);

struct TempImageLayer : Component {
	Vector<Image> imgs;
	
	CLASSTYPE(TempImageLayer)
	TempImageLayer(VfsValue& owner) : Component(owner) {}
	~TempImageLayer(){}
	void Visit(Vis& v) override {}
};

struct ImageGenLayer : TempImageLayer {
	CLASSTYPE(ImageGenLayer)
	ImageGenLayer(VfsValue& owner) : TempImageLayer(owner) {}
	
};

INITIALIZE(ImageGenLayer);



#endif
