#include <ide/ide.h>

NAMESPACE_UPP

// These use lightweight queries; extend as needed.
String GetCurrentWorkspaceName() {
    return TheIde() ? TheIde()->GetTitle().ToString() : "unnamed";
}

int GetCurrentWorkspacePackageCount() {
    const auto& wksp = GetIdeWorkspace();
    return wksp.GetCount();
}

END_UPP_NAMESPACE

