#ifndef _Kernel_VirtioNet_h_
#define _Kernel_VirtioNet_h_

#include "Common.h"
#include "Defs.h"
#include "Virtio.h"
#include "DriverFramework.h"
#include "NetworkDriverInterface.h"

// Virtio network device feature bits
#define VIRTIO_NET_F_CSUM          (1 << 0)   // Device handles checksums
#define VIRTIO_NET_F_GUEST_CSUM    (1 << 1)   // Guest handles checksums
#define VIRTIO_NET_F_CTRL_GUEST_OFFLOADS (1 << 2) // Control channel offloads
#define VIRTIO_NET_F_MAC           (1 << 5)   // Device has given MAC address
#define VIRTIO_NET_F_GUEST_TSO4    (1 << 7)   // Guest can handle TSOv4
#define VIRTIO_NET_F_GUEST_TSO6    (1 << 8)   // Guest can handle TSOv6
#define VIRTIO_NET_F_GUEST_ECN     (1 << 9)   // Guest can handle TSO with ECN
#define VIRTIO_NET_F_GUEST_UFO     (1 << 10)  // Guest can handle UFO
#define VIRTIO_NET_F_HOST_TSO4     (1 << 11)  // Host can handle TSOv4
#define VIRTIO_NET_F_HOST_TSO6     (1 << 12)  // Host can handle TSOv6
#define VIRTIO_NET_F_HOST_ECN      (1 << 13)  // Host can handle TSO with ECN
#define VIRTIO_NET_F_HOST_UFO      (1 << 14)  // Host can handle UFO
#define VIRTIO_NET_F_MRG_RXBUF     (1 << 15)  // Guest can merge receive buffers
#define VIRTIO_NET_F_STATUS        (1 << 16)  // Device reports link status
#define VIRTIO_NET_F_CTRL_VQ       (1 << 17)  // Control channel available
#define VIRTIO_NET_F_CTRL_RX       (1 << 18)  // Control channel RX mode
#define VIRTIO_NET_F_CTRL_VLAN     (1 << 19)  // Control channel VLAN filtering
#define VIRTIO_NET_F_CTRL_RX_EXTRA (1 << 20)  // Extra RX mode control
#define VIRTIO_NET_F_GUEST_ANNOUNCE (1 << 21) // Guest can announce device
#define VIRTIO_NET_F_MQ            (1 << 22)  // Device supports multiqueue
#define VIRTIO_NET_F_CTRL_MAC_ADDR (1 << 23)  // Set MAC address

// Virtio network packet header
struct VirtioNetHeader {
    uint8 flags;           // Flags (VIRTIO_NET_HDR_F_*)
    uint8 gso_type;        // GSO type (VIRTIO_NET_HDR_GSO_*)
    uint16_t hdr_len;        // Ethernet + IP + TCP/UDP header length
    uint16_t gso_size;       // GSO segment size
    uint16_t csum_start;     // Checksum start offset
    uint16_t csum_offset;    // Checksum offset from csum_start
    uint16_t num_buffers;    // Number of merged buffers (if VIRTIO_NET_F_MRG_RXBUF)
};

// Virtio network header flags
#define VIRTIO_NET_HDR_F_NEEDS_CSUM    1  // Use csum_start and csum_offset
#define VIRTIO_NET_HDR_F_DATA_VALID    2  // Data is valid (checksummed)

// Virtio network GSO types
#define VIRTIO_NET_HDR_GSO_NONE        0
#define VIRTIO_NET_HDR_GSO_TCPV4       1
#define VIRTIO_NET_HDR_GSO_UDP         3
#define VIRTIO_NET_HDR_GSO_TCPV6       4
#define VIRTIO_NET_HDR_GSO_ECN         0x80

// Virtio network configuration structure
struct VirtioNetConfig {
    uint8 mac[6];          // MAC address
    uint16_t status;         // Link status
    uint16_t max_virtqueue_pairs; // Max number of queue pairs
    uint16_t mtu;            // Maximum transmission unit
    uint32 speed;          // Link speed in Mbps
    uint8 duplex;          // Duplex (0 = half, 1 = full)
    uint8 rss_max_key_size; // Max RSS key size
    uint16_t rss_max_indirection_table_length; // Max RSS indirection table length
    uint32 supported_hash_types; // Supported hash types
};

// Virtio network statistics
struct VirtioNetStats {
    uint64_t tx_packets;     // Transmitted packets
    uint64_t tx_bytes;       // Transmitted bytes
    uint64_t rx_packets;     // Received packets
    uint64_t rx_bytes;       // Received bytes
    uint64_t tx_errors;      // Transmit errors
    uint64_t rx_errors;      // Receive errors
    uint64_t rx_dropped;     // Dropped packets
    uint64_t tx_dropped;     // Dropped packets
    uint64_t multicast;      // Multicast packets
    uint64_t collisions;     // Collisions
};

// Virtio network device driver class
class VirtioNetDriver : public NetworkDriver, public VirtioDriver {
private:
    VirtioNetConfig config;           // Device configuration
    VirtioNetStats stats;            // Network statistics
    uint8 mac_address[6];          // MAC address
    uint32 mtu;                    // Maximum transmission unit
    bool link_up;                    // Whether link is up
    uint32 speed;                  // Link speed in Mbps
    bool full_duplex;                // Whether link is full duplex
    uint32 max_queue_pairs;        // Maximum queue pairs
    uint32 num_queue_pairs;        // Actual number of queue pairs
    uint32 rx_queue;               // Receive queue index
    uint32 tx_queue;               // Transmit queue index
    uint32 ctrl_queue;             // Control queue index (if available)
    bool checksum_offload;           // Whether checksum offload is supported
    bool tso_support;                // Whether TCP segmentation offload is supported
    bool ufo_support;                // Whether UDP fragmentation offload is supported
    bool vlan_filtering;             // Whether VLAN filtering is supported
    bool multiqueue;                 // Whether multiqueue is supported
    bool rss_support;                // Whether receive side scaling is supported
    char device_name[32];            // Device name
    RingBuffer<NetworkPacket, 256> rx_packet_buffer;  // Receive packet buffer
    RingBuffer<NetworkPacket, 256> tx_packet_buffer;  // Transmit packet buffer
    Spinlock net_lock;               // Lock for network operations
    uint32 packet_id_counter;      // Counter for packet IDs

public:
    VirtioNetDriver(const char* driver_name, const char* driver_version, 
                    uint32 vid = 0, uint32 did = 0, uint32 irq = 0);
    virtual ~VirtioNetDriver();
    
    // Implement required virtual functions from NetworkDriver
    virtual DriverInitResult Initialize() override;
    virtual int Shutdown() override;
    virtual int HandleInterrupt() override;
    virtual int ProcessIoRequest(IoRequest* request) override;
    virtual int SendPacket(const void* packet, uint32 size) override;
    virtual int ReceivePacket(void* packet, uint32 max_size) override;
    virtual const uint8* GetMacAddress() const override { return mac_address; }
    virtual uint32 GetMTU() const override { return mtu; }
    virtual bool IsLinkUp() const override { return link_up; }
    virtual void SetLinkState(bool up) override { link_up = up; }
    virtual uint32 GetLinkSpeed() const override { return speed; }
    virtual bool IsFullDuplex() const override { return full_duplex; }
    virtual void GetNetworkStats(NetworkStats& stats_out) override;
    virtual bool SetMacAddress(const uint8* mac) override;
    virtual bool SetPromiscuousMode(bool promiscuous) override;
    virtual bool SetMulticastFilter(const uint8* multicast_list, uint32 count) override;
    virtual bool ConfigureOffload(uint32 offload_features) override;
    virtual bool EnableMultiqueue(uint32 num_queues) override;
    virtual bool DisableMultiqueue() override;
    virtual bool EnableRSS(const uint8* key, uint32 key_size, 
                          const uint32* indirection_table, uint32 table_size) override;
    virtual bool DisableRSS() override;
    virtual bool FlushRxBuffer() override;
    virtual bool FlushTxBuffer() override;
    virtual uint32 GetRxBufferSize() override;
    virtual uint32 GetTxBufferSize() override;
    virtual bool SetMTU(uint32 new_mtu) override;
    virtual bool GetLinkStatus(NetworkLinkStatus& status) override;
    virtual bool SetLinkParameters(uint32 speed, bool full_duplex) override;
    virtual bool EnableWakeOnLan(WakeOnLanMode mode) override;
    virtual bool DisableWakeOnLan() override;
    virtual bool GetWakeOnLanStatus(WakeOnLanStatus& status) override;
    virtual bool SetVlanFilter(uint16_t vlan_id, bool enable) override;
    virtual bool GetVlanFilter(uint16_t vlan_id, bool& enabled) override;
    virtual bool EnableVlanFiltering() override;
    virtual bool DisableVlanFiltering() override;
    virtual bool SetCoalesceParameters(const NetworkCoalesceParams& params) override;
    virtual bool GetCoalesceParameters(NetworkCoalesceParams& params) override;
    virtual bool SetRingParameters(const NetworkRingParams& params) override;
    virtual bool GetRingParameters(NetworkRingParams& params) override;
    virtual bool SetChannelParameters(const NetworkChannelParams& params) override;
    virtual bool GetChannelParameters(NetworkChannelParams& params) override;
    virtual bool PauseTx() override;
    virtual bool ResumeTx() override;
    virtual bool PauseRx() override;
    virtual bool ResumeRx() override;
    virtual bool ResetStats() override;
    virtual bool GetExtendedStats(NetworkExtendedStats& stats) override;
    virtual bool SetFlowControl(FlowControlMode mode) override;
    virtual bool GetFlowControl(FlowControlMode& mode) override;
    virtual bool EnableNapi() override;
    virtual bool DisableNapi() override;
    virtual bool SetNapiWeight(uint32 weight) override;
    virtual uint32 GetNapiWeight() override;
    virtual bool EnableChecksumOffload(ChecksumOffloadType type) override;
    virtual bool DisableChecksumOffload(ChecksumOffloadType type) override;
    virtual bool GetChecksumOffloadStatus(ChecksumOffloadType type, bool& enabled) override;
    virtual bool EnableTSO() override;
    virtual bool DisableTSO() override;
    virtual bool GetTSOStatus(bool& enabled) override;
    virtual bool EnableUFO() override;
    virtual bool DisableUFO() override;
    virtual bool GetUFOStatus(bool& enabled) override;
    virtual bool EnableGRO() override;
    virtual bool DisableGRO() override;
    virtual bool GetGROStatus(bool& enabled) override;
    virtual bool EnableLRO() override;
    virtual bool DisableLRO() override;
    virtual bool GetLROStatus(bool& enabled) override;
    virtual bool SetInterruptModeration(uint32 usec) override;
    virtual uint32 GetInterruptModeration() override;
    virtual bool SetRxBufferParams(const NetworkRxBufferParams& params) override;
    virtual bool GetRxBufferParams(NetworkRxBufferParams& params) override;
    virtual bool SetTxBufferParams(const NetworkTxBufferParams& params) override;
    virtual bool GetTxBufferParams(NetworkTxBufferParams& params) override;
    virtual bool EnableHardwareTimestamping(HardwareTimestampingMode mode) override;
    virtual bool DisableHardwareTimestamping() override;
    virtual bool GetHardwareTimestampingStatus(HardwareTimestampingStatus& status) override;
    virtual bool SetJumboFrames(bool enable, uint32 max_frame_size) override;
    virtual bool GetJumboFramesStatus(bool& enabled, uint32& max_frame_size) override;
    virtual bool EnableVxlanOffload() override;
    virtual bool DisableVxlanOffload() override;
    virtual bool GetVxlanOffloadStatus(bool& enabled) override;
    virtual bool EnableGeneveOffload() override;
    virtual bool DisableGeneveOffload() override;
    virtual bool GetGeneveOffloadStatus(bool& enabled) override;
    virtual bool EnableTunnelOffload(TunnelProtocol protocol) override;
    virtual bool DisableTunnelOffload(TunnelProtocol protocol) override;
    virtual bool GetTunnelOffloadStatus(TunnelProtocol protocol, bool& enabled) override;
    virtual bool EnableRsc(RscMode mode) override;
    virtual bool DisableRsc() override;
    virtual bool GetRscStatus(RscMode& mode) override;
    virtual bool EnableNtupleFiltering() override;
    virtual bool DisableNtupleFiltering() override;
    virtual bool GetNtupleFilteringStatus(bool& enabled) override;
    virtual bool AddNtupleFilter(const NtupleFilterRule& rule) override;
    virtual bool RemoveNtupleFilter(uint32 filter_id) override;
    virtual bool GetNtupleFilter(uint32 filter_id, NtupleFilterRule& rule) override;
    virtual bool EnableArpOffload() override;
    virtual bool DisableArpOffload() override;
    virtual bool GetArpOffloadStatus(bool& enabled) override;
    virtual bool EnableNsOffload() override;
    virtual bool DisableNsOffload() override;
    virtual bool GetNsOffloadStatus(bool& enabled) override;
    virtual bool EnableTcpSegOffload() override;
    virtual bool DisableTcpSegOffload() override;
    virtual bool GetTcpSegOffloadStatus(bool& enabled) override;
    virtual bool EnableUdpTnlOffload() override;
    virtual bool DisableUdpTnlOffload() override;
    virtual bool GetUdpTnlOffloadStatus(bool& enabled) override;
    virtual bool EnableIpsecOffload() override;
    virtual bool DisableIpsecOffload() override;
    virtual bool GetIpsecOffloadStatus(bool& enabled) override;
    virtual bool EnableSctpOffload() override;
    virtual bool DisableSctpOffload() override;
    virtual bool GetSctpOffloadStatus(bool& enabled) override;
    
    // Implement required virtual functions from VirtioDriver
    virtual bool NegotiateFeatures(uint64_t device_features) override;
    virtual bool SetupQueues(uint32 queue_count) override;
    virtual bool InitializeQueue(uint32 queue_index, uint16_t queue_size) override;
    virtual bool CleanupQueues() override;
    virtual bool SendBuffer(uint32 queue_index, void* buffer, uint32 size) override;
    virtual bool ReceiveBuffer(uint32 queue_index, void** buffer, uint32* size) override;
    virtual uint32 GetQueueSize(uint32 queue_index) override;
    virtual bool NotifyQueue(uint32 queue_index) override;
    virtual bool ResetDevice() override;
    virtual bool SetStatus(uint8 status) override;
    virtual uint8 GetStatus() override;
    virtual bool SetDriverFeatures(uint64_t features) override;
    virtual uint64_t GetDeviceFeatures() override;
    virtual uint32 GetConfigGeneration() override;
    virtual bool ReadConfig(uint32 offset, void* buffer, uint32 size) override;
    virtual bool WriteConfig(uint32 offset, const void* buffer, uint32 size) override;
    
    // Network-specific functions
    bool GetDeviceConfig();
    bool UpdateLinkStatus();
    bool ProcessReceivedPacket(void* packet, uint32 size);
    bool PrepareTransmitPacket(const void* packet, uint32 size);
    bool CompleteTransmitOperation();
    bool HandleControlQueue();
    bool SendControlCommand(uint32 command, const void* data, uint32 size);
    bool ReceiveControlResponse(void* data, uint32 size);
    bool SetMacAddressViaControl(const uint8* mac);
    bool SetPromiscuousModeViaControl(bool promiscuous);
    bool SetMulticastFilterViaControl(const uint8* multicast_list, uint32 count);
    bool EnableMultiqueueViaControl(uint32 num_queues);
    bool DisableMultiqueueViaControl();
    bool EnableRSSViaControl(const uint8* key, uint32 key_size, 
                            const uint32* indirection_table, uint32 table_size);
    bool DisableRSSViaControl();
    bool AnnounceDevice();
    bool GetExtendedDeviceConfig();
    bool SetOffloadFeatures(uint32 features);
    bool GetOffloadFeatures(uint32& features);
    bool SetCoalesceParams(const NetworkCoalesceParams& params);
    bool GetCoalesceParams(NetworkCoalesceParams& params);
    bool SetRingParams(const NetworkRingParams& params);
    bool GetRingParams(NetworkRingParams& params);
    bool SetChannelParams(const NetworkChannelParams& params);
    bool GetChannelParams(NetworkChannelParams& params);
    bool SetFlowCtrl(FlowControlMode mode);
    bool GetFlowCtrl(FlowControlMode& mode);
    bool EnableNapiMode();
    bool DisableNapiMode();
    bool SetNapiWt(uint32 weight);
    uint32 GetNapiWt();
    bool EnableChksumOffload(ChecksumOffloadType type);
    bool DisableChksumOffload(ChecksumOffloadType type);
    bool GetChksumOffloadStatus(ChecksumOffloadType type, bool& enabled);
    bool EnableTcpSegOff();
    bool DisableTcpSegOff();
    bool GetTcpSegOffStatus(bool& enabled);
    bool EnableUdpFragOff();
    bool DisableUdpFragOff();
    bool GetUdpFragOffStatus(bool& enabled);
    bool EnableGenericRcvOff();
    bool DisableGenericRcvOff();
    bool GetGenericRcvOffStatus(bool& enabled);
    bool EnableLargeRcvOff();
    bool DisableLargeRcvOff();
    bool GetLargeRcvOffStatus(bool& enabled);
    bool SetIntrModeration(uint32 usec);
    uint32 GetIntrModeration();
    bool SetRxBufParams(const NetworkRxBufferParams& params);
    bool GetRxBufParams(NetworkRxBufferParams& params);
    bool SetTxBufParams(const NetworkTxBufferParams& params);
    bool GetTxBufParams(NetworkTxBufferParams& params);
    bool EnableHwTimestmp(HardwareTimestampingMode mode);
    bool DisableHwTimestmp();
    bool GetHwTimestmpStatus(HardwareTimestampingStatus& status);
    bool SetJumboFrms(bool enable, uint32 max_frame_size);
    bool GetJumboFrmsStatus(bool& enabled, uint32& max_frame_size);
    bool EnableVxlanOff();
    bool DisableVxlanOff();
    bool GetVxlanOffStatus(bool& enabled);
    bool EnableGeneveOff();
    bool DisableGeneveOff();
    bool GetGeneveOffStatus(bool& enabled);
    bool EnableTunnelOff(TunnelProtocol protocol);
    bool DisableTunnelOff(TunnelProtocol protocol);
    bool GetTunnelOffStatus(TunnelProtocol protocol, bool& enabled);
    bool EnableRcvSideComb(RscMode mode);
    bool DisableRcvSideComb();
    bool GetRcvSideCombStatus(RscMode& mode);
    bool EnableNtupleFltr();
    bool DisableNtupleFltr();
    bool GetNtupleFltrStatus(bool& enabled);
    bool AddNtupleFltrRule(const NtupleFilterRule& rule);
    bool RemoveNtupleFltrRule(uint32 filter_id);
    bool GetNtupleFltrRule(uint32 filter_id, NtupleFilterRule& rule);
    bool EnableArpOff();
    bool DisableArpOff();
    bool GetArpOffStatus(bool& enabled);
    bool EnableNsOff();
    bool DisableNsOff();
    bool GetNsOffStatus(bool& enabled);
    bool EnableTcpSegOffload();
    bool DisableTcpSegOffload();
    bool GetTcpSegOffloadStatus(bool& enabled);
    bool EnableUdpTnlOffload();
    bool DisableUdpTnlOffload();
    bool GetUdpTnlOffloadStatus(bool& enabled);
    bool EnableIpsecOffload();
    bool DisableIpsecOffload();
    bool GetIpsecOffloadStatus(bool& enabled);
    bool EnableSctpOffload();
    bool DisableSctpOffload();
    bool GetSctpOffloadStatus(bool& enabled);

private:
    // Internal helper functions
    bool InitializePciDevice();
    bool InitializeMmioDevice();
    bool SetupRing(uint32 queue_index, uint16_t queue_size);
    bool CleanupRing(uint32 queue_index);
    bool AddBufferToQueue(uint32 queue_index, void* buffer, uint32 size, bool write);
    bool ProcessUsedBuffers(uint32 queue_index);
    bool HandleConfigChange();
    bool SendNetworkPacket(const void* packet, uint32 size);
    bool ReceiveNetworkPacket(void* packet, uint32 max_size);
    bool UpdateNetworkStats();
    bool ResetNetworkStats();
    bool FlushNetworkBuffers();
    bool GetNetworkBufferSizes(uint32& rx_size, uint32& tx_size);
    bool SetNetworkMTU(uint32 new_mtu);
    bool GetNetworkLinkStatus(NetworkLinkStatus& status);
    bool SetNetworkLinkParameters(uint32 speed, bool full_duplex);
    bool EnableNetworkWakeOnLan(WakeOnLanMode mode);
    bool DisableNetworkWakeOnLan();
    bool GetNetworkWakeOnLanStatus(WakeOnLanStatus& status);
    bool SetNetworkVlanFilter(uint16_t vlan_id, bool enable);
    bool GetNetworkVlanFilter(uint16_t vlan_id, bool& enabled);
    bool EnableNetworkVlanFiltering();
    bool DisableNetworkVlanFiltering();
    bool SetNetworkCoalesceParameters(const NetworkCoalesceParams& params);
    bool GetNetworkCoalesceParameters(NetworkCoalesceParams& params);
    bool SetNetworkRingParameters(const NetworkRingParams& params);
    bool GetNetworkRingParameters(NetworkRingParams& params);
    bool SetNetworkChannelParameters(const NetworkChannelParams& params);
    bool GetNetworkChannelParameters(NetworkChannelParams& params);
    bool PauseNetworkTx();
    bool ResumeNetworkTx();
    bool PauseNetworkRx();
    bool ResumeNetworkRx();
    bool GetNetworkExtendedStats(NetworkExtendedStats& stats);
    bool SetNetworkFlowControl(FlowControlMode mode);
    bool GetNetworkFlowControl(FlowControlMode& mode);
    bool EnableNetworkNapi();
    bool DisableNetworkNapi();
    bool SetNetworkNapiWeight(uint32 weight);
    uint32 GetNetworkNapiWeight();
    bool EnableNetworkChecksumOffload(ChecksumOffloadType type);
    bool DisableNetworkChecksumOffload(ChecksumOffloadType type);
    bool GetNetworkChecksumOffloadStatus(ChecksumOffloadType type, bool& enabled);
    bool EnableNetworkTSO();
    bool DisableNetworkTSO();
    bool GetNetworkTSOStatus(bool& enabled);
    bool EnableNetworkUFO();
    bool DisableNetworkUFO();
    bool GetNetworkUFOStatus(bool& enabled);
    bool EnableNetworkGRO();
    bool DisableNetworkGRO();
    bool GetNetworkGROStatus(bool& enabled);
    bool EnableNetworkLRO();
    bool DisableNetworkLRO();
    bool GetNetworkLROStatus(bool& enabled);
    bool SetNetworkInterruptModeration(uint32 usec);
    uint32 GetNetworkInterruptModeration();
    bool SetNetworkRxBufferParams(const NetworkRxBufferParams& params);
    bool GetNetworkRxBufferParams(NetworkRxBufferParams& params);
    bool SetNetworkTxBufferParams(const NetworkTxBufferParams& params);
    bool GetNetworkTxBufferParams(NetworkTxBufferParams& params);
    bool EnableNetworkHardwareTimestamping(HardwareTimestampingMode mode);
    bool DisableNetworkHardwareTimestamping();
    bool GetNetworkHardwareTimestampingStatus(HardwareTimestampingStatus& status);
    bool SetNetworkJumboFrames(bool enable, uint32 max_frame_size);
    bool GetNetworkJumboFramesStatus(bool& enabled, uint32& max_frame_size);
    bool EnableNetworkVxlanOffload();
    bool DisableNetworkVxlanOffload();
    bool GetNetworkVxlanOffloadStatus(bool& enabled);
    bool EnableNetworkGeneveOffload();
    bool DisableNetworkGeneveOffload();
    bool GetNetworkGeneveOffloadStatus(bool& enabled);
    bool EnableNetworkTunnelOffload(TunnelProtocol protocol);
    bool DisableNetworkTunnelOffload(TunnelProtocol protocol);
    bool GetNetworkTunnelOffloadStatus(TunnelProtocol protocol, bool& enabled);
    bool EnableNetworkRsc(RscMode mode);
    bool DisableNetworkRsc();
    bool GetNetworkRscStatus(RscMode& mode);
    bool EnableNetworkNtupleFiltering();
    bool DisableNetworkNtupleFiltering();
    bool GetNetworkNtupleFilteringStatus(bool& enabled);
    bool AddNetworkNtupleFilter(const NtupleFilterRule& rule);
    bool RemoveNetworkNtupleFilter(uint32 filter_id);
    bool GetNetworkNtupleFilter(uint32 filter_id, NtupleFilterRule& rule);
    bool EnableNetworkArpOffload();
    bool DisableNetworkArpOffload();
    bool GetNetworkArpOffloadStatus(bool& enabled);
    bool EnableNetworkNsOffload();
    bool DisableNetworkNsOffload();
    bool GetNetworkNsOffloadStatus(bool& enabled);
    bool EnableNetworkTcpSegOffload();
    bool DisableNetworkTcpSegOffload();
    bool GetNetworkTcpSegOffloadStatus(bool& enabled);
    bool EnableNetworkUdpTnlOffload();
    bool DisableNetworkUdpTnlOffload();
    bool GetNetworkUdpTnlOffloadStatus(bool& enabled);
    bool EnableNetworkIpsecOffload();
    bool DisableNetworkIpsecOffload();
    bool GetNetworkIpsecOffloadStatus(bool& enabled);
    bool EnableNetworkSctpOffload();
    bool DisableNetworkSctpOffload();
    bool GetNetworkSctpOffloadStatus(bool& enabled);
};

// Global Virtio network driver instance
extern VirtioNetDriver* g_virtio_net_driver;

// Initialize the Virtio network driver
bool InitializeVirtioNet();

#endif