#include "WebcamCV.h"


NAMESPACE_UPP


void BoxBlurBase::Process() {
	Grayscale(input, tmp0);
	BoxBlurGray(tmp0, tmp1, radius, 0);
	OutputFromGray(tmp1);
}


END_UPP_NAMESPACE
