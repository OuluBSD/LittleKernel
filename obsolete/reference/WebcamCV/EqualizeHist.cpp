#include "WebcamCV.h"

NAMESPACE_UPP


void EqualizeHistBase::Process() {
    Grayscale(input, tmp0);
    
    EqualizeHistogram(tmp0, tmp1);
    
    OutputFromGray(tmp1);
}
        

END_UPP_NAMESPACE
