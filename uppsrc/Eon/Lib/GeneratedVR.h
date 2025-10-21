#ifndef _EonLib_GeneratedVR_h_
#define _EonLib_GeneratedVR_h_

// This file is generated. Do not modify this file.

#if (defined flagFREEBSD && defined flagOPENHMD) || (defined flagLINUX && defined flagOPENHMD)
class OpenHMDPipe : public OpenHMDSinkDevice {

public:
	ATOM_CTOR_(OpenHMDPipe, OpenHMDSinkDevice)
	//ATOMTYPE(OpenHMDPipe)
	static String GetAction();
	static AtomTypeCls GetAtomType();
	static LinkTypeCls GetLinkType();
	void Visit(Vis& v) override;
	AtomTypeCls GetType() const override;
	
};
#endif

#if defined flagLOCALHMD
class LocalHMDPipe : public LocalHMDSinkDevice {

public:
	ATOM_CTOR_(LocalHMDPipe, LocalHMDSinkDevice)
	//ATOMTYPE(LocalHMDPipe)
	static String GetAction();
	static AtomTypeCls GetAtomType();
	static LinkTypeCls GetLinkType();
	void Visit(Vis& v) override;
	AtomTypeCls GetType() const override;
	
};
#endif

#if (defined flagFREEBSD) || (defined flagLINUX)
class RemoteVRServerPipe : public RemoteVRServerSinkDevice {

public:
	ATOM_CTOR_(RemoteVRServerPipe, RemoteVRServerSinkDevice)
	//ATOMTYPE(RemoteVRServerPipe)
	static String GetAction();
	static AtomTypeCls GetAtomType();
	static LinkTypeCls GetLinkType();
	void Visit(Vis& v) override;
	AtomTypeCls GetType() const override;
	
};
#endif

#if (defined flagFREEBSD && defined flagHACK) || (defined flagLINUX && defined flagHACK)
class BluetoothHoloPipe : public DevBluetoothSinkDevice {

public:
	ATOM_CTOR_(BluetoothHoloPipe, DevBluetoothSinkDevice)
	//ATOMTYPE(BluetoothHoloPipe)
	static String GetAction();
	static AtomTypeCls GetAtomType();
	static LinkTypeCls GetLinkType();
	void Visit(Vis& v) override;
	AtomTypeCls GetType() const override;
	
};
#endif

#if (defined flagUWP && defined flagDX12)
class HoloContextAtom : public HoloContextBase {

public:
	ATOM_CTOR_(HoloContextAtom, HoloContextBase)
	//ATOMTYPE(HoloContextAtom)
	static String GetAction();
	static AtomTypeCls GetAtomType();
	static LinkTypeCls GetLinkType();
	void Visit(Vis& v) override;
	AtomTypeCls GetType() const override;
	
};
#endif

#if (defined flagUWP && defined flagDX12)
class HoloEventAtomPipe : public HoloEventsBase {

public:
	ATOM_CTOR_(HoloEventAtomPipe, HoloEventsBase)
	//ATOMTYPE(HoloEventAtomPipe)
	static String GetAction();
	static AtomTypeCls GetAtomType();
	static LinkTypeCls GetLinkType();
	void Visit(Vis& v) override;
	AtomTypeCls GetType() const override;
	
};
#endif

#if (defined flagUWP && defined flagDX12 && defined flagDX12)
class HoloD12FboAtomSA : public HoloD12VideoSinkDevice {

public:
	ATOM_CTOR_(HoloD12FboAtomSA, HoloD12VideoSinkDevice)
	//ATOMTYPE(HoloD12FboAtomSA)
	static String GetAction();
	static AtomTypeCls GetAtomType();
	static LinkTypeCls GetLinkType();
	void Visit(Vis& v) override;
	AtomTypeCls GetType() const override;
	
};
#endif

#endif
