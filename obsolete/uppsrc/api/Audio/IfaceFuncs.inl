// This file have been generated automatically.
// DO NOT MODIFY THIS FILE!

static bool SinkDevice_Create(NativeSinkDevice*& dev);
static void SinkDevice_Destroy(NativeSinkDevice*& dev);
static bool SinkDevice_Initialize(NativeSinkDevice&, AtomBase&, const WorldState&);
static bool SinkDevice_PostInitialize(NativeSinkDevice&, AtomBase&);
static bool SinkDevice_Start(NativeSinkDevice&, AtomBase&);
static void SinkDevice_Stop(NativeSinkDevice&, AtomBase&);
static void SinkDevice_Uninitialize(NativeSinkDevice&, AtomBase&);
static bool SinkDevice_Send(NativeSinkDevice&, AtomBase&, RealtimeSourceConfig& cfg, PacketValue& out, int src_ch);
static void SinkDevice_Visit(NativeSinkDevice&, AtomBase&, Vis& vis);
static bool SinkDevice_NegotiateSinkFormat(NativeSinkDevice&, AtomBase&, LinkBase& link, int sink_ch, const ValueFormat& new_fmt);

static bool SourceDevice_Create(NativeSourceDevice*& dev);
static void SourceDevice_Destroy(NativeSourceDevice*& dev);
static bool SourceDevice_Initialize(NativeSourceDevice&, AtomBase&, const WorldState&);
static bool SourceDevice_PostInitialize(NativeSourceDevice&, AtomBase&);
static bool SourceDevice_Start(NativeSourceDevice&, AtomBase&);
static void SourceDevice_Stop(NativeSourceDevice&, AtomBase&);
static void SourceDevice_Uninitialize(NativeSourceDevice&, AtomBase&);
static bool SourceDevice_Send(NativeSourceDevice&, AtomBase&, RealtimeSourceConfig& cfg, PacketValue& out, int src_ch);
static void SourceDevice_Visit(NativeSourceDevice&, AtomBase&, Vis& vis);
static bool SourceDevice_NegotiateSinkFormat(NativeSourceDevice&, AtomBase&, LinkBase& link, int sink_ch, const ValueFormat& new_fmt);

