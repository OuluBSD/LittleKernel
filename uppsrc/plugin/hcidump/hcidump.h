#ifndef _ports_hcidump_hcidump_h_
#define _ports_hcidump_hcidump_h_

#include <Core/Core.h>
#include "internal.h"
/*extern "C" {
	struct hcidump_process_data;
}*/

NAMESPACE_UPP

class SimpleBluetoothConnection {
	//struct hcidump_process_data* data = 0;
	struct hcidump_process_data data;
	int device_idx;
	int mode;
	int sock;
	unsigned long flags = 0;
	
	bool is_open = false;
	
public:
	SimpleBluetoothConnection();
	~SimpleBluetoothConnection();
	
	
	bool Open(int device_idx);
	void Close();
	bool IsOpen() const {return is_open;}
	
	bool ReadPacket(Vector<byte>& data);
	
	
};

END_UPP_NAMESPACE


#endif
