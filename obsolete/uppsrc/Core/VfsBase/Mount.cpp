#include "VfsBase.h"

NAMESPACE_UPP



MountManager::MountManager() {
	
}

bool MountManager::HasRoot() const {
	for (const auto& mm : mounts)
		if (mm.path.IsEmpty())
			return true;
	return false;
}

MountManager::MountPoint* MountManager::Find(const String& path) {
	for (auto& mm : mounts)
		if (mm.path == path)
			return &mm;
	return 0;
}

MountManager::MountPoint* MountManager::Find(const VfsPath& path, VfsPath* rel_path) {
	for (auto& mm : mounts) {
		if (path.IsLeft(mm.path)) {
			if (rel_path) {
				rel_path->Set(path, mm.path.GetPartCount(), path.GetPartCount());
			}
			return &mm;
		}
	}
	return 0;
}

bool MountManager::GetFiles(const VfsPath& path, Vector<VfsItem>& items) {
	items.Clear();
	for (auto& mm : mounts) {
		if (mm.path.GetPartCount()-1 == path.GetPartCount() &&
			mm.path.IsSame(path, 0, 0, path.GetPartCount())) {
			VfsItem& item = items.Add();
			item.name = mm.path.TopPart();
			item.type_str = mm.type;
			#if INTERNAL_CPM || INTERNAL_COMB
			if (mm.path.GetPartCount() == 1) {
				item.name += ":";
			}
			#endif
		}
	}
	if (!path.IsEmpty()) {
		VfsPath rel_path;
		MountPoint* mp = Find(path, &rel_path);
		if (!mp) {
			last_error = "Path is not contained in any mount: " + path;
			return false;
		}
		if (!mp->vfs) {
			last_error = "Null vfs in mountpoint";
			return false;
		}
		return mp->vfs->GetFiles(rel_path, items);
	}
	return true;
}

bool MountManager::Mount(String path, VFS* vfs, String type) {
	if (Find(path)) {
		last_error = "Path already mounted '" + path + "'";
		return false;
	}
	
	MountPoint& mp = mounts.Add();
	mp.path.Set(path);
	mp.type = type;
	mp.vfs = vfs;
	
	return true;
}

bool MountManager::DirectoryExists(const VfsPath& dir) {
	return Exists(dir, VFS_DIRECTORY);
}

bool MountManager::FileExists(const VfsPath& dir) {
	return Exists(dir, VFS_FILE);
}

bool MountManager::Exists(const VfsPath& dir, VfsItemType type) {
	if (dir.GetPartCount() == 0)
		return true;
	VfsPath rel_path;
	MountManager::MountPoint* mp = Find(dir, &rel_path);
	if (!mp || !mp->vfs)
		return false;
	VfsItemType path_type = mp->vfs->CheckItem(rel_path);
	return path_type == type;
}

	
END_UPP_NAMESPACE
