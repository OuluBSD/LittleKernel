// IDE default precedence provider
#ifndef _Ide_Vfs_EnvPrecedence_h_
#define _Ide_Vfs_EnvPrecedence_h_

#include <Core/Core.h>
#include <Vfs/Overlay/Precedence.h>

NAMESPACE_UPP

// Placeholder: pulls package order from current IDE workspace/project.
// For now returns empty to signal fallback to lexical order.
struct IdePackagePrecedence : PackagePrecedenceProvider {
    Vector<hash_t> GetPackageOrder() const override { return Vector<hash_t>(); }
};

END_UPP_NAMESPACE

#endif

