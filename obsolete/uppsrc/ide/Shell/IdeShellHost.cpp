#include "Shell.h"
#include <Intranet/Intranet.h>

NAMESPACE_UPP

IdeShellHostBase::IdeShellHostBase() {
	
}

IdeShellHostBase::~IdeShellHostBase() {
	
}




IdeShellHost::IdeShellHost() {
	AddProgram("cwd",	THISBACK(CurrentWorkingDirectory));
	AddProgram("ls",	THISBACK(ListFiles));
	AddProgram("cd",	THISBACK(ChangeDirectory));
	
	#ifdef flagNET
	AddProgram("intra",	THISBACK(StartIntranet));
	#endif
	
	#ifndef flagV1
	ShellReg_MetaEnv(*this);
	#endif
}

const String& IdeShellHost::GetOutput() const {
	return out;
}

const String& IdeShellHost::GetError() const {
	return err;
}

void IdeShellHost::Put(const String& s) {
	out << s;
}

void IdeShellHost::PutLine(const String& s) {
	out << s << "\n";
}

void IdeShellHost::AddProgram(String cmd, Event<IdeShell&,Value> cb) {
	commands.Add(cmd, cb);
}

bool IdeShellHost::Command(IdeShell& shell, Value cmd) {
	ValueArray args;
	if (cmd.Is<ValueArray>()) args = cmd;
	else args.Add(cmd);
	if (args.IsEmpty())
		return false;
	String main_cmd = args[0].ToString();
	args.Remove(0);
	
	LOG("IdeShellHost::Command: " << args.ToString());
	
	String ret;
	
	out.Clear();
	err.Clear();
	
	for(int i = 0; i < commands.GetCount(); i++) {
		const String& c = commands.GetKey(i);
		if (main_cmd == c) {
			commands[i](shell, args);
			
			// Leave out last \n
			if (!out.IsEmpty() && *(out.End()-1) == '\n')
				out = out.Left(out.GetCount()-1);
			
			return true;
		}
	}
	
	return false;
}

String GetFirstValue(Value arg) {
	if (arg.Is<ValueArray>()) {
		ValueArray va = arg;
		if (va.GetCount() >= 1)
			return va[0];
		else
			return String();
	}
	return arg;
}

void IdeShellHost::CurrentWorkingDirectory(IdeShell& shell, Value arg) {
	MountManager& mm = MountManager::System();
	out = shell.cwd;
	out.Cat('\n');
}

void IdeShellHost::ListFiles(IdeShell& shell, Value arg) {
	MountManager& mm = MountManager::System();
	String path_str = GetFirstValue(arg);
	if (path_str.IsEmpty())
		path_str = shell.cwd;
	else if (!IsFullInternalDirectory(path_str))
		AppendInternalFileName(shell.cwd, path_str);
	VfsPath path(StrVfs(path_str));
	Vector<VfsItem> items;
	mm.GetFiles(path, items);
	const int cols = 4;
	const int col_width = 20;
	int col = 0;
	for (const VfsItem& i : items) {
		String n = i.name;
		if (n.GetCount() > col_width-4)
			n = n.Left(col_width-4) + "...";
		out << n;
		if (col < cols-1) {
			int spaces = col_width - n.GetCount();
			out.Cat(' ', spaces);
			col++;
		}
		else {
			col = 0;
		}
	}
}

void IdeShellHost::ChangeDirectory(IdeShell& shell, Value arg) {
	MountManager& mm = MountManager::System();
	String path_str = GetFirstValue(arg);
	if (path_str.IsEmpty()) {
		#if INTERNAL_POSIX
		path_str = "/";
		#endif
	}
	else if (!IsFullInternalDirectory(path_str)) {
		path_str = AppendInternalFileName(shell.cwd, path_str);
		path_str = NormalizeInternalPath(path_str);
	}
	VfsPath path(StrVfs(path_str));
	if (!path.IsValidFullPath()) {
		out = "error: not valid full path '" + path_str + "'";
	}
	else if (mm.DirectoryExists(path)) {
		shell.SetCurrentDirectory(path);
	}
	else {
		out = "error: no directory '" + path + "'";
	}
}

#ifdef flagNET
void IdeShellHost::StartIntranet(IdeShell& shell, Value arg) {
	Thread::Start(IntranetDaemon);
}
#endif



#ifndef flagV1
void InitShellHost(MetaEnvironment& env, IdeShellHost& host) {
	//host.AddProgram("blog", [&env](IdeShell& is, Value val){EcsExt(env,is,val);});
}

void ShellReg_MetaEnv(IdeShellHost& host) {
	InitShellHost(MetaEnv(), host);
}

void EcsExt(MetaEnvironment& env, IdeShell& shell, Value value) {
	MountManager& mm = MountManager::System();
	
	ConsoleCtrl* con = shell.host.GetConsole();
	if (!con)
		return;
	
	VfsPath cwd = shell.cwd;
	
	// Empty path is not sane... check current project file
	if (cwd.IsEmpty()) {
		MountManager::MountPoint* mp = mm.Find(INTERNAL_ROOT_FILE("prj"));
		if (!mp) return;
		if (mp->vfs) {
			Vector<VfsItem> items;
			mp->vfs->GetFiles(VfsPath(), items);
			if (items.GetCount() == 1) {
				cwd.Set(INTERNAL_ROOT_FILE("prj"));
				cwd.Add(items[0].name);
			}
		}
	}
	
	VfsPath rel_path;
	MountManager::MountPoint* mp = mm.Find(cwd, &rel_path);
	
	if (mp && mp->vfs) {
		MetaEnvironment* env = dynamic_cast<MetaEnvironment*>(&*mp->vfs);
		if (env) {
			VfsValue* n = env->root.FindPath(rel_path);
			if (!n)
				return;
			
			TODO
			#if 0
			if (n->kind == METAKIND_ECS_SPACE) {
				String ent_name = "Blog";
				n = &n->GetAdd(ent_name, "", METAKIND_ECS_ENTITY);
			}
			else if (n->kind >= METAKIND_ECS_COMPONENT_BEGIN &&
					 n->kind <= METAKIND_ECS_COMPONENT_END) {
				n = n->owner;
			}
			
			if (n->kind == METAKIND_ECS_ENTITY) {
				EntityEditorCtrl& c = con->EcsExt<EntityEditorCtrl>(shell, value);
			}
			#endif
		}
	}
}
#endif

END_UPP_NAMESPACE
