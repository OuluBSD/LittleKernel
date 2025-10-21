#ifndef _Local_Image_h_
#define _Local_Image_h_

NAMESPACE_UPP

class Ether;


enum {
	IMAGEID_GRID,
	
	IMAGEID_COUNT
};

Image GetDefaultImage(int code);
Image RealizeImage(Image& img, String path);
void SetCenterHotSpot(Image& img);

struct DefaultImages {
	static Image Arrow;
};


END_UPP_NAMESPACE

#endif
