#ifndef _EscAnim_Animation_h_
#define _EscAnim_Animation_h_


class Animation {
	int keys_per_second = 4;
	
public:
	Array<AnimScene> scenes;
	int active_scene = 0;
	
	
public:
	typedef Animation CLASSNAME;
	Animation();
	
	void Clear();
	
	AnimScene& AddScene(String name);
	AnimScene& GetActiveScene() {return scenes[active_scene];}
	
	int GetKeysFromTime(double seconds);
	int GetKeysPerSecond() const {return keys_per_second;}
	
};


#endif
