#include "Media.h"


NAMESPACE_UPP

String ImageLayer::StoreString() {
	PNGEncoder enc;
	String png = enc.SaveString(img);
	String bz_enc = BZ2Compress(png);
	return bz_enc;
}

void ImageLayer::LoadString(const String& bz_enc) {
	if (bz_enc.IsEmpty())
		img.Clear();
	else {
		String png = BZ2Decompress(bz_enc);
		PNGRaster dec;
		img = dec.LoadStringAny(png);
	}
}

void ImageLayer::Visit(Vis& v) {
	if (v.IsHashing()) {
		v.hash.Do(img);
	}
	else {
		v.Ver(1);
		if (v.IsStoring()) {
			String bz_png = StoreString();
			v(1)("bz_png", bz_png);
		}
		else {
			String bz_png;
			v(1)("bz_png", bz_png);
			LoadString(bz_png);
		}
	}
}

INITIALIZER_COMPONENT(ImageLayer, "photo.layer", "Image|Photo");
INITIALIZER_COMPONENT(ImageGenLayer, "photo.layer.generator", "Image|Photo");

END_UPP_NAMESPACE
