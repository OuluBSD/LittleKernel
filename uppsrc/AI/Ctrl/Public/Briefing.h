#ifndef _AI_Ctrl_Briefing_h_
#define _AI_Ctrl_Briefing_h_

NAMESPACE_UPP


class ReleaseBriefingCtrl : public AiComponentCtrl {
	WithKeyValueList<Ctrl> values;
	ArrayCtrl list;
	Splitter vsplit;
	
	#define ALBUM_BRIEFING_LIST \
		ITEM(GENERAL_HISTORY, t_("General History"), t_("Common factors in the history of components")) \
		ITEM(LIFE_BEGINNING, t_("Life in the beginning"), t_("Life of the author in the beginning of the album timeline")) \
		ITEM(LIFE_END, t_("Life in the end"), t_("Life of the author in the end of the album timeline")) \
		ITEM(LIMITS_BEGIN, t_("Limitations of the beginning"), t_("Significant events that happened immediately before all components in this album")) \
		ITEM(LIMITS_END, t_("Limitations of the end"), t_("Significant events that happened immediately after all components in this album")) \
		ITEM(LIFE_SCHOOL, t_("School of the Author"), t_("Where did the author go to school and what was it like there?")) \
		ITEM(LIFE_RELATIONSHIPS, t_("Relationships of the Author"), t_("What kind of dating relationships did the author have?")) \
		ITEM(LIFE_NERDINESS, t_("Nerdiness of the Author"), t_("What did the author do on the computer in general?")) \
		ITEM(LIFE_MUSIC_PRODUCTION, t_("Music Production of the Author"), t_("How did the author contribute to his/her career as a music producer?")) \
		ITEM(LIFE_PHYSICAL_EXERCISE, t_("Physical Exercise of the Author"), t_("How did the author maintain his/her physical condition?")) \
		ITEM(LIFE_ART, t_("Art of the Author"), t_("What kind of art did the artist make?")) \
		ITEM(LIFE_FAMILY, t_("Family of the Author"), t_("What kind of family did the author live in or what important family members did he/she have?")) \
		ITEM(HEADSPACE_POSITIVITY, t_("Positive Headspace of the Author"), t_("In what way did the author express his/her positivity in the text?")) \
		ITEM(HEADSPACE_NEGATIVITY, t_("Negative Headspace of the Author"), t_("In what way did the author express his/her negativity in the text?")) \
		ITEM(THEME_COMPOSITION, t_("Composition Theme"), t_("Common musical style of components")) \
		ITEM(THEME_LYRICAL, t_("Lyrical Theme"), t_("Common lyrical style of components")) \
		ITEM(THEME_PRODUCTION, t_("Production Theme"), t_("Common genre and production limitations and features of components")) \
	
	
	enum {
		#define ITEM(k,s,d) k,
		ALBUM_BRIEFING_LIST
		#undef ITEM
		ITEM_COUNT
	};
	
public:
	typedef ReleaseBriefingCtrl CLASSNAME;
	ReleaseBriefingCtrl();
	
	void Data() override;
	void ToolMenu(Bar& bar) override {}
	void OnListCursor();
	void OnValueChange();
	
};

INITIALIZE(ReleaseBriefingCtrl)


END_UPP_NAMESPACE

#endif
 
