#ifndef _Eon_Ecs_Service_h_
#define _Eon_Ecs_Service_h_


class EcsService : public DaemonService {
	EnetServiceServer* server = 0;
	EntityPtr bound_entity;
	EntityPtr ResolveEntity(VfsValue& root, String path);
	
public:
	// Remote connection
	//GeomSerializer write, read;
	
	void ReceiveGeoms(Stream& in, Stream& out);
	void SendEngine(Stream& in, Stream& out);
	
public:
	typedef EcsService CLASSNAME;
	
	EcsService();
	bool Init(String name) override;
	void Update() override;
	void Stop() override;
	void Deinit() override;
	
};


#endif
