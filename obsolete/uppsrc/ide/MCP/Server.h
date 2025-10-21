#ifndef _ide_MCP_Server_h_
#define _ide_MCP_Server_h_

class McpServer : public McpServerCore {
public:
    typedef McpServer CLASSNAME;
    bool Start(int port = 0); // 0 -> choose default
    void Stop();
protected:
    virtual String HandleExtended(const McpRequest& req) override;
};

extern McpServer sMcpServer;

#endif
