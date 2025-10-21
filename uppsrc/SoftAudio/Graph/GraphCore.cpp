#include "Graph.h"

NAMESPACE_SAGRAPH_BEGIN

int Graph::AddNode(One<Node> node) {
    int idx = nodes_.GetCount();
    nodes_.Add(node.Detach());
    node_names_.Add(String());
    return idx;
}

int Graph::AddNodeWithName(const String& name, One<Node> node) {
    int idx = AddNode(pick(node));
    SetNodeName(idx, name);
    return idx;
}

void Graph::Connect(int from, int to, int from_port, int to_port, float gain) {
    Edge e; e.from = from; e.to = to; e.from_port = from_port; e.to_port = to_port; e.gain = gain;
    edges_.Add(e);
}

int Graph::ConnectReturnIndex(int from, int to, int from_port, int to_port, float gain) {
    Edge e; e.from = from; e.to = to; e.from_port = from_port; e.to_port = to_port; e.gain = gain;
    int idx = edges_.GetCount();
    edges_.Add(pick(e));
    return idx;
}

int Graph::ConnectWithName(const String& name, int from, int to, int from_port, int to_port, float gain) {
    int idx = ConnectReturnIndex(from, to, from_port, to_port, gain);
    SetEdgeName(idx, name);
    return idx;
}

bool Graph::Compile(String& error) {
    // Kahn's algorithm for topological sorting.
    int n = nodes_.GetCount();
    Vector<Vector<int>> adj(n);
    Vector<int> indeg(n, 0);
    for(const auto& e : edges_) {
        adj[e.from].Add(e.to);
        indeg[e.to]++;
    }
    Vector<int> q;
    for(int i = 0; i < n; ++i) if(indeg[i] == 0) q.Add(i);
    order_.Clear();
    int visited = 0;
    while(!q.IsEmpty()) {
        int u = q.Top(); q.Drop();
        visited++;
        CompiledNode& cn = order_.Add();
        cn.node = &nodes_[u];
        // Gather input indices for u
        for(const auto& e : edges_)
			if(e.to == u)
				cn.inputs.Add(e.from);
        for(int v : adj[u]) { if(--indeg[v] == 0) q.Add(v); }
    }
    if(visited != n) { error = "Graph has cycles (not supported yet)"; order_.Clear(); return false; }
    return true;
}

void Graph::Prepare(const ProcessContext& ctx) {
    ctx_ = ctx;
    for(auto& cn : order_) {
        cn.node->Prepare(ctx_);
        // set output buffer channels according to node spec if provided (0=dynamic, default to 2)
        int ch = cn.node->GetOutputSpec(0).channels;
        cn.output.SetSize(ctx_.block_size, ch ? ch : 2);
    }
}

void Graph::ProcessBlock() {
    // For each node in order, collect input bus pointers, process, and store into its output bus.
    for(int i = 0; i < order_.GetCount(); ++i) {
        CompiledNode& cn = order_[i];
        Vector<Bus*> inputs;
        for(int src_idx : cn.inputs) {
            // find compiled node index for src_idx
            // order_ is in topo order; src might appear before current i
            // We can map node* to compiled index; for simplicity, scan.
            for(int j = 0; j < order_.GetCount(); ++j) {
                if(order_[j].node == &nodes_[src_idx]) { inputs.Add(&order_[j].output); break; }
            }
        }
        cn.output.SetSize(ctx_.block_size, cn.output.channels ? cn.output.channels : 2);
        cn.node->Process(ctx_, inputs, cn.output);
    }
    ctx_.frame_cursor += ctx_.block_size;
}

bool Graph::SetParam(int node_index, const String& id, double value) {
    if(node_index < 0 || node_index >= nodes_.GetCount()) return false;
    Node* n = &nodes_[node_index];
    if(!n) return false;
    return n->SetParam(id, value);
}

bool Graph::SetParam(const String& node_name, const String& id, double value) {
    int idx = FindNode(node_name);
    if(idx < 0) return false;
    return SetParam(idx, id, value);
}

bool Graph::SetParams(int node_index, const VectorMap<String, double>& params) {
    bool ok = true;
    for(int i = 0; i < params.GetCount(); ++i)
        ok &= SetParam(node_index, params.GetKey(i), params[i]);
    return ok;
}

bool Graph::SetParams(const String& node_name, const VectorMap<String, double>& params) {
    int idx = FindNode(node_name);
    if(idx < 0) return false;
    return SetParams(idx, params);
}

bool Graph::SetParams(int node_index, std::initializer_list<std::pair<const char*, double>> params) {
    bool ok = true;
    for(const auto& p : params)
        ok &= SetParam(node_index, String(p.first), p.second);
    return ok;
}

bool Graph::SetParams(const String& node_name, std::initializer_list<std::pair<const char*, double>> params) {
    int idx = FindNode(node_name);
    if(idx < 0) return false;
    return SetParams(idx, params);
}

bool Graph::SetNodeName(int node_index, const String& name) {
    if(node_index < 0 || node_index >= nodes_.GetCount()) return false;
    // remove old mapping if present
    String old = node_names_[node_index];
    if(!IsNull(old)) {
        int pi = name_to_index_.Find(old);
        if(pi >= 0) name_to_index_.Remove(pi);
    }
    node_names_[node_index] = name;
    if(!IsNull(name)) {
        int fi = name_to_index_.Find(name);
        if(fi >= 0) name_to_index_[fi] = node_index;
        else name_to_index_.Add(name, node_index);
    }
    return true;
}

int Graph::FindNode(const String& name) const {
    int fi = name_to_index_.Find(name);
    return fi >= 0 ? name_to_index_[fi] : -1;
}

const String& Graph::GetNodeName(int node_index) const {
    static String empty;
    if(node_index < 0 || node_index >= node_names_.GetCount()) return empty;
    return node_names_[node_index];
}

bool Graph::SetEdgeName(int edge_index, const String& name) {
    if(edge_index < 0 || edge_index >= edges_.GetCount()) return false;
    String old = edges_[edge_index].name;
    if(!IsNull(old)) {
        int fi = edge_name_to_index_.Find(old);
        if(fi >= 0) edge_name_to_index_.Remove(fi);
    }
    edges_[edge_index].name = name;
    if(!IsNull(name)) {
        int fi = edge_name_to_index_.Find(name);
        if(fi >= 0) edge_name_to_index_[fi] = edge_index;
        else edge_name_to_index_.Add(name, edge_index);
    }
    return true;
}

int Graph::FindEdge(const String& name) const {
    int fi = edge_name_to_index_.Find(name);
    return fi >= 0 ? edge_name_to_index_[fi] : -1;
}

const Edge* Graph::GetEdge(int edge_index) const {
    if(edge_index < 0 || edge_index >= edges_.GetCount()) return nullptr;
    return &edges_[edge_index];
}

Edge* Graph::GetEdge(int edge_index) {
    if(edge_index < 0 || edge_index >= edges_.GetCount()) return nullptr;
    return &edges_[edge_index];
}

NAMESPACE_SAGRAPH_END

