#include "LLDB.h"


VectorMap<hash_t, String> FileHandle::filepath_cache;
VectorMap<hash_t, String> FileHandle::filename_cache;
VectorMap<hash_t, Vector<String>> FileHandle::contents_cache;
Mutex FileHandle::s_mutex;

Opt<FileHandle> FileHandle::Create(const String& filepath)
{
    VfsPath canonical_path = StrVfs(filepath).GetCanonical();

    // TODO: Log specific reason for any failures
    if (!canonical_path.IsSysFile()) {
        return {};
    }

    if (!canonical_path.GetPartCount()) {
        return {};
    }

    const hash_t path_hash = canonical_path.GetHashValue();

    Mutex::Lock lock(s_mutex);

    {  // cache the canonical absolute filepath for this file
        int i = filepath_cache.Find(path_hash);
        if (i < 0) {
            filepath_cache.Add(path_hash, canonical_path);
        }
    }

    {  // cache the short filename for this file: ex (/some/path/foo.txt -> foo.txt)
        String name = GetFileName(filepath);
        hash_t name_hash = name.GetHashValue();
        int i = filename_cache.Find(name_hash);
        if (i < 0) {
            filename_cache.Add(name_hash, name);
        }
    }

    return FileHandle(path_hash);
}

const Vector<String>& FileHandle::GetContents()
{
    Mutex::Lock lock(s_mutex);

    if (auto i = contents_cache.Find(m_hash); i >= 0) {
        return contents_cache[i];
    }

    const String& filepath = filepath_cache[m_hash];

    FileIn infile(filepath);

    Vector<String> contents;

    String line;
    while (!infile.IsEof()) {
        line = TrimBoth(infile.GetLine());
        contents.Add(line);
    }
    contents.Shrink();

    LOG("Read file from disk: " << filepath);

    const auto& c = contents_cache.Add(m_hash, pick(contents));
    return c;
}

const String& FileHandle::GetFilepath()
{
    Mutex::Lock lock(s_mutex);
    int i = filepath_cache.Find(m_hash);
    ASSERT(i >= 0);
    return filepath_cache[i];
}

const String& FileHandle::GetFilename()
{
    Mutex::Lock lock(s_mutex);
    int i = filename_cache.Find(m_hash);
    ASSERT(i >= 0);
    return filename_cache[i];
}

One<FileBrowserNode> FileBrowserNode::Create(Opt<VfsPath> path_request)
{
    auto fallback = [](bool show_warning) {
        if (show_warning) {
            LOG("warning: Invalid path argument to FileBrowserNode::create, Falling back to current working directory");
        }
        const VfsPath wd = StrVfs(GetCurrentDirectory());
        return One<FileBrowserNode>(new FileBrowserNode(wd));
    };

    if (!path_request.has_value()) {
        return fallback(false);
    }

    const VfsPath relative_path = *path_request;

    if (!relative_path.IsSysFile()) {
        LOG("error: FileBrowser attempted to load non-existent file:" << (String)relative_path);
        return fallback(true);
    }

    VfsPath canonical_path = relative_path.GetCanonical();

    if (!canonical_path.IsSysDirectory() && !canonical_path.IsSysFile()) {
        LOG("error: Attemped to load a path (" << (String)(String)canonical_path << ") that wasn't a directory or regular file!");
        return fallback(true);
    }

    if (!canonical_path.GetPartCount()) {
        LOG("error: No filename for file: " << (String)(String)canonical_path);
        return fallback(true);
    }

    return One<FileBrowserNode>(new FileBrowserNode(canonical_path));
}

void FileBrowserNode::OpenChildren()
{
    if (m_filepath.IsSysDirectory() && !m_opened) {
        for(int i = 0; i < m_filepath.GetPartCount(); i++) {
            VfsPath p = m_filepath.Left(i);
            One<FileBrowserNode> new_child_node = FileBrowserNode::Create(p);
            if (new_child_node) {
                m_children.Add(std::move(new_child_node));
            }
            else {
                LOG("warning: Encountered invalid object while traversing directory: " << (String)m_filepath);
            }
        }

        std::sort(m_children.begin(), m_children.end(),
          [](const One<FileBrowserNode>& a,
             const One<FileBrowserNode>& b) {
			if (a->IsDirectory() && !b->IsDirectory()) {
				return true;
			}
			else if (!a->IsDirectory() && b->IsDirectory()) {
				return false;
			}
			else {
				return a->GetFilename().Compare(b->GetFilename()) < 0;
			}
		});

        m_opened = true;
    }
}

bool OpenFiles::Open(const String& requested_filepath)
{
    const auto handle_attempt = FileHandle::Create(requested_filepath);

    if (handle_attempt) {
        Open(*handle_attempt);
        return true;
    }
    else {
        return false;
    }
}

void OpenFiles::Open(FileHandle handle)
{
    auto it = std::find(m_files.begin(), m_files.end(), handle);
    if (it != m_files.end()) {
        m_focus = it - m_files.begin();
        LOG("Successfully switched focus to previously-opened file: " << handle.GetFilepath());
        return;
    }

    m_files.push_back(handle);
    m_focus = m_files.GetCount() - 1;

    LOG("Successfully opened new file: " << handle.GetFilepath());
    LOG("Number of currently open files: " << m_files.GetCount());
}

void OpenFiles::Close(int tab_index)
{
    m_files.Remove(tab_index);

    if (m_files.empty()) {
        m_focus = {};
    }
    else {
        ASSERT(m_focus);
        const size_t old_open_file_count = m_files.GetCount();
        const size_t old_focused_tab_idx = *m_focus;

        const bool closed_tab_to_left_of_focus = tab_index < old_focused_tab_idx;
        const bool closed_last_tab =
            tab_index == old_focused_tab_idx && old_focused_tab_idx == old_open_file_count - 1;

        if (closed_tab_to_left_of_focus || closed_last_tab) {
            m_focus = old_focused_tab_idx - 1;
        }
    }
}

