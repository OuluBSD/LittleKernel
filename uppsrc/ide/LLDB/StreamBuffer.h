#ifndef _ide_LLDB_StreamBuffer_h_
#define _ide_LLDB_StreamBuffer_h_

class StreamBuffer {
public:
    enum class StreamSource { StdOut, StdErr };

    void Update(lldb::SBProcess process);
    const char* Get() const { return m_data; }
    void Clear();
    size_t GetCount() const { return m_offset; }

    StreamBuffer(StreamSource source);
    ~StreamBuffer();

    StreamBuffer(const StreamBuffer&) = delete;
    StreamBuffer& operator=(const StreamBuffer&) = delete;
    StreamBuffer& operator=(StreamBuffer&&) = delete;

private:
    size_t m_offset;
    size_t m_capacity;
    char* m_data;
    const StreamSource m_source;  // TODO: switch this to template parameter

    static const size_t MAX_CAPACITY = static_cast<size_t>(2e9);
};


#endif
