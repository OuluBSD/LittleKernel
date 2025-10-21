#include "WebcamCV.h"

NAMESPACE_UPP

void ScharrBase::Process() {
	auto& img_u8 = tmp0;
	
    Grayscale(input, img_u8);
    
    ScharrDerivatives(img_u8, img_gxgy);
    
    OutputFromXY(img_gxgy);
}

END_UPP_NAMESPACE
