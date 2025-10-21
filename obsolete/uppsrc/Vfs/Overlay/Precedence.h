// Overlay precedence provider (API scaffold)
#ifndef _Vfs_Overlay_Precedence_h_
#define _Vfs_Overlay_Precedence_h_

// Return ordered package names or hashes, highest precedence first.
struct PackagePrecedenceProvider {
    virtual Vector<hash_t> GetPackageOrder() const = 0;
    virtual ~PackagePrecedenceProvider() {}
};

#endif

