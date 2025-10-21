#ifndef _Core_VfsBase_VFS_h_
#define _Core_VfsBase_VFS_h_

/*
	Internal path notation (default is INTERNAL_COMB):
		INTERNAL_POSIX:		e.g. /usr/local/bin/bash
		INTERNAL_CPM:		e.g. C:\GAMES\ACROSS
		INTERNAL_COMB:		e.g. C:/Apps/Upp
*/
#if defined flagINTERNAL_POSIX
	#define INTERNAL_POSIX 1
	#define INTERNAL_CPM   0
	#define INTERNAL_COMB  0
	#define INTERNAL_ROOT_PATH "/"
	#define INTERNAL_SEPS "/"
	#define INTERNAL_SEP '/'
	#define INTERNAL_ROOT_FILE(x) "/" x
#elif defined flagINTERNAL_CPM
	#define INTERNAL_POSIX 0
	#define INTERNAL_CPM   1
	#define INTERNAL_COMB  0
	#define INTERNAL_ROOT_PATH ""
	#define INTERNAL_SEPS "\\"
	#define INTERNAL_SEP '\\'
	#define INTERNAL_ROOT_FILE(x) x ":"
#else
	#define INTERNAL_POSIX 0
	#define INTERNAL_CPM   0
	#define INTERNAL_COMB  1
	#define INTERNAL_ROOT_PATH ""
	#define INTERNAL_SEPS "/" // the default is "/" and not "\" because it's easier to type for the original maintainer (who uses the Finnish/Swedish keyboard layout)
	#define INTERNAL_SEP '/'
	#define INTERNAL_ROOT_FILE(x) x ":"
#endif

struct Visitor;

struct VfsPath : Moveable<VfsPath> {
	VfsPath();
	//VfsPath(const Value& s);
	VfsPath(const VfsPath& path);
	VfsPath(const Vector<Value>& path);
	VfsPath(VfsPath&& path);
	VfsPath& operator=(const VfsPath& path);
	VfsPath& Add(Value part);
	void	Set(String path);
	void	Set(Value s, const VfsPath& vfs);
	void	Set(const Vector<Value>& parts);
	void	Set(const VfsPath& path, int begin, int end);
	void	SetDotPath(String path);
	void	SetPosixPath(String path);
	void    Remove(int i);
	bool	IsLeft(const VfsPath& path) const;
	bool	IsSame(const VfsPath& path, int this_begin, int other_begin, int len) const;
	String	AsSysPath() const;
	int		GetPartCount() const;
	bool	IsEmpty() const;
	Value	TopPart() const;
	String  GetFilename() const;
	String	ToString() const;
	VfsPath GetCanonical() const;
	VfsPath Left(int i) const;
	bool	Normalize();
	bool	IsValid() const;
	bool	IsValidFullPath() const;
	void	Append(const VfsPath& p);
	const String& Get() const;
	const Vector<Value>& Parts() const;
	operator String() const;
	void    RemoveLast();
	hash_t  GetHashValue() const;
	bool operator==(const VfsPath& p) const;
	bool operator==(const String& p) const;
	void    Visit(Visitor& v);
	bool    IsSysDirectory() const;
	bool    IsSysFile() const;
private:
	String			str;
	Vector<Value>	parts;
	void	StrFromParts();
};

VfsPath ValVfs(Value v);
VfsPath StrVfs(String s);

VfsPath operator+(const char* s, const VfsPath& vfs);
VfsPath operator+(Value s, const VfsPath& vfs);
VfsPath operator+(const VfsPath& vfs, const char* s);
VfsPath operator+(const VfsPath& vfs, Value s);
VfsPath operator/(const VfsPath& a, const VfsPath& b);
bool IsFullInternalDirectory(const String& path);
String AppendInternalFileName(const String& a, const String& b);
String NormalizeInternalPath(const String& path);
Value FindValuePath(Value v, const VfsPath& p);

typedef enum : byte {
	VFS_NULL,
	VFS_DIRECTORY,
	VFS_FILE,
	VFS_SYMLINK
} VfsItemType;

struct VfsItem : Moveable<VfsItem> {
	String name;
	String type_str;
	VfsItemType type = VFS_NULL;
	virtual ~VfsItem();
	void operator=(Value v);
	Shared<VfsItem> operator()(Value key);
	virtual Value Get();
	virtual Value Get(Value key);
	virtual Value GetKey(int i);
	virtual void Set(Value v);
	virtual void MapSet(Value key, Value v);
	virtual void RealizeMap();
	virtual Value At(int i);
	virtual int GetCount() const;
	virtual Shared<VfsItem> Open(Value key);
};

struct VFS : Pte<VFS> {
	VFS() {}
	virtual ~VFS() {}
	virtual bool GetFiles(const VfsPath& rel_path, Vector<VfsItem>& items) = 0;
	virtual VfsItemType CheckItem(const VfsPath& rel_path) = 0;
	
	String last_error;
};

struct SystemFS : VFS {
	SystemFS() {}
	bool GetFiles(const VfsPath& rel_path, Vector<VfsItem>& items) override;
	VfsItemType CheckItem(const VfsPath& rel_path) override;
};

struct ValueFS;

struct ValueVfsItem : VfsItem {
	VfsPath path;
	mutable Value value;
	mutable bool valid_read = false;
	ValueFS* fs = 0;
	Ptr<VFS> fs_smart_ptr = 0; // for checking if 'fs' is usable
	
	Value Get() override;
	Value Get(Value key) override;
	Value GetKey(int i) override;
	void Set(Value v) override;
	void MapSet(Value key, Value v) override;
	void RealizeMap() override;
	Value At(int i) override;
	int GetCount() const override;
	void RealizeValue() const;
	Shared<VfsItem> Open(Value key) override;
};

struct ValueFS : VFS {
	ValueFS();
	ValueFS(Value& v);
	
	Value GetValue(const VfsPath& p);
	bool Set(const VfsPath& p, const Value& val);
	Shared<VfsItem> Get(const VfsPath& p);
	Shared<VfsItem> operator()(String);
	Shared<VfsItem> RealizeMap(const VfsPath& p);
	bool GetFiles(const VfsPath& rel_path, Vector<VfsItem>& items) override;
	VfsItemType CheckItem(const VfsPath& rel_path) override;
	
protected:
	friend struct ValueVfsItem;
	Value* v = 0;
	
	bool Set(Value& dir, int i, const VfsPath& p, const Value& val);
	bool Get(const Value& dir, int i, const VfsPath& p, Value& val);
	void RealizeMap(Value& dir, int i, const VfsPath& p);
};

#endif
