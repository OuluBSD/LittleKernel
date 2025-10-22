#include "Kernel.h"

Timer::Timer() {
    frequency = 0;
    tick_count = 0;
    lock.Initialize();
}

void Timer::Initialize(uint32 freq) {
    frequency = freq;
    tick_count = 0;
    
    // Calculate divisor for the desired frequency
    uint32 divisor = 1193180 / frequency;
    
    // Send command to PIT (Programmable Interval Timer)
    outportb(0x43, 0x36);  // Command port: channel 0, low/high byte, rate generator
    
    // Send divisor bytes
    uint8 low = (uint8)(divisor & 0xFF);
    uint8 high = (uint8)((divisor >> 8) & 0xFF);
    
    outportb(0x40, low);   // Channel 0 data port (low byte)
    outportb(0x40, high);  // Channel 0 data port (high byte)
    
    DLOG("Timer initialized with frequency: " << frequency << " Hz");
}

void Timer::SetFrequency(uint32 freq) {
    lock.Acquire();
    frequency = freq;
    Initialize(freq);  // Reinitialize with new frequency
    lock.Release();
}

void Timer::Tick() {
    lock.Acquire();
    tick_count++;
    lock.Release();
}

void Timer::Sleep(uint32 milliseconds) {
    uint32 target_ticks = tick_count + (milliseconds * frequency) / 1000;
    
    while (tick_count < target_ticks) {
        // In a real implementation, this would yield to other processes
        // For now, use a busy wait
    }
}

void Timer::SleepSeconds(uint32 seconds) {
    uint32 target_ticks = tick_count + seconds * frequency;
    
    while (tick_count < target_ticks) {
        // In a real implementation, this would yield to other processes
        // For now, use a busy wait
    }
}

// Global timer instance
Timer* global_timer = nullptr;