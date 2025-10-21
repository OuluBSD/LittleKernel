#ifndef _ide_LLDB_Timer_h_
#define _ide_LLDB_Timer_h_

class Timer final {
    using Clock = std::chrono::high_resolution_clock;
    using Time = Clock::time_point;

    const Time start;

public:
    uint64_t elapsed_ns()
    {
        const Time stop = Clock::now();
        auto d = std::chrono::duration_cast<std::chrono::nanoseconds>(stop - start);
        return d.count();
    }

    Timer() : start(Clock::now()) {}
};


#endif
