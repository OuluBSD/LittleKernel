#ifndef _Geometry_GeomController_h_
#define _Geometry_GeomController_h_


struct ControllerSource {
	virtual ~ControllerSource() {}
	
	//virtual bool GetLocation(float* matrix4x4) const {return false;}
	virtual void GetVelocity(float* v3) const = 0;
	virtual void GetAngularVelocity(float* v3) const = 0;
	
};

struct ControllerState {
	ControllerSource* source = 0;
	ControllerMatrix props;
	
	const ControllerSource& GetSource() const {ASSERT(source); return *source;}
	const ControllerMatrix& GetControllerProperties() const {return props;}
	
};


#define COPY2(dst, from) for(int i = 0; i < 2; i++) dst[i] = from[i]
#define COPY3(dst, from) for(int i = 0; i < 3; i++) dst[i] = from[i]
#define COPY4(dst, from) for(int i = 0; i < 4; i++) dst[i] = from[i]
#define COPY3x3(dst, from) for(int i = 0; i < 3; i++) for(int j = 0; j < 3; j++) (dst)[i][j] = (from)[i][j]
#define COPY4x4(dst, from) for(int i = 0; i < 4; i++) for(int j = 0; j < 4; j++) (dst)[i][j] = (from)[i][j]
	
	
#endif
