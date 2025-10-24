#include "Kernel.h"
#include "NetworkDriverInterface.h"
#include "Logging.h"

NetworkDriver::NetworkDriver(const char* interface_name) {
    // Initialize the device structure
    network_device.id = 0;  // Will be assigned by framework
    strcpy_safe(network_device.name, interface_name, sizeof(network_device.name));
    network_device.type = DEVICE_TYPE_NETWORK;
    network_device.private_data = this;  // Point to this object for device callbacks
    network_device.flags = 0;
    network_device.base_port = 0;  // Will be set by specific hardware driver
    network_device.irq_line = 0;   // Will be set by specific hardware driver
    network_device.mmio_base = nullptr;  // Will be set by specific hardware driver
    network_device.next = nullptr;
    
    // Set up driver operations
    static DriverOperations ops = {
        NetworkInit,
        NetworkRead,   // Reading would return network packets
        NetworkWrite,  // Writing would send network packets
        NetworkIoctl,
        NetworkClose
    };
    network_device.ops = &ops;
    
    // Initialize interface information
    memset(&interface_info, 0, sizeof(interface_info));
    strcpy_safe(interface_info.name, interface_name, sizeof(interface_info.name));
    interface_info.mtu = ETH_MTU;
    interface_info.link_up = false;
    interface_info.initialized = false;
    interface_info.driver_private = nullptr;
    
    // Initialize statistics
    memset(&stats, 0, sizeof(stats));
    
    // Initialize spinlock
    buffer_lock.Initialize();
}

NetworkDriver::~NetworkDriver() {
    // Clean up any allocated resources
    FlushBuffers();
}

bool NetworkDriver::ReceivePacket(uint8_t* buffer, uint32_t* length, uint32_t max_length) {
    buffer_lock.Acquire();
    
    if (!rx_buffer.IsEmpty()) {
        NetworkPacket packet = rx_buffer.Pop();
        uint32_t copy_len = (packet.length < max_length) ? packet.length : max_length;
        
        memcpy(buffer, packet.data, copy_len);
        *length = copy_len;
        
        // Update stats
        stats.packets_received++;
        stats.bytes_received += copy_len;
        
        buffer_lock.Release();
        return true;
    }
    
    buffer_lock.Release();
    return false;
}

bool NetworkDriver::ProcessReceivedData(const uint8_t* data, uint32_t length) {
    if (!data || length == 0 || length > ETH_FRAME_MAX) {
        return false;
    }
    
    // Validate Ethernet frame
    if (!IsValidEthernetFrame(data, length)) {
        stats.errors_received++;
        return false;
    }
    
    // Add packet to receive buffer
    buffer_lock.Acquire();
    
    if (!rx_buffer.IsFull()) {
        NetworkPacket packet;
        packet.length = length;
        packet.max_length = ETH_FRAME_MAX;
        packet.interface = &interface_info;
        packet.timestamp = global_timer ? global_timer->GetTickCount() : 0;
        // Note: In a real implementation, we would need to allocate memory for packet.data
        // For now, we'll just record the length and let the receiver know how much was received
        
        // Add to buffer
        rx_buffer.Push(packet);
        
        // Update stats
        stats.packets_received++;
        stats.bytes_received += length;
        
        buffer_lock.Release();
        return true;
    }
    
    // Buffer full
    stats.dropped_packets++;
    buffer_lock.Release();
    return false;
}

void NetworkDriver::HandleInterrupt() {
    // Handle network interrupts - to be implemented by derived classes
    // In a real implementation, this would read received packets from hardware
    // and call ProcessReceivedData for each packet
}

bool NetworkDriver::SetIpAddress(uint32_t ip) {
    interface_info.ip_address = ip;
    return true;
}

uint32_t NetworkDriver::GetIpAddress() {
    return interface_info.ip_address;
}

bool NetworkDriver::SetSubnetMask(uint32_t mask) {
    interface_info.subnet_mask = mask;
    return true;
}

uint32_t NetworkDriver::GetSubnetMask() {
    return interface_info.subnet_mask;
}

bool NetworkDriver::SetGateway(uint32_t gateway) {
    interface_info.gateway = gateway;
    return true;
}

uint32_t NetworkDriver::GetGateway() {
    return interface_info.gateway;
}

void NetworkDriver::GetMacAddress(uint8_t* mac) {
    if (mac) {
        memcpy(mac, interface_info.mac_address, ETH_ADDRESS_SIZE);
    }
}

void NetworkDriver::SetMacAddress(const uint8_t* mac) {
    if (mac) {
        memcpy(interface_info.mac_address, mac, ETH_ADDRESS_SIZE);
    }
}

bool NetworkDriver::IsLinkUp() {
    return interface_info.link_up;
}

uint32_t NetworkDriver::GetMtu() {
    return interface_info.mtu;
}

void NetworkDriver::GetNetworkStats(NetworkStats& stats_out) {
    buffer_lock.Acquire();
    stats_out = stats;
    buffer_lock.Release();
}

void NetworkDriver::ResetStats() {
    buffer_lock.Acquire();
    memset(&stats, 0, sizeof(stats));
    buffer_lock.Release();
}

bool NetworkDriver::GetReceivedPacket(NetworkPacket& packet) {
    buffer_lock.Acquire();
    if (!rx_buffer.IsEmpty()) {
        packet = rx_buffer.Pop();
        buffer_lock.Release();
        return true;
    }
    buffer_lock.Release();
    return false;
}

void NetworkDriver::FlushBuffers() {
    buffer_lock.Acquire();
    rx_buffer.Clear();
    tx_buffer.Clear();
    buffer_lock.Release();
}

bool NetworkDriver::HandleIoctl(uint32 command, void* arg) {
    switch (command) {
        case NETWORK_GET_MAC_ADDRESS: {
            uint8_t* mac = (uint8_t*)arg;
            if (mac) {
                GetMacAddress(mac);
            }
            break;
        }
        
        case NETWORK_SET_MAC_ADDRESS: {
            uint8_t* mac = (uint8_t*)arg;
            if (mac) {
                SetMacAddress(mac);
                return true;
            }
            return false;
        }
        
        case NETWORK_GET_IP_ADDRESS: {
            uint32_t* ip = (uint32_t*)arg;
            if (ip) {
                *ip = GetIpAddress();
            }
            break;
        }
        
        case NETWORK_SET_IP_ADDRESS: {
            uint32_t* ip = (uint32_t*)arg;
            if (ip) {
                return SetIpAddress(*ip);
            }
            return false;
        }
        
        case NETWORK_GET_SUBNET_MASK: {
            uint32_t* mask = (uint32_t*)arg;
            if (mask) {
                *mask = GetSubnetMask();
            }
            break;
        }
        
        case NETWORK_SET_SUBNET_MASK: {
            uint32_t* mask = (uint32_t*)arg;
            if (mask) {
                return SetSubnetMask(*mask);
            }
            return false;
        }
        
        case NETWORK_GET_GATEWAY: {
            uint32_t* gateway = (uint32_t*)arg;
            if (gateway) {
                *gateway = GetGateway();
            }
            break;
        }
        
        case NETWORK_SET_GATEWAY: {
            uint32_t* gateway = (uint32_t*)arg;
            if (gateway) {
                return SetGateway(*gateway);
            }
            return false;
        }
        
        case NETWORK_GET_STATUS: {
            bool* status = (bool*)arg;
            if (status) {
                *status = IsLinkUp();
            }
            break;
        }
        
        case NETWORK_GET_STATS: {
            NetworkStats* stats_out = (NetworkStats*)arg;
            if (stats_out) {
                GetNetworkStats(*stats_out);
            }
            break;
        }
        
        case NETWORK_SET_PROMISCUOUS_MODE: {
            // To be implemented by derived class
            break;
        }
        
        case NETWORK_FLUSH_PACKET_BUFFER: {
            FlushBuffers();
            break;
        }
        
        default:
            return false;
    }
    
    return true;
}

bool NetworkDriver::IsValidEthernetFrame(const uint8_t* frame, uint32_t length) {
    if (!frame || length < ETH_FRAME_MIN || length > ETH_FRAME_MAX) {
        return false;
    }
    
    // Basic validation:
    // 1. Frame size is valid
    // 2. EtherType field is at least 0x0600 (IPv4) or 0x0806 (ARP)
    if (length >= ETH_HEADER_SIZE) {
        uint16_t ethertype = (frame[12] << 8) | frame[13];
        if (ethertype < 0x0600 && ethertype != 0x0806) {
            // Not a valid EtherType
            return false;
        }
    }
    
    return true;
}

uint16_t NetworkDriver::CalculateChecksum(const uint8_t* data, uint32_t length) {
    uint32_t sum = 0;
    
    for (uint32_t i = 0; i < length - 1; i += 2) {
        sum += (data[i] << 8) | data[i + 1];
    }
    
    if (length & 1) {
        sum += data[length - 1] << 8;
    }
    
    while (sum >> 16) {
        sum = (sum & 0xFFFF) + (sum >> 16);
    }
    
    return ~sum;
}

uint16_t NetworkDriver::CalculateIpChecksum(const IpHeader* ip_header) {
    // Save original checksum
    uint16_t original_checksum = ip_header->header_checksum;
    
    // Set checksum to 0
    const_cast<IpHeader*>(ip_header)->header_checksum = 0;
    
    // Calculate checksum
    uint16_t checksum = CalculateChecksum((const uint8_t*)ip_header, 
                                         (ip_header->version_ihl & 0x0F) * 4);
    
    // Restore original checksum
    const_cast<IpHeader*>(ip_header)->header_checksum = original_checksum;
    
    return checksum;
}

// Driver framework callbacks
bool NetworkDriver::NetworkInit(Device* device) {
    if (!device) {
        return false;
    }
    
    // Get the network driver instance from private_data
    NetworkDriver* driver = (NetworkDriver*)device->private_data;
    if (!driver) {
        return false;
    }
    
    bool result = driver->Initialize();
    if (result) {
        device->flags |= DRIVER_INITIALIZED;
        driver->interface_info.initialized = true;
        DLOG("Network device initialized");
    } else {
        device->flags |= DRIVER_ERROR;
    }
    
    return result;
}

bool NetworkDriver::NetworkRead(Device* device, void* buffer, uint32 size, uint32 offset) {
    if (!device || !buffer || size == 0) {
        return false;
    }
    
    NetworkDriver* driver = (NetworkDriver*)device->private_data;
    if (!driver) {
        return false;
    }
    
    // This function reads network packets
    // Size should be at least large enough for a packet
    if (size < ETH_FRAME_MIN) {
        return false;
    }
    
    uint32 length;
    bool result = driver->ReceivePacket((uint8_t*)buffer, &length, size);
    
    // Update the size parameter to reflect actual data read
    // This is tricky because the NetworkRead function signature doesn't allow us to modify size
    // In a real implementation, the calling code would need to handle this differently
    
    return result;
}

bool NetworkDriver::NetworkWrite(Device* device, const void* buffer, uint32 size, uint32 offset) {
    if (!device || !buffer || size == 0 || size > ETH_FRAME_MAX) {
        return false;
    }
    
    NetworkDriver* driver = (NetworkDriver*)device->private_data;
    if (!driver) {
        return false;
    }
    
    // Send the network packet
    bool result = driver->SendPacket((const uint8_t*)buffer, size);
    
    if (result) {
        driver->stats.packets_sent++;
        driver->stats.bytes_sent += size;
    } else {
        driver->stats.errors_sent++;
    }
    
    return result;
}

bool NetworkDriver::NetworkIoctl(Device* device, uint32 command, void* arg) {
    if (!device) {
        return false;
    }
    
    NetworkDriver* driver = (NetworkDriver*)device->private_data;
    if (!driver) {
        return false;
    }
    
    return driver->HandleIoctl(command, arg);
}

bool NetworkDriver::NetworkClose(Device* device) {
    if (!device) {
        return false;
    }
    
    NetworkDriver* driver = (NetworkDriver*)device->private_data;
    if (!driver) {
        return false;
    }
    
    // Flush buffers and mark as inactive
    driver->FlushBuffers();
    device->flags &= ~DRIVER_ACTIVE;
    driver->interface_info.initialized = false;
    
    return true;
}

// EthernetDriver implementation

EthernetDriver::EthernetDriver(const char* interface_name) 
    : NetworkDriver(interface_name) {
    // Initialize the device type specifically for Ethernet
    network_device.type = DEVICE_TYPE_NETWORK;
}

EthernetDriver::~EthernetDriver() {
    // Ethernet-specific cleanup would go here
}

bool EthernetDriver::Initialize() {
    // First do hardware initialization
    if (!HardwareInitialize()) {
        LOG("Hardware initialization failed for Ethernet driver");
        return false;
    }
    
    // Then do network initialization
    // Set link status to true after hardware init
    interface_info.link_up = true;
    
    LOG("Ethernet driver initialized successfully");
    return true;
}

bool EthernetDriver::SendPacket(const uint8_t* data, uint32_t length) {
    if (!data || length == 0 || length > interface_info.mtu) {
        return false;
    }
    
    // In a real implementation, this would create an Ethernet frame with proper header
    // and send it using SendRawFrame
    
    // For now, just call the hardware-specific send function directly
    return SendRawFrame(data, length);
}