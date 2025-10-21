#include "Lib.h"

// This file is generated. Do not modify this file.

NAMESPACE_UPP


#if defined flagSCREEN
String HandleProgEvents::GetAction() {
	return "center.events.prog.ecs";
}

AtomTypeCls HandleProgEvents::GetAtomType() {
	AtomTypeCls t;
	t.sub = SUB_ATOM_CLS;
	t.role = AtomRole::PIPE;
	t.AddIn(VD(CENTER,RECEIPT),0);
	t.AddOut(VD(CENTER,RECEIPT),0);
	return t;
}

LinkTypeCls HandleProgEvents::GetLinkType() {
	return LINKTYPE(PIPE, PROCESS);
}

void HandleProgEvents::Visit(Vis& v) {
	VIS_THIS(HandleEventsBase);
}

AtomTypeCls HandleProgEvents::GetType() const {
	return GetAtomType();
}
#endif


#if defined flagSCREEN
String CenterProgPipe::GetAction() {
	return "center.video.prog.pipe";
}

AtomTypeCls CenterProgPipe::GetAtomType() {
	AtomTypeCls t;
	t.sub = SUB_ATOM_CLS; //CENTER_PROG_PIPE;
	t.role = AtomRole::PIPE;
	t.AddIn(VD(CENTER,ORDER),0);
	t.AddOut(VD(CENTER,PROG),0);
	return t;
}

LinkTypeCls CenterProgPipe::GetLinkType() {
	return LINKTYPE(PIPE, PROCESS);
}

void CenterProgPipe::Visit(Vis& v) {
	VIS_THIS(HandleVideoBase);
}

AtomTypeCls CenterProgPipe::GetType() const {
	return GetAtomType();
}
#endif


#if defined flagSCREEN
String OglProgPipe::GetAction() {
	return "ogl.prog.pipe";
}

AtomTypeCls OglProgPipe::GetAtomType() {
	AtomTypeCls t;
	t.sub = SUB_ATOM_CLS; //OGL_PROG_PIPE;
	t.role = AtomRole::PIPE;
	t.AddIn(VD(OGL,ORDER),0);
	t.AddOut(VD(OGL,PROG),0);
	return t;
}

LinkTypeCls OglProgPipe::GetLinkType() {
	return LINKTYPE(PIPE, PROCESS);
}

void OglProgPipe::Visit(Vis& v) {
	VIS_THIS(HandleVideoBase);
}

AtomTypeCls OglProgPipe::GetType() const {
	return GetAtomType();
}
#endif


#if defined flagSCREEN
String HandleProgVideo::GetAction() {
	return "center.video.prog.ecs";
}

AtomTypeCls HandleProgVideo::GetAtomType() {
	AtomTypeCls t;
	t.sub = SUB_ATOM_CLS; //HANDLE_PROG_VIDEO;
	t.role = AtomRole::PIPE;
	t.AddIn(VD(CENTER,ORDER),0);
	t.AddOut(VD(CENTER,RECEIPT),0);
	return t;
}

LinkTypeCls HandleProgVideo::GetLinkType() {
	return LINKTYPE(PIPE, PROCESS);
}

void HandleProgVideo::Visit(Vis& v) {
	VIS_THIS(HandleVideoBase);
}

AtomTypeCls HandleProgVideo::GetType() const {
	return GetAtomType();
}
#endif

END_UPP_NAMESPACE

