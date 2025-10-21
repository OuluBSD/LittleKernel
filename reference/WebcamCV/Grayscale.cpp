#include "WebcamCV.h"


NAMESPACE_UPP


void GrayscaleBase::Process() {
	Grayscale(input, tmp0);
	OutputFromGray(tmp0);
}


END_UPP_NAMESPACE
