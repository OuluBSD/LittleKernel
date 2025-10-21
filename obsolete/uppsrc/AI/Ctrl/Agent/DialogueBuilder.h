#ifndef _DialogueBuilder_DialogueBuilder_h_
#define _DialogueBuilder_DialogueBuilder_h_


#define HAVE_DIALOGUE_BUILDER 1

#if HAVE_DIALOGUE_BUILDER
struct DialogueBuilder : CppBuilder {
	ScriptReferenceMakerCtrl edit;
	
	typedef DialogueBuilder CLASSNAME;
	virtual bool   BuildPackage(const String& package, Vector<String>& linkfile, Vector<String>&, String& linkoptions,
		const Vector<String>& all_uses, const Vector<String>& all_libraries, int optimize);
	virtual bool   Link(const Vector<String>& linkfile, const String& linkoptions, bool createmap);
	virtual bool   Preprocess(const String& package, const String& file, const String& target, bool asmout);

	bool           PreprocessJava(String file, String target, String options, String package, const Package& pkg);
	Time           AddClassDeep(String& link, String dir, String reldir);

};

INITIALIZE(DialogueBuilder)
#endif

#endif
 
