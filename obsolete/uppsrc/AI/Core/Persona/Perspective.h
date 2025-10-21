#ifndef _AI_Core_Persona_Perspective_h_
#define _AI_Core_Persona_Perspective_h_





struct PerspectiveComponent : Component
{
	COMPONENT_CONSTRUCTOR(PerspectiveComponent)
	
	struct Attr : Moveable<Attr> {
		String		positive;
		String		negative;
		void Visit(Vis& v) {v.Ver(1)(1)("positive",positive)("negative",negative);}
	};
	String			description;
	String			reference;
	Vector<Attr>	attrs;
	Vector<String>	user;
	
	void Visit(Vis& v) override {
		v.Ver(1)
		(1)	("description", description)
			("reference", reference)
			("attrs", attrs, VISIT_VECTOR)
			("user", user);
	}
	
};

INITIALIZE(PerspectiveComponent);




#endif
