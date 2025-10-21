#ifndef _ide_LLDB_FileSystem_h_
#define _ide_LLDB_FileSystem_h_

class FileHandle {
    hash_t m_hash;

    // TODO: possibly record last file read/access time in separate static map
    // so that file contents can be dynamically reloaded when they change.
    // This will involve inotify on linux and kqueue on macos.

    static VectorMap<hash_t, String> filepath_cache;
    static VectorMap<hash_t, String> filename_cache;
    static VectorMap<hash_t, Vector<String>> contents_cache;
    static Mutex s_mutex;  // all static VectorMap access must be thread-safe

    FileHandle(hash_t h) : m_hash(h) {}

public:
    FileHandle() = delete;

    static Opt<FileHandle> Create(const String& filepath);

    const Vector<String>& GetContents();
    const String& GetFilepath();
    const String& GetFilename();
    hash_t GetHashValue() const {return m_hash;}

    inline friend bool operator==(const FileHandle& a, const FileHandle& b) {
        return a.m_hash == b.m_hash;
    }
    inline friend bool operator<(const FileHandle& a, const FileHandle& b) {
        return a.m_hash < b.m_hash;
    }
};

class OpenFiles {
    Vector<FileHandle> m_files;
    Opt<size_t> m_focus;

    void Close(int tab_index);

public:
    bool Open(const String& filepath);
    void Open(FileHandle handle);

    inline size_t GetCount() const { return m_files.GetCount(); }

    inline Opt<FileHandle> GetFocus()
    {
        if (m_focus) {
            return m_files[*m_focus];
        }
        else {
            return {};
        }
    }

    enum class Action { Nothing, ChangeFocusTo, Close };

    // Beyond simply looping over the open files, the supplied Callable can optionally return an
    // 'Action' to be applied to each specific file, as defined above.
    template <typename Callable>
    void ForEachOpenFile(Callable&& f)
    {
        if (m_files.empty()) return;

        Opt<size_t> tab_idx_to_close = {};
        Opt<size_t> tab_idx_to_focus = {};

        ASSERT(m_focus);
        const size_t focused_tab_index = *m_focus;

        for (size_t i = 0; i < m_files.GetCount(); i++) {
            switch (f(m_files[i], i == focused_tab_index)) {
                case Action::ChangeFocusTo:
                    ASSERT(!tab_idx_to_focus);
                    tab_idx_to_focus = i;
                    break;
                case Action::Close:
                    ASSERT(!tab_idx_to_close);
                    tab_idx_to_close = i;
                    break;
                case Action::Nothing:
                    break;
            }
        }

        if (tab_idx_to_close) this->Close(*tab_idx_to_close);
        if (tab_idx_to_focus) m_focus = tab_idx_to_focus;
    }
};

class FileBrowserNode {
    VfsPath m_filepath;
    String m_filename;
    bool m_opened;

    void OpenChildren();
    Vector<One<FileBrowserNode>> m_children;

    FileBrowserNode() = delete;

    FileBrowserNode(VfsPath validated_path)
        : m_filepath(validated_path.GetCanonical()),
          m_filename(m_filepath.GetFilename()),
          m_opened(false)
    {
    }

public:
    static One<FileBrowserNode> Create(Opt<VfsPath> path_request);

    String GetFilepath() const { return m_filepath; }
    String GetFilename() const { return m_filename; }
    bool IsDirectory() const { return m_filepath.IsSysDirectory(); }

    inline Vector<One<FileBrowserNode>>& GetChildren()
    {
        this->OpenChildren();
        return m_children;
    }
};


#endif
