#include "AI.h"
#include <ide/ide.h>
#include <ide/Designers/Designers.h>

NAMESPACE_UPP

template <class T>
Des<T>::Des()
{
	edit.WhenSaveEditPos = [this]{this->SaveEditPos();};
	edit << [=] { delay.KillSet(250, [=] { Preview(); }); };
	Add(edit.SizePos());
	if(TheIde())
		edit.SetFont(((Ide*)TheIde())->editorfont);
}

template <class T>
Des<T>::~Des()
{
	EditPosCached<T>::SaveEditPosCache();
}

template <class T>
void Des<T>::Preview() {
	try {
		edit.Data();
	}
	catch (NoPointerExc e) {
		LOG("Des< " << T::GetID() << ">: error: " << e);
	}
}

template <class T>
bool Des<T>::FileHeaderParser::Parse(Des<T>* des, const String& first_line) {
	bool ready = false;
	
	// Parse file header
	// Determine, if alternative file persistency modes are being used
	if (first_line.GetCount() && first_line[0] == '#') {
		CParser p(first_line);
		try {
			p.PassChar('#');
			if (p.Id("move")) {
				if (p.Id("here")) {
					move_here = true;
					ready = true;
				}
				else if (p.IsId("dir"))
					move_dir = true;
				else
					p.ThrowError("unexpected move qualifier");
			}
			
			if (ready) {
				// pass
			}
			else if (p.Id("dir")) {
				if (p.Char('.') || p.IsEof()) {
					dirpath = des->filename + ".d";
					use_dir = true;
				}
				else if (p.IsString()) {
					String s = p.ReadString();
					if (s.IsEmpty())
						p.ThrowError("empty filename");
					
					// Relative home directory path
					if (s[0] == '~') {
						dirpath = AppendFileName(
								GetHomeDirectory(),
								s.Mid(1));
						use_dir = true;
					}
					
					// Absolute path
					#ifdef flagWIN32
					else if (s.GetCount() >= 2 && s[1] == ':')
					#else
					else if (s[0] == '/')
					#endif
					{
						dirpath = s;
						use_dir = true;
					}
					
					// Path in the same directory
					else {
						dirpath = AppendFileName(GetFileDirectory(des->filename), s);
						use_dir = true;
					}
				}
				else p.ThrowError("unexpected directory value");
			}
			else p.ThrowError("unexpected file header");
		}
		catch (Exc e) {
			Loge("Loading '" + des->filename + "' failed: " + e);
			return false;
		}
	}
	is_parsed = true;
	return true;
}

template <class T>
bool Des<T>::Load(const String& includes, const String& filename_)
{
	filename = filename_;
	
	FileIn in(filename);
	FileHeaderParser p;
	if (in) {
		String first_line = in.GetLine();
		if (!p.Parse(this, first_line))
			return false;
		in.Seek(0);
	}
	
	if (p.use_dir) {
		if (p.move_dir) {
			// use data already in memory: only realize instead of loading
			edit.Realize(includes, filename);
			Preview();
			return true;
		}
		else if (edit.LoadDirectory(includes, filename, p.dirpath, CHARSET_UTF8)) {
			Preview();
			return true;
		}
	}
	else {
		if(in) {
			if (p.move_here) {
				// use data already in memory: only realize instead of loading
				edit.Realize(includes, filename);
			}
			else {
				// TODO check if faster cache can be loaded based on same sha1
				edit.Load(includes, filename, in, CHARSET_UTF8);
			}
			IdeEditPos& ep = sEPai().GetAdd(filename);
			if(ep.filetime == FileGetTime(filename)) {
				//edit.SetEditPos(ep.editpos);
				edit.SetPickUndoData(pick(ep.undodata));
			}
			RestoreEditPos();
			Preview();
			return true;
		}
	}
	return false;
}

template <class T>
void Des<T>::SaveEditPos()
{
	auto& cache = EditPosCached<T>::EditPosCache();
	JsonIO jio;
	edit.EditPos(jio);
	cache.GetAdd(filename) = jio.GetResult();
}

template <class T>
void Des<T>::RestoreEditPos()
{
	auto& cache = EditPosCached<T>::EditPosCache();
	if (cache.IsEmpty())
		EditPosCached<T>::LoadEditPosCache();
	JsonIO jio(cache.GetAdd(filename));
	edit.EditPos(jio);
}

template <class T>
void Des<T>::Save()
{
	FileHeaderParser p;
	FileIn in(filename);
	if (in ){
		String first_line = in.GetLine();
		p.Parse(this, first_line);
	}
	
	if (p.is_parsed && p.use_dir) {
		edit.SaveDirectory(p.dirpath, CHARSET_UTF8);
		
		// Do some housekeeping: change '#move dir' to '#dir'
		if (p.move_dir) {
			String content = LoadFile(filename);
			if (content.Left(9) == "#move dir") {
				content = "#" + content.Mid(6);
				FileOut fout(filename);
				fout << content;
			}
		}
	}
	else {
		// TODO check sha1 if the persistent file is needed to be overwritten
		// TODO check if fast-cached file needs to be overwritten
		StringStream ss;
		edit.Save(ss, CHARSET_UTF8);
		if (ss.IsError()) {
			LOG("error saving file " + filename);
		}
		else {
			ss.Seek(0);
			FileOut out(filename);
			CopyStream(out, ss);
		}
	}
}

template <class T>
void Des<T>::EditMenu(Bar& menu)
{
	//	EditTools(menu);
}

template <class T>
void Des<T>::GotFocus() { edit.SetFocus(); }

template <class T>
void Des<T>::Serialize(Stream& s) {}

template <class T>
String Des<T>::GetIDStatic() {return T::GetID();}

template <class T>
int Des<T>::GetNodeKind() {return T::GetNodeKind();}

template <class T>
bool Des<T>::AcceptsExt(String e) {return T::AcceptsExt(e);}


//template<> struct Des<ScriptReferenceMakerCtrl>;




template <class T>
struct DesModule : public IdeModule {
	static String GetIDStatic() { return T::GetIDStatic() + "-ctrl"; }
	virtual String GetID() { return T::GetIDStatic() + "DesModule"; }

	virtual Image FileIcon(const char* path)
	{
		return T::AcceptsExt(GetFileExt(path)) ? IdeCommonImg::AI() : Null;
	}

	virtual bool AcceptsFile(const char* path) { return T::AcceptsExt(GetFileExt(path)); }

	IdeDesigner* CreateSolver(Ide* ide, const char* path, byte charset)
	{
		TaskMgr::Setup(ide);
		if(T::AcceptsExt(GetFileExt(path))) {
			T* d = new T;
			LoadFromGlobal(*d, GetIDStatic());
			if(d->Load(ide->GetCurrentIncludePath(), path))
				return d;
			delete d;
			return NULL;
		}
		return NULL;
	}
	
	virtual IdeDesigner* CreateSolver(const char* path, byte) { return NULL; }

	virtual void Serialize(Stream& s)
	{
		int version = 0;
		s / version;
	}
};

#define INITIALIZE_MODULE(x) \
	using x##Module = DesModule<x>; \
	INITIALIZER(x) \
	{ \
		RegisterIdeModule(Single<x##Module>()); \
		RegisterGlobalConfig(x##Module::GetIDStatic()); \
	}

INITIALIZE_MODULE(SourceTextDes)
INITIALIZE_MODULE(EnvEditorDes)
INITIALIZE_MODULE(EntityEditorDes)

END_UPP_NAMESPACE
