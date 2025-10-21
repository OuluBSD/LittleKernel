#include "Graphics.h"


NAMESPACE_UPP




ProgImage::ProgImage() {
	
}

void ProgImage::Paint(const DrawCommand* begin, const DrawCommand* end, ImagePainter& id) {
	draw.SetTarget(&id);
	draw.Process(begin, end);
}

void ProgImage::SkipWindowCommands(bool b) {
	draw.SkipWindowCommands(b);
}


//SDLCPU_EXCPLICIT_INITIALIZE_CLASS(ProgImage)


END_UPP_NAMESPACE

