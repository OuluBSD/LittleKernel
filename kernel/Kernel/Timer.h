#ifndef _Kernel_Timer_h_
#define _Kernel_Timer_h_

// Don't include other headers in this file - only the package header should include other headers

// Forward declaration
struct Spinlock;

class Timer {
private:
    uint32 frequency;      // Timer frequency in Hz
    uint32 tick_count;     // Number of ticks since boot
    Spinlock lock;         // For thread-safe access
    
public:
    Timer();
    void Initialize(uint32 freq = 100);  // Default to 100Hz (10ms intervals)
    void SetFrequency(uint32 freq);
    uint32 GetFrequency() const { return frequency; }
    uint32 GetTickCount() const { return tick_count; }
    void Sleep(uint32 milliseconds);
    void SleepSeconds(uint32 seconds);
    
    // Called by timer interrupt handler
    void Tick();
};

// Global timer functions
extern Timer* global_timer;

#endif