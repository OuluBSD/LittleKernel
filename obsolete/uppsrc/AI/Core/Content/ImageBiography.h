#ifndef _AI_Core_ImageBiography_h_
#define _AI_Core_ImageBiography_h_





class ImageBiographyProcess : public SolverBase {
	
public:
	enum {
		PHASE_ANALYZE_IMAGE_BIOGRAPHY,
		
		PHASE_COUNT,
	};
	
	Profile* p = 0;
	BiographyPerspectives* snap = 0;
	
	
	struct VisionTask : Moveable<VisionTask> {
		BioImage* bimg = 0;
		String jpeg;
	};
	Vector<VisionTask> vision_tasks;
	void TraverseVisionTasks();
	
	
	
	
public:
	typedef ImageBiographyProcess CLASSNAME;
	ImageBiographyProcess();
	
	int GetPhaseCount() const override;
	int GetBatchCount(int phase) const override;
	int GetSubBatchCount(int phase, int batch) const override;
	void DoPhase() override;
	
	static ImageBiographyProcess& Get(Profile& p, BiographyPerspectives& snap);
	
private:
	
	void ProcessAnalyzeImageBiography();
	void OnProcessAnalyzeImageBiography(String res);
	
};




#endif
