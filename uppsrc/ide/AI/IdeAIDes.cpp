#include "AI.h"

NAMESPACE_UPP

IdeAIDes::IdeAIDes()
{
	code << [=] { delay.KillSet(250, [=] { Preview(); }); };
	Add(code.SizePos());
	if(TheIde())
		code.SetFont(((Ide*)TheIde())->editorfont);
}

void IdeAIDes::Preview() { code.Refresh(); }

void IdeAIDes::SaveEditPos()
{
	if(filename.GetCount()) {
		IdeEditPos& p = sEPai().GetAdd(filename);
		p.filetime = FileGetTime(filename);
		
		p.undodata = code.PickUndoData();
		p.editpos = code.GetEditPos();
	}
}

bool IdeAIDes::Load(const String& includes, const String& filename_)
{
	filename = filename_;
	FileIn in(filename);
	if(in) {
		code.Load(includes, filename, in, CHARSET_UTF8);
		IdeEditPos& ep = sEPai().GetAdd(filename);
		if(ep.filetime == FileGetTime(filename)) {
			code.SetEditPos(ep.editpos);
			code.SetPickUndoData(pick(ep.undodata));
		}
		Preview();
		return true;
	}
	return false;
}

void IdeAIDes::Save()
{
	FileOut out(filename);
	code.Save(out, CHARSET_UTF8);
}

void IdeAIDes::EditMenu(Bar& menu)
{
	//	EditTools(menu);
}

void IdeAIDes::GotFocus() { code.SetFocus(); }

void IdeAIDes::Serialize(Stream& s) {}

void SerializeAIDesPos(Stream& s)
{
	VectorMap<String, IdeEditPos>& filedata = sEPai();
	s.Magic();
	s.Magic(0);
	if(s.IsStoring()) {
		for(int i = 0; i < filedata.GetCount(); i++) {
			String fn = filedata.GetKey(i);
			if(!fn.IsEmpty()) {
				FindFile ff(fn);
				IdeEditPos& ep = filedata[i];
				if(ff && ff.GetLastWriteTime() == ep.filetime) {
					s % fn;
					s % ep.filetime;
					s % ep.editpos;
				}
			}
		}
		String end;
		s % end;
	}
	else {
		String fn;
		filedata.Clear();
		for(;;) {
			s % fn;
			if(fn.IsEmpty())
				break;
			IdeEditPos& ep = filedata.GetAdd(fn);
			s % ep.filetime;
			s % ep.editpos;
		}
	}
}

bool IsAIFile(const char* path)
{
	String n = GetFileName(path);
	String e = ToLower(GetFileExt(path));
	return n == "AI.json" || e == ".cpp" || e == ".c" || e == ".h" || e == ".hpp" ||
	       e == ".icpp";
}

struct AIDesModule : public IdeModule {
	virtual String GetID() { return "AIDesModule"; }

	virtual Image FileIcon(const char* path)
	{
		return IsAIFile(path) ? IdeCommonImg::AI() : Null;
	}

	virtual bool AcceptsFile(const char* path) { return GetFileName(path) == "AI.json"; }

	IdeDesigner* CreateSolver(Ide* ide, const char* path, byte charset)
	{
		TaskMgr::Setup(ide);
		if(IsAIFile(path)) {
			IdeAIDes* d = new IdeAIDes;
			LoadFromGlobal(*d, "aides-ctrl");
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
		SerializeAIDesPos(s);
	}
};

INITIALIZER(IdeAIDes)
{
	RegisterIdeModule(Single<AIDesModule>());
	RegisterGlobalConfig("aides-ctrl");
}

END_UPP_NAMESPACE
