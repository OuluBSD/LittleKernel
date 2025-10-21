#ifndef _AI_Ctrl_Perspective_h_
#define _AI_Ctrl_Perspective_h_

NAMESPACE_UPP


class PerspectiveCtrl : public AiComponentCtrl {
	Splitter hsplit;
	ArrayCtrl attrs, user;
	WithSocialBelief<Ctrl> info;
	
public:
	typedef PerspectiveCtrl CLASSNAME;
	PerspectiveCtrl();
	
	void Data() override;
	void ToolMenu(Bar& bar) override;
	void Do(int fn);
	void OnValueChange();
};

INITIALIZE(PerspectiveCtrl)

// TODO Rename to PerspectiveSolver
class PerspectiveProcess : public SolverBase {
	
public:
	enum {
		PHASE_GET_POSITIVE_ATTRS,
		PHASE_GET_NEGATIVE_ATTRS,
		
		PHASE_COUNT
	};
	
public:
	typedef PerspectiveProcess CLASSNAME;
	PerspectiveProcess();
	
	int GetPhaseCount() const override;
	void DoPhase() override;
	
	static PerspectiveProcess& Get(DatasetPtrs p);
	
	
};



END_UPP_NAMESPACE

#endif
 
