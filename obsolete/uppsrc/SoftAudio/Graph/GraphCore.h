#ifndef _SoftAudioGraph_GraphCore_h_
#define _SoftAudioGraph_GraphCore_h_

#include "Node.h"
#include "Connection.h"

NAMESPACE_SAGRAPH_BEGIN

class Graph {
public:
    int AddNode(One<Node> node);
    int AddNodeWithName(const String& name, One<Node> node);
    void Connect(int from, int to, int from_port = 0, int to_port = 0, float gain = 1.0f);
    int  ConnectReturnIndex(int from, int to, int from_port = 0, int to_port = 0, float gain = 1.0f);
    int  ConnectWithName(const String& name, int from, int to, int from_port = 0, int to_port = 0, float gain = 1.0f);
    bool Compile(String& error);
    void Prepare(const ProcessContext& ctx);
    void ProcessBlock();
    bool SetParam(int node_index, const String& id, double value);
    bool SetParam(const String& node_name, const String& id, double value);
    bool SetParams(int node_index, const VectorMap<String, double>& params);
    bool SetParams(const String& node_name, const VectorMap<String, double>& params);
    bool SetParams(int node_index, std::initializer_list<std::pair<const char*, double>> params);
    bool SetParams(const String& node_name, std::initializer_list<std::pair<const char*, double>> params);
    bool SetEdgeName(int edge_index, const String& name);
    int  FindEdge(const String& name) const;
    const Edge* GetEdge(int edge_index) const;
    Edge*       GetEdge(int edge_index);
    bool SetNodeName(int node_index, const String& name);
    int  FindNode(const String& name) const;
    const String& GetNodeName(int node_index) const;

    // For reference apps: allow writing into a sink node that commits to file/device.
    void SetBlockSize(int bs) { ctx_.block_size = bs; }
    void SetSampleRate(int sr) { ctx_.sample_rate = sr; }

private:
    struct CompiledNode {
        Node* node = nullptr;
        Vector<int> inputs; // indices of upstream nodes
        Bus output;
    };

    Array<Node> nodes_;
    Vector<String> node_names_;
    VectorMap<String, int> name_to_index_;
    Array<Edge> edges_;
    VectorMap<String, int> edge_name_to_index_;
    Array<CompiledNode> order_; // topologically sorted nodes with resolved inputs
    ProcessContext ctx_;
};

NAMESPACE_SAGRAPH_END

#endif
