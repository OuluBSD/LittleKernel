#ifndef _AI_Core_Notepad_h_
#define _AI_Core_Notepad_h_





struct Notepad : Component
{
	struct Note : Moveable<Note> {
		String title;
		String outcome;
		String reference;
		String description;
		Time created;
		void Visit(Vis& v) {
			v.Ver(1)
			(1)	("title", title)
				("outcome", outcome)
				("reference", reference)
				("description", description)
				("created", created)
				;
		}
	};
	Vector<Note> notes;
	
	COMPONENT_CONSTRUCTOR(Notepad)
	
	void Visit(Vis& v) override {
		v.Ver(1)
		(1)	("notes", notes, VISIT_VECTOR)
			;
	}
	
};

INITIALIZE(Notepad)




#endif
