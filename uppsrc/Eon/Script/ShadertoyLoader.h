#ifndef _Eon_Script_ShadertoyLoader_h_
#define _Eon_Script_ShadertoyLoader_h_


class ShadertoyContextLoader
{
	int			id_counter = 0;
	String		last_error;
	
	
	bool		LoadFileToy(String path, Value& dst);
	void		OnError(TypeCls type, String fn, String msg);
	void		OnError(String fn, String msg);
	void		MakeUniqueIds(Value& v);
	void		Clear();
	int			MakeUniqueId(VectorMap<int,int>& ids, int orig_id);
	
public:
	typedef ShadertoyContextLoader CLASSNAME;
	ShadertoyContextLoader();
	
	bool		Load(String path, Value& o);
	
	
	Callback	WhenError;
	
};


struct SerialShadertoyLoader : SerialLoaderBase {
	
	virtual String LoadFile(String file_path) override;
	
};


#endif
