#ifndef _AI_Core_Artist_h_
#define _AI_Core_Artist_h_




struct Artist : Component
{
	VectorMap<String,Value> data;
	
	COMPONENT_CONSTRUCTOR(Artist)
	void Visit(Vis& v) override {
		v.Ver(1)
		(1)	("data", data);
	}
	
	Value& Data(String key) {return data.GetAdd(key);}
};

INITIALIZE(Artist)




#endif
