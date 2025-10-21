#ifndef _Eon_Core_Types_h_
#define _Eon_Core_Types_h_


struct Atom;
class AtomStore;
class AtomSystem;
class SoundSample;
class Machine;


typedef enum {
	SIDE_NOT_ACCEPTED,
	SIDE_ACCEPTED,
} SideStatus;

namespace FboKbd {

static const int key_tex_w = 256;
static const int key_tex_h = 1;//256;
typedef FixedArray<byte, 256> KeyVec;

}


struct GfxDataState;

struct RendererContent {
	virtual bool Load(GfxDataState& state) = 0;
	
	static Vector<RendererContent*>& Content() {static Vector<RendererContent*> v; return v;}
	static void AddContent(RendererContent* r) {VectorFindAdd(Content(), r);}
	static void RemoveContent(RendererContent* r) {VectorRemoveKey(Content(), r);}
	
};


#endif
