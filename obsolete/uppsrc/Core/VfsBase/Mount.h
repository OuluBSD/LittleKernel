#ifndef _Core_VfsBase_Mount_h_
#define _Core_VfsBase_Mount_h_

struct MountManager {
	struct MountPoint {
		VfsPath path;
		String type;
		Ptr<VFS> vfs;
		One<VFS> owned_vfs;
	};
	
	typedef MountManager CLASSNAME;
	MountManager();
	bool HasRoot() const;
	bool Mount(String path, VFS* vfs, String type="");
	MountPoint* Find(const String& path);
	MountPoint* Find(const VfsPath& path, VfsPath* rel_path=0);
	bool GetFiles(const VfsPath& path, Vector<VfsItem>& items);
	bool DirectoryExists(const VfsPath& dir);
	bool FileExists(const VfsPath& dir);
	bool Exists(const VfsPath& dir, VfsItemType type);
	
	Array<MountPoint> mounts;
	String last_error;
	
	static MountManager& System() {static MountManager m; return m;}
};


#endif
