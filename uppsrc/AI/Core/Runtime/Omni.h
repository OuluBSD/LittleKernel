#ifndef _AICore_Omni_h_
#define _AICore_Omni_h_

struct CompletionThread : Component {
	
public:
	struct Session {
		struct Item : Moveable<Item> {
			String txt;
			
			void Visit(Vis& v) {
				v.Ver(1)
				(1)	("txt", txt);
			}
		};
		Vector<Item> items;
		
		void Visit(Vis& v) {
			v.Ver(1)
			(1)	("items", items, VISIT_VECTOR);
		}
	};
	
	Array<Session> sessions;
	
public:
	CLASSTYPE(CompletionThread);
	CompletionThread(VfsValue& n) : Component(n) {}
	
	void Visit(Vis& v) override {
		v.Ver(1)
		(1)	("sessions", sessions, VISIT_VECTOR);
	}
};

struct ChatThread : Component {
	
	struct Attachment : Moveable<Attachment> {
		String mime;
		String filepath; // if any
		String data; // if any
		String source_info; // if any
		void Visit(Vis& v) {
			v.Ver(1)
			(1)	("mime", mime)
				("filepath", filepath)
				("data", data)
				("source_info", source_info);
		}
	};
	struct Session {
		struct Item : Moveable<Item> {
			AiMsgType type = MSG_NULL;
			String content;
			String username;
			Time created;
			Vector<Attachment> attachments;
			
			void Visit(Vis& v) {
				v.Ver(1)
				(1)	("type", (int&)type)
					("content", content)
					("username", username)
					("created", created)
					("attachments", attachments, VISIT_VECTOR)
					;
			}
		};
		String name;
		Time created;
		Time changed;
		Vector<Item> items;
		
		void Visit(Vis& v) {
			v.Ver(1)
			(1)	("name", name)
				("created", created)
				("changed", changed)
				("items", items, VISIT_VECTOR)
				;
		}
	};
	
	Array<Session> sessions;
	
public:
	CLASSTYPE(ChatThread);
	ChatThread(VfsValue& n) : Component(n) {}
	
	void Visit(Vis& v) override {
		v.Ver(1)
		(1)	("sessions",sessions, VISIT_VECTOR)
			;
	}
};


struct ChainThread : Component {
	CLASSTYPE(ChainThread)
	ChainThread(VfsValue& n) : Component(n) {}
	void Visit(Vis& v) override {
		v.Ver(0)
			;
	}
};

struct SpeechTranscriptionThread : Component {
	CLASSTYPE(SpeechTranscriptionThread)
	SpeechTranscriptionThread(VfsValue& v) : Component(v) {}
	void Visit(Vis& v) override {v.Ver(0);}
};

struct SpeechGenerationThread : Component {
	CLASSTYPE(SpeechGenerationThread)
	SpeechGenerationThread(VfsValue& v) : Component(v) {}
	void Visit(Vis& v) override {v.Ver(0);}
};

struct ImageGenerationThread : Component {
	CLASSTYPE(ImageGenerationThread)
	ImageGenerationThread(VfsValue& v) : Component(v) {}
	void Visit(Vis& v) override {v.Ver(0);}
};

struct ImageVisionThread : Component {
	CLASSTYPE(ChatThread)
	ImageVisionThread(VfsValue& v) : Component(v) {}
	void Visit(Vis& v) override {v.Ver(0);}
};

struct MetaEnvThread : Component {
	CLASSTYPE(MetaEnvThread)
	MetaEnvThread(VfsValue& v) : Component(v) {}
	void Visit(Vis& v) override {v.Ver(0);}
};

#if 0
class OmniThread :
	public CompletionThread,
	public ChatThread,
	public SpeechTranscriptionThread,
	public SpeechGenerationThread,
	public ImageGenerationThread,
	public ImageVisionThread,
	public MetaEnvThread,
	public SoundDiscussionManager
{
	
public:
	typedef OmniThread CLASSNAME;
	OmniThread();
	
	void Visit(Vis& v) override;
	void OnPhraseEnd(SoundPhrase& p) override;
	void OnMessageEnd(SoundMessage&) override;
	void OnDiscussionEnd(SoundDiscussion&) override;
	
	static OmniThread& Single() {static OmniThread m; return m;}
};
#endif



#endif
