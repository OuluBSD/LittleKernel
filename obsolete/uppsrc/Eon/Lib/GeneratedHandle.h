#ifndef _EonLib_GeneratedHandle_h_
#define _EonLib_GeneratedHandle_h_

// This file is generated. Do not modify this file.

#if defined flagSCREEN
class HandleProgEvents : public HandleEventsBase {

public:
	ATOM_CTOR_(HandleProgEvents, HandleEventsBase)
	//ATOMTYPE(HandleProgEvents)
	static String GetAction();
	static AtomTypeCls GetAtomType();
	static LinkTypeCls GetLinkType();
	void Visit(Vis& v) override;
	AtomTypeCls GetType() const override;
	
};
#endif

#if defined flagSCREEN
class CenterProgPipe : public HandleVideoBase {

public:
	ATOM_CTOR_(CenterProgPipe, HandleVideoBase)
	//ATOMTYPE(CenterProgPipe)
	static String GetAction();
	static AtomTypeCls GetAtomType();
	static LinkTypeCls GetLinkType();
	void Visit(Vis& v) override;
	AtomTypeCls GetType() const override;
	
};
#endif

#if defined flagSCREEN
class OglProgPipe : public HandleVideoBase {

public:
	ATOM_CTOR_(OglProgPipe, HandleVideoBase)
	//ATOMTYPE(OglProgPipe)
	static String GetAction();
	static AtomTypeCls GetAtomType();
	static LinkTypeCls GetLinkType();
	void Visit(Vis& v) override;
	AtomTypeCls GetType() const override;
	
};
#endif

#if defined flagSCREEN
class HandleProgVideo : public HandleVideoBase {

public:
	ATOM_CTOR_(HandleProgVideo, HandleVideoBase)
	//ATOMTYPE(HandleProgVideo)
	static String GetAction();
	static AtomTypeCls GetAtomType();
	static LinkTypeCls GetLinkType();
	void Visit(Vis& v) override;
	AtomTypeCls GetType() const override;
	
};
#endif

#endif
