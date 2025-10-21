#ifndef _AudioHost_Loader_h_
#define _AudioHost_Loader_h_


NAMESPACE_UPP


class MidiEonContextLoader
{
	int			id_counter = 0;
	String		last_error;
	String		result;
	
	
	bool		LoadFileMidi(String path, Value& dst);
	void		OnError(TypeCls type, String fn, String msg);
	void		OnError(String fn, String msg);
	void		Clear();
	
public:
	typedef MidiEonContextLoader CLASSNAME;
	MidiEonContextLoader();
	
	bool		Load(String path, Value& o);
	String		GetResult() const {return result;}
	
	
	Callback	WhenError;
	
};


struct SerialMidiEonLoader : SerialLoaderBase {
	
	virtual String LoadFile(String file_path) override;
	
};

END_UPP_NAMESPACE

#endif
