#ifndef _AI_Core_Marketing_Owner_h_
#define _AI_Core_Marketing_Owner_h_




struct LeadOpportunity;


struct PlatformNeed {
	bool enabled = false;
	
	void Visit(Vis& v) {
		v.Ver(1)
		(1)	("enabled", enabled)
			;
	}
};

struct Need {
	String name;
	Vector<String> causes, messages;
	Array<PlatformNeed> platforms;
	
	void Visit(Vis& v) {
		v.Ver(1)
		(1)	("name", name)
			("causes", causes)
			("messages", messages)
			.VisitVector("platforms", platforms)
			;
	}
};

struct RoleEvent {
	String text;
	VectorMap<int,String> entries;
	
	void Visit(Vis& v) {
		v.Ver(1)
		(1)	("text", text)
			("entries", entries)
			;
	}
};

struct RoleAction {
	struct NeedCause : Moveable<NeedCause> {
		int need_i = -1;
		int cause_i = -1;
		void Visit(Vis& v) {v("n", need_i)("c", cause_i);}
	};
	String name;
	Vector<NeedCause> need_causes;
	Array<RoleEvent> events;
	
	
	void Visit(Vis& v) {
		v.Ver(1)
		(1)	("name", name)
			.VisitVector("need_causes", need_causes)
			.VisitVector("events", events)
			;
	}
	int FindEvent(const String& event) const;
	
};

struct Role {
	String name;
	Array<Need> needs;
	Array<RoleAction> actions;
	
	void Visit(Vis& v) {
		v.Ver(1)
		(1)	("name", name)
			.VisitVector("needs", needs)
			.VisitVector("actions", actions)
			;
	}
	int FindAction(const String& name) const;
	
};

struct Owner : Component
{
	String name;
	Date born = Null;
	String description, environment;
	Array<Role> roles;
	
	CLASSTYPE(Owner)
	Owner(VfsValue& owner) : Component(owner) {}
	int FindRole(const String& name) const;
	void Store();
	void Load(String name);
	
	String GetName() const override {return name;}
	void Visit(Vis& v) override {
		v.Ver(1)
		(1)	("name", name)
			("born", born)
			("description", description)
			("environment", environment)
			.VisitVector("roles", roles)
			;
	}
	
	int GetOpportunityScore(const LeadOpportunity& opp) const;
	
	static void CopyOld();
};

INITIALIZE(Owner);




#endif
