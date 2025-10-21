#include "WebcamCV.h"

NAMESPACE_UPP

void SobelBase::Process() {
	auto& img_u8 = tmp0;
	
    Grayscale(input, img_u8);
    
    SobelDerivatives(img_u8, img_gxgy);
    
    OutputFromXY(img_gxgy);
}

END_UPP_NAMESPACE
