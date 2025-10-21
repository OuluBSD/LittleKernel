#ifndef _ide_LLDB_Log_h_
#define _ide_LLDB_Log_h_

// TODO: modify to include file/function/line information?
// also include lldb version/commit number?
//#define LOG(LEV) LogMessageStream(LogLevel::LEV)

enum class LogLevel { Verbose, Debug, Info, Warning, Error };

struct LogMessage : Moveable<LogMessage> {
    const LogLevel level;
    const String message;

    LogMessage(LogLevel level, const String& message) : level(level), message(message) {}
};

class LLDBLogger {
    Vector<LogMessage> messages;
    VectorMap<hash_t, int> hashed_counts;

    static One<LLDBLogger> s_instance;
    static Mutex s_mutex;

public:
    static LLDBLogger* Get()
    {
        Mutex::Lock lock(s_mutex);
        if (!s_instance) {
            s_instance.Create();
        }
        return &*s_instance;
    }

    void Log(LogLevel level, const String& message)
    {
        Mutex::Lock lock(s_mutex);
        const hash_t strhash = message.GetHashValue();
        int i = hashed_counts.Find(strhash);

        if (i >= 0) {
            int& count = hashed_counts[i];
            if (count <= 3) {
                messages.Add(LogMessage(level, message));
                count++;
            }
            else if (count == 4) {
                messages.Add(LogMessage(LogLevel::Info, "Silencing repeating message..."));
                count++;
            }
        }
        else {
            hashed_counts.Add(strhash, 1);
            messages.Add(LogMessage(level, message));
        }
    };

    template <typename MessageHandlerFunc>
    void ForEachMessage(MessageHandlerFunc&& f)
    {
        Mutex::Lock lock(s_mutex);
        for (const LogMessage& message : messages) {
            f(message);
        }
    }

    inline size_t message_count()
    {
        Mutex::Lock lock(s_mutex);
        return messages.GetCount();
    }
};

class LogMessageStream {
    const LogLevel level;

public:
    template <typename T>
    LogMessageStream& operator<<(const T& data)
    {
        UPP::VppLog() << data;
        return *this;
    }

    LogMessageStream(LogLevel level) : level(level) {}
    
};


#endif
