#ifndef _Kernel_NetworkDriverInterface_h_
#define _Kernel_NetworkDriverInterface_h_

#include "Common.h"
#include "DriverFramework.h"
#include "Defs.h"

// Ethernet constants
#define ETH_HEADER_SIZE 14
#define ETH_ADDRESS_SIZE 6
#define ETH_FRAME_MIN 64
#define ETH_FRAME_MAX 1518
#define ETH_MTU (ETH_FRAME_MAX - ETH_HEADER_SIZE - 4) // -4 for FCS

// Ethernet frame structure
struct EthernetFrame {
    uint8 destination[ETH_ADDRESS_SIZE];
    uint8 source[ETH_ADDRESS_SIZE];
    uint16_t type;  // EtherType
    uint8 data[ETH_MTU];
    uint32 fcs;   // Frame Check Sequence (not actually part of data)
};

// ARP constants
#define ARP_HW_TYPE_ETHERNET 1
#define ARP_PROTO_TYPE_IP 0x0800
#define ARP_OP_REQUEST 1
#define ARP_OP_REPLY 2

// ARP structure
struct ArpPacket {
    uint16_t hw_type;
    uint16_t proto_type;
    uint8 hw_addr_len;
    uint8 proto_addr_len;
    uint16_t op;
    uint8 sender_hw_addr[ETH_ADDRESS_SIZE];
    uint32 sender_proto_addr;  // IP address
    uint8 target_hw_addr[ETH_ADDRESS_SIZE];
    uint32 target_proto_addr;  // IP address
};

// IPv4 constants
#define IP_VERSION 4
#define IP_HEADER_MIN_SIZE 20
#define IP_PROTO_ICMP 1
#define IP_PROTO_TCP 6
#define IP_PROTO_UDP 17

// IPv4 header structure
struct IpHeader {
    uint8 version_ihl;      // Version and Internet Header Length
    uint8 type_of_service;  // Type of Service
    uint16_t total_length;    // Total Length
    uint16_t identification;  // Identification
    uint16_t flags_fragment;  // Flags and Fragment Offset
    uint8 time_to_live;     // Time to Live
    uint8 protocol;         // Protocol
    uint16_t header_checksum; // Header Checksum
    uint32 source_addr;     // Source Address
    uint32 dest_addr;       // Destination Address
} __attribute__((packed));

// Network interface structure
struct NetworkInterface {
    uint8 mac_address[ETH_ADDRESS_SIZE];
    uint32 ip_address;
    uint32 subnet_mask;
    uint32 gateway;
    char name[16];            // Interface name (e.g., "eth0")
    uint32 mtu;             // Maximum Transmission Unit
    bool link_up;             // Whether physical link is up
    bool initialized;         // Whether interface is initialized
    void* driver_private;     // Private data for the specific driver
};

// Network-specific IOCTL commands
enum NetworkIoctlCommands {
    NETWORK_GET_MAC_ADDRESS = 1,
    NETWORK_SET_MAC_ADDRESS,
    NETWORK_GET_IP_ADDRESS,
    NETWORK_SET_IP_ADDRESS,
    NETWORK_GET_SUBNET_MASK,
    NETWORK_SET_SUBNET_MASK,
    NETWORK_GET_GATEWAY,
    NETWORK_SET_GATEWAY,
    NETWORK_GET_STATUS,
    NETWORK_GET_STATS,
    NETWORK_SET_PROMISCUOUS_MODE,
    NETWORK_FLUSH_PACKET_BUFFER
};

// Network statistics structure
struct NetworkStats {
    uint32 packets_sent;
    uint32 packets_received;
    uint32 bytes_sent;
    uint32 bytes_received;
    uint32 errors_sent;
    uint32 errors_received;
    uint32 dropped_packets;
};

// Network packet structure for the driver interface
struct NetworkPacket {
    uint8* data;
    uint32 length;
    uint32 max_length;
    NetworkInterface* interface;
    uint32 timestamp;
};

// Base class for network drivers
class NetworkInterfaceDriver {
protected:
    Device network_device;
    NetworkInterface interface_info;
    RingBuffer<NetworkPacket, 64> rx_buffer;  // Receive buffer
    RingBuffer<NetworkPacket, 64> tx_buffer;  // Transmit buffer
    NetworkStats stats;
    Spinlock buffer_lock;

public:
    NetworkInterfaceDriver(const char* interface_name);
    virtual ~NetworkInterfaceDriver();

    // Initialize the network driver
    virtual bool Initialize() = 0;

    // Network-specific functions
    virtual bool SendPacket(const uint8* data, uint32 length) = 0;
    virtual bool ReceivePacket(uint8* buffer, uint32* length, uint32 max_length);
    virtual bool ProcessReceivedData(const uint8* data, uint32 length);
    virtual void HandleInterrupt();

    // Interface configuration
    bool SetIpAddress(uint32 ip);
    uint32 GetIpAddress();
    bool SetSubnetMask(uint32 mask);
    uint32 GetSubnetMask();
    bool SetGateway(uint32 gateway);
    uint32 GetGateway();
    void GetMacAddress(uint8* mac);
    void SetMacAddress(const uint8* mac);
    bool IsLinkUp();
    uint32 GetMtu();

    // Statistics
    void GetNetworkStats(NetworkStats& stats_out);
    void ResetStats();

    // Packet buffer management
    bool GetReceivedPacket(NetworkPacket& packet);
    void FlushBuffers();

    // Handle network-specific IOCTL commands
    virtual bool HandleIoctl(uint32 command, void* arg);

    // Get the device structure for registration
    Device* GetDevice() { return &network_device; }

protected:
    // Driver framework callbacks
    static bool NetworkInit(Device* device);
    static bool NetworkRead(Device* device, void* buffer, uint32 size, uint32 offset);
    static bool NetworkWrite(Device* device, const void* buffer, uint32 size, uint32 offset);
    static bool NetworkIoctl(Device* device, uint32 command, void* arg);
    static bool NetworkClose(Device* device);

    // Helper functions
    bool IsValidEthernetFrame(const uint8* frame, uint32 length);
    uint16_t CalculateChecksum(const uint8* data, uint32 length);
    uint16_t CalculateIpChecksum(const IpHeader* ip_header);
};

// Base class for specific network hardware drivers
class EthernetDriver : public NetworkInterfaceDriver {
public:
    EthernetDriver(const char* interface_name);
    virtual ~EthernetDriver();

    // Pure virtual functions that hardware-specific drivers must implement
    virtual bool HardwareInitialize() = 0;
    virtual bool SendRawFrame(const uint8* frame, uint32 length) = 0;
    virtual bool ReceiveRawFrame(uint8* frame, uint32* length, uint32 max_length) = 0;

    // Implement base class pure virtual functions
    virtual bool Initialize() override;
    virtual bool SendPacket(const uint8* data, uint32 length) override;
};

#endif