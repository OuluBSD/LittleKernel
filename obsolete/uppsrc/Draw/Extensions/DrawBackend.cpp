#include "Extensions.h"


NAMESPACE_UPP

RawSysImage::operator Image() const {
	TODO
	return Image();
}

#if 0

int SysImage::GetWidth() const {return raw ? raw->w : 0;}
int SysImage::GetHeight() const {return raw ? raw->h : 0;}
int SysImage::GetStride() const {return raw ? raw->ch : 0;}
int SysImage::GetPitch() const {return raw ? raw->pitch : 0;}
const unsigned char* SysImage::GetData() const {return raw ? (byte*)raw->data.Begin() : 0;}


Image StreamRaster::LoadStringAny(String content) {
	TODO // try all readers
}

Image StreamRaster::LoadFileAny(String path) {
	String ext = GetFileExt(path).Mid(1);
	Upp::StaticIfaceBackend* e = Upp::StaticIfaceFactory::GetReader(ext);
	if (e)
		return e->LoadFileAny(path);
	return Image();
}

Font Font::LoadFont(String dir, String name, int ptsize, int weight, bool italic) {
	String ext = GetFileExt(name).Mid(1);
	if (ext.IsEmpty())
		ext = "ttf";
	Upp::StaticIfaceBackend* e = Upp::StaticIfaceFactory::GetReader(ext);
	if (e)
		return e->LoadFont(dir, name, ptsize, weight, italic);
	return Font();
}

Size GetSysFontTextSize(const SysFont& fnt, const String& s) {
	auto r = Upp::StaticIfaceFactory::GetReader(fnt.raw->backend);
	if (r)
		return r->GetTextSize(fnt, s);
	return Size(0,0);
}

void SysFont::Clear() {
	if (raw) {
		auto r = Upp::StaticIfaceFactory::GetReader(raw->backend);
		if (r)
			r->ClearFont(*this);
		delete raw;
	}
	raw = 0;
}

void SysImage::Clear() {
	if (raw) {
		auto r = Upp::StaticIfaceFactory::GetReader(raw->backend);
		if (r)
			r->ClearImage(*this);
		delete raw;
	}
	raw = 0;
}

Image ImageBuffer::GetImage() {
	int len = buf.GetCount() * 4;
	
	RawSysImage* img = new RawSysImage();
	img->data.SetCount(len);
	if (len)
		memcpy(img->data.Begin(), (RGBA*)buf.Begin(), len);
	img->backend = 0;
	img->w = sz.cx;
	img->h = sz.cy;
	img->ch = 4;
	img->pitch = sz.cx * 4;
	
	return img;
}

#endif

END_UPP_NAMESPACE

