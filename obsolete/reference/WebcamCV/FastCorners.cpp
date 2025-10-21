#include "WebcamCV.h"


NAMESPACE_UPP


void FastCornersBase::SetSize(Size sz) {
	ASSERT(!sz.IsEmpty());
	
	this->sz = sz;
	
	corners.SetCount(sz.cx * sz.cy);
	for (Keypoint& k : corners)
		k.Clear();
	
}

void FastCornersBase::Process() {
    Grayscale(input, tmp0);
    
    c.set_threshold(threshold);
    
    int count = c.Detect(tmp0, corners, 5);
    
    // render result back to canvas
    RenderCorners(corners, output);
}


END_UPP_NAMESPACE

