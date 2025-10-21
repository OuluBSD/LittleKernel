#ifndef _Vfs_Overlay_VfsOverlay_h_
#define _Vfs_Overlay_VfsOverlay_h_

struct SourceRef : Moveable<SourceRef> {
    hash_t pkg_hash = 0;
    hash_t file_hash = 0;
    String local_path;   // path or id inside the source fragment
    int    priority = 0; // higher wins
    dword  flags = 0;    // e.g., disabled
};

// Initial conflict policy (documented):
// - Child key = (id, type_hash)
// - Scalars first-wins by precedence; children union by key; extensions TBD

struct OverlayView {
    // Resolve an effective child set for a logical node identified by path
    virtual Vector<String> List(String logical_path) const = 0;
    // Get merged json (for debugging/inspection)
    virtual Value GetMerged(String logical_path) const = 0;
    virtual ~OverlayView() {}
};

#endif

