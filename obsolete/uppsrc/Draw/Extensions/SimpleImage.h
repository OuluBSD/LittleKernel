#ifndef _Draw_Extensions_SimpleImage_h_
#define _Draw_Extensions_SimpleImage_h_

struct FloatImage {
	float* data = 0;
	Size sz;
	int pitch = 0;
	int channels = 0;
	int size = 0;
	
public:
	typedef FloatImage CLASSNAME;
	FloatImage() : sz(0,0) {}
	~FloatImage() {Clear();}
	FloatImage(Image& img) {*this = img;}
	
	void operator=(const FloatImage& i);
	void operator=(const Nuller&) {Clear();}
	
	bool Create() {return true;} // for IGraphics compatibility
	void Set(const Image& img);
	void Set(int w, int h, int stride, int src_pitch, const byte* data);
	void Set(Size sz, int channels);
	void Clear();
	void FlipVert();
	
	int GetPitch() const {return pitch;}
	int GetWidth() const {return sz.cx;}
	int GetHeight() const {return sz.cy;}
	int GetChannels() const {return channels;}
	int GetDataSize() const {return size;}
	Size GetSize() const {return sz;}
	
	float* Begin() {return data;}
	float* End() {if (!data) return 0; return data + sz.cy * pitch;}
	float* Detach() {float* f = data; data = 0; sz = Size(0,0); pitch = 0; channels = 0; size = 0; return f;}
	
	bool IsEmpty() {return data == 0;}
	operator bool() const {return data != 0;}
	
};

typedef const FloatImage ConstFloatImage;


struct ByteImage {
	byte* data = 0;
	Size sz;
	int pitch = 0;
	int channels = 0;
	int size = 0;
	bool lock_channels = false;
	
public:
	typedef ByteImage CLASSNAME;
	ByteImage() : sz(0,0) {}
	~ByteImage() {Clear();}
	ByteImage(Image& img) {*this = img;}
	
    hash_t GetHashValue() const;
	void Visit(Vis& v);
	
	void operator=(const ByteImage& i);
	void operator=(const Image& img) {Set(img);}
	void operator=(const Nuller&) {Clear();}
	
	bool Create() {return true;} // for IGraphics compatibility
	void Set(const Image& img);
	void Set(int w, int h, int stride, int src_pitch, const byte* data);
	void Set(Size sz, int channels);
	void Resize(Size sz);
	void Clear();
	void Zero();
	void Zero(RGBA clr);
	void FlipVert();
	void Randomize();
	void SwapRedBlue();
	void SetSwapRedBlue(const ByteImage& i, bool add_alpha_ch=false);
	void LockChannels(bool b=true) {lock_channels = b;}
	void ToGrayscaleRGB();
	
	int GetPitch() const;
	int GetWidth() const;
	int GetHeight() const;
	int GetChannels() const;
	int GetLength() const;
	Size GetSize() const;
	byte* GetIter(int x, int y);
	const byte* GetIter(int x, int y) const;
	
	byte* Begin() {return data;}
	byte* End() {if (!data) return 0; return data + sz.cy * pitch;}
	byte* Detach() {byte* f = data; data = 0; sz = Size(0,0); pitch = 0; channels = 0; size = 0; return f;}
	
	bool IsEmpty() {return data == 0;}
	operator bool() const {return data != 0;}
	
};

typedef const ByteImage ConstByteImage;

void	DataFromImage(const Image& img, Vector<byte>& data);
Image	MirrorVertical(const Image& img);


template <int bytes>
struct Descriptor : Moveable<Descriptor<bytes>> {
	static const int Bytes = bytes;
	static const int Uints = bytes / 4;
	
	float x, y;
	byte angle;
	union {
		byte b[bytes];
		uint32 u[bytes / 4];
		uint64 u64[bytes / 8];
	};
};

using Descriptor32 = Descriptor<32>;

int GetDescriptor8HammingDistance(const uint32* a, const uint32* b);

class DescriptorImage {
	Vector<Descriptor32> descriptors;
	Size resolution = Size(0,0);
	
public:
	typedef DescriptorImage CLASSNAME;
	DescriptorImage();
	
	void AddDescriptor(float x, float y, float angle, void* descriptor);
	
	void SetResolution(Size sz) {resolution = sz;}
	void Clear() {descriptors.Clear(); resolution.Clear();}
	void ClearDescriptors() {descriptors.SetCount(0);}
	
	Size GetResolution() const {return resolution;}
	const Vector<Descriptor32>& GetDescriptors() const {return descriptors;}
	
	String ToString() const;
	
};


#endif
