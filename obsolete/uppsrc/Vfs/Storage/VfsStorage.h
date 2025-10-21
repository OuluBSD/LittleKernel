// Vfs/Storage header (API scaffold)
#ifndef _Vfs_Storage_VfsStorage_h_
#define _Vfs_Storage_VfsStorage_h_

#include <Core/Core.h>
#include <Vfs/Core/VfsCore.h>

NAMESPACE_UPP

struct VfsValue;

// Save/load per-file fragments; overlay index handled separately.
// Planned JSON schema (draft):
// {
//   "version": 1,
//   "pkg_hash": uint64,
//   "file_hash": uint64,
//   "root": Node
// }
// Node -> {
//   "id": string,
//   "type_hash": uint64,
//   "value": {...},        // Upp::Value or Ast payload; serialized via helpers
//   "ext": {...},          // VfsValueExt payload keyed by ext type_hash
//   "flags": uint32,
//   "children": [Node]
// }
// Value payload encodes whether it's Upp::Value, AstValue, or extension-defined.

bool VfsSaveFragment(const String& path, const VfsValue& fragment);
bool VfsLoadFragment(const String& path, VfsValue& out_fragment);

// Back-compat loader for legacy IDE dumps (placeholder).
bool VfsLoadLegacy(const String& path, VfsValue& out_fragment);

END_UPP_NAMESPACE

#endif
