#ifndef MONITOR_H
#define MONITOR_H

#include "Common.h"
#include "SerialDriver.h"


struct Monitor {
	uint16 *video_memory;
	uint8 cursor_x, cursor_y;
	uint16 cursorLocation;
	
	int Init();
	
	void MoveCursor();
	void Scroll();
	void Put(char c);		// Write a single character out to the screen.
	void Clear();			// Clear the screen to all black.
	Monitor& Write(const char *c);	// Output a null-terminated ASCII string to the monitor.
	Monitor& WriteDec(int i);
	Monitor& WriteHex(void* p);
	Monitor& WriteHex(uint32 i);
	Monitor& NewLine();
};

#define PANIC(x) GenericWrite("\n --> "); \
	GenericWrite(__FILE__); GenericWrite(":"); \
	GenericWriteDec(__LINE__); GenericWrite(" "); \
	GenericWrite(x); while(1);

#define MON global->monitor

#define KDUMPI(x) GenericWrite(#x ": "); GenericWriteDec(x); GenericWrite("\n");
#define KDUMPH(x) GenericWrite(#x ": "); GenericWriteHex((uint32)x); GenericWrite("\n");

#endif
