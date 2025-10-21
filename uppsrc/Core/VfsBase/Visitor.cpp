#include "VfsBase.h"

NAMESPACE_UPP

void Vis::ChkSerializeMagic() {
	#define HAVE_CHK_NODEVISITOR_MAGIC 0
	#if HAVE_CHK_NODEVISITOR_MAGIC
	ASSERT(mode == MODE_STREAM);
	if (storing) {
		byte magic = 0x55;
		stream->Put(&magic, 1);
	}
	else {
		byte magic = 0;
		stream->Get(&magic, 1);
		ASSERT(magic == 0x55);
	}
	#endif
}

END_UPP_NAMESPACE
