#ifndef _AI_ImgCtrl_VideoPrompt_h_
#define _AI_ImgCtrl_VideoPrompt_h_

NAMESPACE_UPP


class VideoPromptMakerCtrl : public AiComponentCtrl {
	Splitter vsplit, storyboard_prompt_split;
	ArrayCtrl storyboard_parts;
	ArrayCtrl storyboard_prompt_keys, storyboard_prompt_values;
	ArrayCtrl text_storyboard_parts;
	
public:
	typedef VideoPromptMakerCtrl CLASSNAME;
	VideoPromptMakerCtrl();
	
	void Data() override;
	void DataPrompt();
	void OnValueChange();
	void ToolMenu(Bar& bar) override;
	void Do(int fn);
	
	
};

INITIALIZE(VideoPromptMakerCtrl)

class VideoSolver : public SolverBase {
	
public:
	enum {
		PHASE_MAKE_STORYBOARD,
		PHASE_STORYBOARD_TO_PARTS_AND_DALLE_PROMPTS,
		PHASE_TEXT_TO_PARTS,
		PHASE_IMAGE_SEARCH_PROMPT,
		PHASE_FILL_STORY_PROMPTS,
		PHASE_GET_IMAGES,
		PHASE_SAFE_PROMPTS,
		PHASE_GET_SAFE_IMAGES,
		PHASE_GET_RUNWAY_STORYBOARD,
		
		PHASE_COUNT,
	};
	
	Song* song = 0;
	Release* snap = 0;
	Entity* entity = 0;
	
	Vector<String> tmp_lines;
	
	int arg_image_count = 4;
	
	void OnProcessAnalyzeRoleScores(String res);
	
public:
	typedef VideoSolver CLASSNAME;
	VideoSolver();
	
	int GetPhaseCount() const override;
	void DoPhase() override;
	
	static VideoSolver& Get(Song& c);
	
};


END_UPP_NAMESPACE

#endif
 
