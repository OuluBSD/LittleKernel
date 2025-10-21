#ifndef _AI_Core_PlatformProfile_h_
#define _AI_Core_PlatformProfile_h_





struct PlatformBiographyPlatform;

class PlatformProfileProcess : public SolverBase {
	
public:
	enum {
		PHASE_ANALYZE_PROFILE_EPK_PHOTO_AI_PROMPTS,
		PHASE_ANALYZE_PROFILE_EPK_SUMMARIZE_PHOTO_AI_PROMPTS,
		PHASE_ANALYZE_PROFILE_EPK_PHOTO_DALLE2_EXAMPLES,
		
		PHASE_COUNT,
	};
	
	
	struct ProfileEPKTask : Moveable<ProfileEPKTask> {
		PlatformBiographyPlatform* pba = 0;
		const Platform* plat = 0;
		const PlatformAnalysis* pa = 0;
		const PlatformAnalysisPhoto* pap = 0;
		PlatformAnalysisPhoto* prof_pap = 0;
		PhotoPrompt* pp = 0;
	};
	Vector<ProfileEPKTask> prof_epk_tasks;
	String file_dir;
	
	void TraverseProfileEPKTasks();
	
	
public:
	typedef PlatformProfileProcess CLASSNAME;
	PlatformProfileProcess();
	
	int GetPhaseCount() const override;
	int GetBatchCount(int phase) const override;
	int GetSubBatchCount(int phase, int batch) const override;
	void DoPhase() override;
	
	static PlatformProfileProcess& Get(const DatasetPtrs& p, String file_dir);
	
private:
	
	void ProcessAnalyzeProfileEpkPhotoAiPrompts();
	void ProcessAnalyzeProfileEpkSummarizePhotoAiPrompts();
	void ProcessAnalyzeProfileEpkPhotoDalle2Examples();
	void OnProcessAnalyzeProfileEpkPhotoAiPrompts(String res);
	void OnProcessAnalyzeProfileEpkSummarizePhotoAiPrompts(String res);
	void OnProcessAnalyzeProfileEpkPhotoDalle2Examples(Array<Image>& images, int batch);
	
};




#endif
