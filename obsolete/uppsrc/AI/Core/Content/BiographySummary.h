#ifndef _AI_Core_BiographySummary_h_
#define _AI_Core_BiographySummary_h_




class BiographySummaryProcess : public SolverBase {
	
public:
	enum {
		PHASE_FIX_SUMMARY_HASHES,
		PHASE_SUMMARIZE_USING_EXISTING,
		PHASE_SUMMARIZE,
		PHASE_SUMMARIZE_ELEMENTS_USING_EXISTING,
		PHASE_SUMMARIZE_ELEMENTS,
		
		PHASE_COUNT,
	};
	
	Ptr<Profile> profile;
	Ptr<BiographyPerspectives> snap;
	
public:
	typedef BiographySummaryProcess CLASSNAME;
	BiographySummaryProcess();
	
	int GetPhaseCount() const override;
	int GetBatchCount(int phase) const override;
	int GetSubBatchCount(int phase, int batch) const override;
	void DoPhase() override;
	
	static BiographySummaryProcess& Get(Profile& p, BiographyPerspectives& snap);
	
private:
	
	void FixSummaryHashes();
	void SummarizeUsingExisting();
	bool SummarizeBase(int fn, BiographySummaryProcessArgs& args);
	void Summarize();
	void SummarizeElementsUsingExisting();
	void SummarizeElements();
	void OnProcessSummarize(String res);
	void OnProcessSummarizeElements(String res);
	
};




#endif
