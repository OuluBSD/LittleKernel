#ifndef _AI_Core_Marketing_Audience_h_
#define _AI_Core_Marketing_Audience_h_




class AudienceProcess : public SolverBase {
	
public:
	enum {
		PHASE_AUDIENCE_PROFILE_CATEGORIES,
		
		PHASE_COUNT,
	};
	
	
public:
	typedef AudienceProcess CLASSNAME;
	AudienceProcess();
	
	int GetPhaseCount() const override;
	int GetBatchCount(int phase) const override;
	int GetSubBatchCount(int phase, int batch) const override;
	void DoPhase() override;
	
	static AudienceProcess& Get(Profile& p, BiographyPerspectives& snap);
	
private:
	
	void ProcessAudienceProfileCategories();
	void OnProcessAudienceProfileCategories(String res);
	
};




#endif
