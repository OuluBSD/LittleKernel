#ifndef _AICore2_Agent_h_
#define _AICore2_Agent_h_

#undef FINISHED
#undef Status

class AgentTaskExt : public VfsValueExt {
	
public:
	typedef enum : int {
		CLEARED,
		STARTED,
		
		FINISHED,
	} Status;
	
	Status status = CLEARED;
	String prompt;
	
public:
	CLASSTYPE(AgentTaskExt);
	AgentTaskExt(VfsValue& n);
	
	void Visit(Vis& v) override;
};


#endif
