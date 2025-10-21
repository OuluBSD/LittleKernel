#ifndef _Intranet_FtpClient_h
#define _Intranet_FtpClient_h

#ifdef flagFTP
NAMESPACE_UPP

class FtpBrowser : public TopWindow {
	FileList list;
	WithAccountLayout<ParentCtrl> account;
	WithServerLayout<ParentCtrl> server;

	TabDlg settings;
	Ftp browser;
	String workdir, rootdir;
	MenuBar mainmenu;
	Progress progress;

	void Connect();
	void Disconnect();
	void Browse();
	void Action();
	void Information(const FileList::File& entry);
	void Download(const String& file, const String& path);
	bool DownloadProgress(int64 total, int64 done);

	void About();
	void Settings();

	void MainMenu(Bar& bar);
	void FileMenu(Bar& bar);
	void HelpMenu(Bar& bar);

	void ContextMenu(Bar& bar);

	void UpdateGui() { ProcessEvents(); }

public:
	typedef FtpBrowser CLASSNAME;

	void Serialize(Stream& s);
	FtpBrowser();
	~FtpBrowser();
};

int		FtpGet(const String& path, Stream& out, const String& host, int port = 21, const String& user = Null, const String& pass = Null,
            Gate2<int64, int64> progress = false, Callback whenwait = CNULL, bool binary=true, bool passive=true, bool ssl = false);
int		FtpPut(Stream& in, const String& path, const String& host, int port = 21, const String& user = Null, const String& pass = Null,
            Gate2<int64, int64> progress = false, Callback whenwait = CNULL, bool binary=true, bool passive=true, bool ssl = false);
bool	ParseFtpDirEntry(const String& in, Ftp::DirList& out);

END_UPP_NAMESPACE
#endif

#endif
