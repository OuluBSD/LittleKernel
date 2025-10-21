#ifndef _AI_Core_Runtime_CodeProcess_h_
#define _AI_Core_Runtime_CodeProcess_h_



class CodeProcess : public AiProcessBase {
	
public:
	enum {

		PHASE_COUNT
	};

public:
	typedef CodeProcess CLASSNAME;
	CodeProcess();

	int GetPhaseCount() const override;
	int GetBatchCount(int phase) const override;
	int GetSubBatchCount(int phase, int batch) const override;
	void DoPhase() override;

	static CodeProcess& Get(String id);
};



#endif
