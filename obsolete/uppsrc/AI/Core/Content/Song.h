#ifndef _AI_Core_Song_h_
#define _AI_Core_Song_h_




struct Song : Component
{
	Vector<String>						text_storyboard_searches;
	Vector<String>						text_storyboard_prompts;
	Vector<String>						text_storyboard_prompts_safe;
	Vector<String>						text_storyboard_prompts_runway;
	VectorMap<String,int>				text_storyboard_parts;
	VectorMap<String,Vector<String>>	storyboard_prompts;
	VectorMap<String,String>			storyboard_parts;
	Vector<Vector<int64>>				text_storyboard_hashes;
	
	#if 0
	
	// Public
	int							default_line_syllables = 0;
	int							default_attr_count = 7;
	int							theme_cursor = -1;
	int							part_cursor = -1;
	
	#endif
	
	
	String						entity; // TODO remove
	String						prj_name;
	String						origins;
	String						reference;
	String						scripts_file_title;
	String						style;
	
	
	Release*					snapshot = 0;
	
	
	COMPONENT_CONSTRUCTOR(Song)
	void Store(Release& snap);
	void LoadTitle(Release& snap, String title);
	void ReloadStructure();
	String GetAnyTitle(Entity& a) const;
	
	void Visit(Vis& v) override {
		v.Ver(1)
		(1)	("entity", entity)
			("prj_name", prj_name)
			("origins", origins)
			("reference", reference)
			("file_title", scripts_file_title)
			("music_style", style)
			("storyboard_parts", storyboard_parts)
			("storyboard_prompts", storyboard_prompts)
			("text_storyboard_parts", text_storyboard_parts)
			("text_storyboard_searches", text_storyboard_searches)
			("text_storyboard_prompts", text_storyboard_prompts)
			("text_storyboard_prompts_safe", text_storyboard_prompts_safe)
			("text_storyboard_hashes", text_storyboard_hashes)
		#if 0
			("default_line_syllables", default_line_syllables)
			("default_attr_count", default_attr_count)
			("theme_cursor", theme_cursor)
			("part_cursor", part_cursor)
		#endif
			;
	}
	
};

INITIALIZE(Song);



#endif
