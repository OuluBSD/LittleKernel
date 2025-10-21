#ifndef _EscAnim_Scene_h_
#define _EscAnim_Scene_h_


class AnimScene {
	
	
protected:
	friend class Animation;
	AnimObject root;
	String name;
	int length = 0;
	bool repeat = false;
	
public:
	typedef AnimScene CLASSNAME;
	AnimScene();
	
	
	
	AnimScene& SetLength(int kpi) {length = kpi; return *this;}
	
	AnimObject& GetRoot() {return root;}
	int GetLength() const {return length;}
	bool IsRepeating() const {return repeat;}
	
};


#endif
