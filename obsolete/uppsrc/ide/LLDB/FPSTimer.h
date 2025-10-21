#ifndef _ide_LLDB_FPSTimer_h_
#define _ide_LLDB_FPSTimer_h_

using namespace std::chrono;

class FPSTimer {
	TimeStop ts;
    double m_second_start;
    double m_last_frame_end;
    int m_frames_this_second;
    float m_fps;

public:
    FPSTimer()
        : m_second_start(0),
          m_last_frame_end(0),
          m_frames_this_second(0),
          m_fps(0.f)
    {
    }

    void FrameEnd()
    {
        m_last_frame_end = ts.Seconds();
        double elapsed = m_last_frame_end - m_second_start;

        if (elapsed > 100) {
            m_fps = static_cast<float>(m_frames_this_second);
            m_frames_this_second = 0;
            m_second_start = m_last_frame_end;
        }
        else {
            m_frames_this_second++;
        }
    }

    inline float GetCurrentFPS() { return m_fps; }

    inline void WaitForFrameDuration(int request_us)
    {
        int ms = request_us / 1000000;
        if (ms > 0)
            Sleep(ms);
        else
            Sleep(1);
    }
};

#endif
