#include "Graphics.h"

#ifdef flagDX11

NAMESPACE_UPP

namespace {
// Thread-local D3D11 state for the active rendering context
struct D11Tls {
    Microsoft::WRL::ComPtr<ID3D11Device>            device;
    Microsoft::WRL::ComPtr<ID3D11DeviceContext>     context;
    Microsoft::WRL::ComPtr<IDXGISwapChain>          swapchain;
    Microsoft::WRL::ComPtr<ID3D11RenderTargetView>  rtv;
    Microsoft::WRL::ComPtr<ID3D11Buffer>            vb;
    Microsoft::WRL::ComPtr<ID3D11Buffer>            ib;
    UINT                                            vb_stride = 0;
    DXGI_FORMAT                                     ib_fmt = DXGI_FORMAT_R32_UINT;
    struct Program {
        Microsoft::WRL::ComPtr<ID3D11VertexShader>   vs;
        Microsoft::WRL::ComPtr<ID3D11PixelShader>    ps;
        Microsoft::WRL::ComPtr<ID3D11InputLayout>    layout;
        Microsoft::WRL::ComPtr<ID3DBlob>             vs_blob;
        Microsoft::WRL::ComPtr<ID3DBlob>             ps_blob;
    };
    VectorMap<uint32, Program> programs;
    uint32 next_prog_id = 1;
    float clearColor[4] = {0.f, 0.f, 0.f, 1.f};
    float clearDepth = 1.0f;
};

thread_local D11Tls d11_tls;
} 

// Bridge from platform/windowing code to set current D3D11 state
void D11_Internal_SetDeviceAndTargets(ID3D11Device* dev,
                                      ID3D11DeviceContext* ctx,
                                      IDXGISwapChain* sc,
                                      ID3D11RenderTargetView* rtv)
{
    d11_tls.device = dev;
    d11_tls.context = ctx;
    d11_tls.swapchain = sc;
    d11_tls.rtv = rtv;
}

// TLS for shader compile bookkeeping
thread_local String d11_last_error;

struct D11Shader {
    GVar::ShaderType type = GVar::ShaderType::SHADERTYPE_NULL;
    String code;
    Microsoft::WRL::ComPtr<ID3DBlob> blob;
};


template<class Gfx> void D11GfxT<Gfx>::BindProgramPipeline(NativePipeline& /*pipeline*/) { /* no-op for D3D11 */ }
template<class Gfx> void D11GfxT<Gfx>::UseProgram(NativeProgram& prog) {
    if (!d11_tls.context) return;
    auto idx = d11_tls.programs.Find(prog);
    if (idx < 0) return;
    auto& P = d11_tls.programs[idx];
    if (P.layout) d11_tls.context->IASetInputLayout(P.layout.Get());
    if (P.vs) d11_tls.context->VSSetShader(P.vs.Get(), nullptr, 0);
    if (P.ps) d11_tls.context->PSSetShader(P.ps.Get(), nullptr, 0);
}
//template<class Gfx> void D11GfxT<Gfx>::EnterFramebuffer(NativeFrameBuffer& fb) {TODO}
template<class Gfx> void D11GfxT<Gfx>::FramebufferTexture2D(TexType tgt, NativeColorBufferPtr b) {TODO}
template<class Gfx> void D11GfxT<Gfx>::FramebufferRenderbuffer(NativeDepthBufferPtr fb) {TODO}
template<class Gfx> void D11GfxT<Gfx>::BindFramebuffer(NativeFrameBufferPtr fb) {TODO}
template<class Gfx> void D11GfxT<Gfx>::BindFramebufferRO(NativeFrameBufferConstPtr fb) {TODO}
template<class Gfx> void D11GfxT<Gfx>::BindRenderbuffer(NativeDepthBufferPtr rb) {TODO}
template<class Gfx> void D11GfxT<Gfx>::UnbindRenderbuffer() {TODO}
template<class Gfx> void D11GfxT<Gfx>::RenderbufferStorage(Size sz) {TODO}
template<class Gfx> void D11GfxT<Gfx>::UnbindProgramPipeline() {TODO}

template<class Gfx> void D11GfxT<Gfx>::BindFramebufferDefault() {
    if (d11_tls.context && d11_tls.rtv) {
        ID3D11RenderTargetView* rt = d11_tls.rtv.Get();
        d11_tls.context->OMSetRenderTargets(1, &rt, nullptr);
    }
}

template<class Gfx> void D11GfxT<Gfx>::DrawBuffers(GVar::RenderTarget /*tgt*/) { /* TODO: MRT via OMSetRenderTargets */ }
//template<class Gfx> void D11GfxT<Gfx>::SetRender_Color() {TODO}
template<class Gfx> void D11GfxT<Gfx>::RenderScreenRect() { /* TODO: full-screen quad */ }
template<class Gfx> String D11GfxT<Gfx>::GetShaderTemplate(GVar::ShaderType t) {
    static const char* vtx_tmpl = R"HLSL(
cbuffer CB0 : register(b0) { float4 U[256]; };
// ${IS_VERTEX_SHADER}
struct VSInput { float4 iPos: POSITION; float3 iNormal: NORMAL; float2 iTexCoord: TEXCOORD0; };
struct VSOutput { float4 Position: SV_Position; float3 vPosition: TEXCOORD4; float3 vNormal: TEXCOORD5; float3 vTexCoord: TEXCOORD6; };
${USER_LIBRARY}
${USER_CODE}
VSOutput VSMain(VSInput input) {
  VSOutput o;
  o.Position = input.iPos;
  o.vPosition = input.iPos.xyz;
  o.vNormal = input.iNormal;
  o.vTexCoord = float3(input.iTexCoord, 0);
  // If user provided mainVertex(out float4 pos), call it
  // (Not invoked by default to avoid redefinition issues.)
  return o;
}
)HLSL";
    static const char* frag_tmpl = R"HLSL(
cbuffer CB0 : register(b0) { float4 U[256]; };
// ${IS_FRAGMENT_SHADER}
struct VSOutput { float4 Position: SV_Position; float3 vPosition: TEXCOORD4; float3 vNormal: TEXCOORD5; float3 vTexCoord: TEXCOORD6; };
Texture2D iChannel0 : register(t0);
Texture2D iChannel1 : register(t1);
Texture2D iChannel2 : register(t2);
Texture2D iChannel3 : register(t3);
SamplerState iSampler : register(s0);
${USER_LIBRARY}
${USER_CODE}
float4 PSMain(VSOutput input) : SV_Target {
  float4 color = float4(0.94, 0.19, 0.39, 1.0);
  // If user provided mainImage(inout float4 color, float2 fragCoord), it is not auto-invoked.
  // This default returns black.
  return color;
}
)HLSL";
    switch (t) {
        case GVar::VERTEX_SHADER:   return String(vtx_tmpl);
        case GVar::FRAGMENT_SHADER: return String(frag_tmpl);
        default: return String();
    }
}
template<class Gfx> void D11GfxT<Gfx>::HotfixShaderCode(String&) {}
template<class Gfx> void D11GfxT<Gfx>::ActiveTexture(int) {}
template<class Gfx> void D11GfxT<Gfx>::DeactivateTexture() {}
template<class Gfx> void D11GfxT<Gfx>::BindTextureRO(GVar::TextureMode, NativeColorBufferConstPtr) {}
template<class Gfx> void D11GfxT<Gfx>::BindTextureRW(GVar::TextureMode, NativeColorBufferPtr) {}
template<class Gfx> void D11GfxT<Gfx>::ReserveTexture(GVar::TextureMode, FramebufferT<Gfx>&) {}
template<class Gfx> void D11GfxT<Gfx>::SetTexture(GVar::TextureMode, Size, GVar::Sample, int, const byte*) {}
template<class Gfx> void D11GfxT<Gfx>::SetTexture(GVar::TextureMode, Size3, GVar::Sample, int, const byte*) {}
template<class Gfx> void D11GfxT<Gfx>::UnbindTexture(GVar::TextureMode) {}
template<class Gfx> void D11GfxT<Gfx>::GenerateMipmap(GVar::TextureMode) {}
template<class Gfx> bool D11GfxT<Gfx>::CreateShader(GVar::ShaderType type, NativeShaderPtr& new_shdr) {
    D11Shader* s = new D11Shader();
    s->type = type;
    new_shdr = s;
    return true;
}
template<class Gfx> void D11GfxT<Gfx>::ShaderSource(NativeShaderPtr s, String code) {
    if (!s) return;
    s->code = code;
}
template<class Gfx> bool D11GfxT<Gfx>::CompileShader(NativeShaderPtr s) {
    if (!s) return false;
    d11_last_error.Clear();
    const char* entry = s->type == GVar::VERTEX_SHADER ? "VSMain" : "PSMain";
    const char* target = s->type == GVar::VERTEX_SHADER ? "vs_5_0" : "ps_5_0";
    UINT flags = 0;
#if defined(_DEBUG)
    flags |= D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#endif
    ComPtr<ID3DBlob> blob;
    ComPtr<ID3DBlob> err;
    HRESULT hr = D3DCompile(s->code.Begin(), s->code.GetCount(), nullptr, nullptr, nullptr,
                            entry, target, flags, 0, blob.GetAddressOf(), err.GetAddressOf());
    if (FAILED(hr)) {
        if (err) d11_last_error = String((const char*)err->GetBufferPointer(), (int)err->GetBufferSize());
        s->blob.Reset();
        return false;
    }
    s->blob = blob;
    return true;
}
template<class Gfx> String D11GfxT<Gfx>::GetLastErrorS(NativeShaderPtr) { return d11_last_error; }
template<class Gfx> String D11GfxT<Gfx>::GetLastErrorP(NativeProgram&) { return d11_last_error; }
template<class Gfx> bool D11GfxT<Gfx>::CreateProgram(NativeProgram& prog) { prog = 1; return true; }
// Overload for DX11 to allocate program handle
template<> bool D11GfxT<WinD11Gfx>::CreateProgram(NativeProgram& prog) {
    prog = d11_tls.next_prog_id++;
    d11_tls.programs.Add(prog, D11Tls::Program());
    return true;
}
template<class Gfx> bool D11GfxT<Gfx>::CreateRenderbuffer(NativeDepthBufferPtr&) { return true; }
template<class Gfx> void D11GfxT<Gfx>::ProgramParameteri(NativeProgram&, GVar::ParamType, int) {}
template<class Gfx> void D11GfxT<Gfx>::AttachShader(NativeProgram& prog, NativeShaderPtr shdr) {
    if (!shdr) return;
    // Try to reflect the blob to detect stage
    if (!shdr->blob) return;
    ComPtr<ID3D11ShaderReflection> refl;
    if (SUCCEEDED(D3DReflect(shdr->blob->GetBufferPointer(), shdr->blob->GetBufferSize(), IID_ID3D11ShaderReflection, (void**)refl.GetAddressOf()))) {
        D3D11_SHADER_DESC desc{};
        refl->GetDesc(&desc);
        UINT ver = desc.Version;
        D3D11_SHADER_VERSION_TYPE stype = (D3D11_SHADER_VERSION_TYPE)((ver >> 16) & 0xF);
        auto idx = d11_tls.programs.Find(prog);
        if (idx < 0) return;
        if (stype == D3D11_SHVER_VERTEX_SHADER) d11_tls.programs[idx].vs_blob = shdr->blob;
        else if (stype == D3D11_SHVER_PIXEL_SHADER) d11_tls.programs[idx].ps_blob = shdr->blob;
    }
}
template<class Gfx> void D11GfxT<Gfx>::DeleteShader(NativeShaderPtr& shdr) { if (shdr) { delete shdr; shdr = nullptr; } }
template<class Gfx> bool D11GfxT<Gfx>::LinkProgram(NativeProgram& prog) {
    if (!d11_tls.device) { d11_last_error = "D3D11 device not set"; return false; }
    HRESULT hr = S_OK;
    auto idx = d11_tls.programs.Find(prog);
    if (idx < 0) { d11_last_error = "Invalid program handle"; return false; }
    auto& P = d11_tls.programs[idx];
    if (P.vs_blob)
        hr = d11_tls.device->CreateVertexShader(P.vs_blob->GetBufferPointer(), P.vs_blob->GetBufferSize(), nullptr, P.vs.GetAddressOf());
    if (FAILED(hr)) { d11_last_error = "CreateVertexShader failed"; return false; }
    if (P.ps_blob)
        hr = d11_tls.device->CreatePixelShader(P.ps_blob->GetBufferPointer(), P.ps_blob->GetBufferSize(), nullptr, P.ps.GetAddressOf());
    if (FAILED(hr)) { d11_last_error = "CreatePixelShader failed"; return false; }
    // Default input layout matching VSInput
    if (P.vs_blob) {
        D3D11_INPUT_ELEMENT_DESC il[] = {
            {"POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 0,                           D3D11_INPUT_PER_VERTEX_DATA, 0},
            {"NORMAL",   0, DXGI_FORMAT_R32G32B32_FLOAT,    0, sizeof(float)*4,             D3D11_INPUT_PER_VERTEX_DATA, 0},
            {"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT,       0, sizeof(float)*(4+3),         D3D11_INPUT_PER_VERTEX_DATA, 0},
        };
        hr = d11_tls.device->CreateInputLayout(il, (UINT)(sizeof(il)/sizeof(il[0])), P.vs_blob->GetBufferPointer(), P.vs_blob->GetBufferSize(), P.layout.GetAddressOf());
        if (FAILED(hr)) { d11_last_error = "CreateInputLayout failed"; return false; }
    }
    return true;
}
template<class Gfx> void D11GfxT<Gfx>::GetProgramiv(NativeProgram&, GVar::ProgParamType type, int& out) {
    // Pretend all known uniform variables exist; ProgramState will map indices accordingly
    if (type == GVar::ACTIVE_UNIFORMS) out = GVar::VAR_COUNT; else out = 0;
}
template<class Gfx> String D11GfxT<Gfx>::GetActiveUniform(NativeProgram&, int i, int* size_out, int* type_out) {
    if (i >= 0 && i < GVar::VAR_COUNT) {
        if (size_out) *size_out = 1;
        if (type_out) *type_out = 0;
        return String(GVar::gvars[i].name);
    }
    if (size_out) *size_out = 0; if (type_out) *type_out = 0; return String();
}
template<class Gfx> void D11GfxT<Gfx>::Clear(GVar::BufferType type) {
    if (!d11_tls.context) return;
    if (type == GVar::COLOR_BUFFER && d11_tls.rtv)
        d11_tls.context->ClearRenderTargetView(d11_tls.rtv.Get(), d11_tls.clearColor);
    // Depth/stencil can be added when DSV is wired
}

template<class Gfx> void D11GfxT<Gfx>::GenProgramPipeline(NativePipeline& pipe) {
	// pass
	// Pipeline is tied to the ID3D11DeviceContext, which has been created in the atom
}

template<class Gfx> void D11GfxT<Gfx>::UseProgramStages(NativePipeline&, uint32, NativeProgram& prog) {
    if (!d11_tls.context) return;
    auto idx = d11_tls.programs.Find(prog);
    if (idx < 0) return;
    auto& P = d11_tls.programs[idx];
    if (P.layout) d11_tls.context->IASetInputLayout(P.layout.Get());
    if (P.vs) d11_tls.context->VSSetShader(P.vs.Get(), nullptr, 0);
    if (P.ps) d11_tls.context->PSSetShader(P.ps.Get(), nullptr, 0);
}

static void D11_EnsureCBuffer() {
    if (!d11_tls.device) return;
    if (!d11_tls.cbuffer) {
        d11_tls.udata.SetCount(256*4, 0.0f);
        D3D11_BUFFER_DESC bd{};
        bd.ByteWidth = UINT(d11_tls.udata.GetCount() * sizeof(float));
        bd.Usage = D3D11_USAGE_DEFAULT;
        bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
        bd.CPUAccessFlags = 0;
        bd.MiscFlags = 0;
        bd.StructureByteStride = 0;
        d11_tls.device->CreateBuffer(&bd, nullptr, d11_tls.cbuffer.GetAddressOf());
    }
}

static void D11_UpdateCBuffer() {
    if (!d11_tls.context || !d11_tls.cbuffer)
        return;
    d11_tls.context->UpdateSubresource(d11_tls.cbuffer.Get(), 0, nullptr, d11_tls.udata.Begin(), 0, 0);
    ID3D11Buffer* cb = d11_tls.cbuffer.Get();
    d11_tls.context->VSSetConstantBuffers(0, 1, &cb);
    d11_tls.context->PSSetConstantBuffers(0, 1, &cb);
}
template<class Gfx> void D11GfxT<Gfx>::DeleteProgramPipeline(NativePipeline&) {}
template<class Gfx> void D11GfxT<Gfx>::TexParameteri(GVar::TextureMode, GVar::Filter, GVar::Wrap) {}
template<class Gfx> bool D11GfxT<Gfx>::GenTexture(NativeColorBufferPtr&) { return true; }
template<class Gfx> bool D11GfxT<Gfx>::CreateFramebuffer(NativeFrameBufferPtr&) { return true; }
template<class Gfx> void D11GfxT<Gfx>::GenVertexArray(NativeVertexArray& vao) { vao = 1; }
template<class Gfx> void D11GfxT<Gfx>::GenVertexBuffer(NativeVertexBuffer& vbo) { vbo = 1; }
template<class Gfx> void D11GfxT<Gfx>::GenElementBuffer(NativeElementBuffer& ebo) { ebo = 1; }
template<class Gfx> void D11GfxT<Gfx>::BindVertexArray(NativeVertexArray&) {}
template<class Gfx> void D11GfxT<Gfx>::BindVertexBuffer(NativeVertexBuffer&) {}
template<class Gfx> void D11GfxT<Gfx>::BindElementBuffer(NativeElementBuffer&) {}
template<class Gfx> void D11GfxT<Gfx>::DeleteVertexArray(NativeVertexArray&) {}
template<class Gfx> void D11GfxT<Gfx>::DeleteVertexBuffer(NativeVertexBuffer&) {}
template<class Gfx> void D11GfxT<Gfx>::DeleteElementBuffer(NativeElementBuffer&) {}
template<class Gfx> void D11GfxT<Gfx>::VertexBufferData(const Vector<Vertex>& vtx) {
    if (!d11_tls.device) return;
    D3D11_BUFFER_DESC bd{};
    bd.ByteWidth = (UINT)(vtx.GetCount() * sizeof(Vertex));
    bd.Usage = D3D11_USAGE_DEFAULT;
    bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    D3D11_SUBRESOURCE_DATA init{};
    init.pSysMem = vtx.Begin();
    d11_tls.vb.Reset();
    d11_tls.device->CreateBuffer(&bd, &init, d11_tls.vb.GetAddressOf());
    d11_tls.vb_stride = sizeof(Vertex);
}
template<class Gfx> void D11GfxT<Gfx>::ElementBufferData(const Vector<uint32>& indices) {
    if (!d11_tls.device) return;
    D3D11_BUFFER_DESC bd{};
    bd.ByteWidth = (UINT)(indices.GetCount() * sizeof(uint32));
    bd.Usage = D3D11_USAGE_DEFAULT;
    bd.BindFlags = D3D11_BIND_INDEX_BUFFER;
    D3D11_SUBRESOURCE_DATA init{};
    init.pSysMem = indices.Begin();
    d11_tls.ib.Reset();
    d11_tls.device->CreateBuffer(&bd, &init, d11_tls.ib.GetAddressOf());
    d11_tls.ib_fmt = DXGI_FORMAT_R32_UINT;
}
template<class Gfx> void D11GfxT<Gfx>::SetupVertexStructure() {}
template<class Gfx> void D11GfxT<Gfx>::UnbindVertexArray() {}
template<class Gfx> void D11GfxT<Gfx>::UnbindVertexBuffer() {}
template<class Gfx> void D11GfxT<Gfx>::UnbindElementBuffer() {}
template<class Gfx> void D11GfxT<Gfx>::UnbindFramebuffer() {}
template<class Gfx> void D11GfxT<Gfx>::ActivateVertexStructure() {
    if (!d11_tls.context) return;
    UINT stride = d11_tls.vb_stride;
    UINT offset = 0;
    ID3D11Buffer* vb = d11_tls.vb.Get();
    if (vb)
        d11_tls.context->IASetVertexBuffers(0, 1, &vb, &stride, &offset);
    if (d11_tls.ib)
        d11_tls.context->IASetIndexBuffer(d11_tls.ib.Get(), d11_tls.ib_fmt, 0);
    d11_tls.context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
}
template<class Gfx> void D11GfxT<Gfx>::DeactivateVertexStructure() {}
template<class Gfx> void D11GfxT<Gfx>::DrawVertexElements(int element_limit, bool use_quad) {
    if (!d11_tls.context) return;
    if (use_quad) return; // not supported similar to GL path
    d11_tls.context->DrawIndexed((UINT)element_limit, 0, 0);
}
template<class Gfx> void D11GfxT<Gfx>::TexImage2D(ByteImage&) {}
template<class Gfx> void D11GfxT<Gfx>::TexImage2D(FloatImage&) {}
template<class Gfx> void D11GfxT<Gfx>::DeleteTexture(NativeColorBufferPtr& b) { b.Reset(); }
template<class Gfx> void D11GfxT<Gfx>::DeleteRenderbuffer(NativeDepthBufferPtr& b) { b.Reset(); }
template<class Gfx> void D11GfxT<Gfx>::DeleteFramebuffer(NativeFrameBufferPtr& b) { b.Reset(); }

template<class Gfx> void D11GfxT<Gfx>::SetContextDefaultFramebuffer(NativeFrameBufferPtr fb) {
    d11_tls.swapchain = fb;
}

template<class Gfx> void D11GfxT<Gfx>::BeginRender() {}
template<class Gfx> void D11GfxT<Gfx>::EndRender() {}
template<class Gfx> void D11GfxT<Gfx>::BeginRenderObject() {}
template<class Gfx> void D11GfxT<Gfx>::EndRenderObject() {}
//template<class Gfx> NativeColorBufferConstPtr GetFrameBufferColor(NativeFrameBufferConstPtr fb, TexType t) {TODO}
template<class Gfx> void D11GfxT<Gfx>::Uniform1i(int idx, int i) {
    D11_EnsureCBuffer(); if (idx < 0) return; if (idx >= 256) idx = 255;
    int base = idx*4; if (d11_tls.udata.GetCount() < (base+4)) return;
    d11_tls.udata[base+0] = (float)i; D11_UpdateCBuffer();
}
template<class Gfx> void D11GfxT<Gfx>::Uniform2i(int idx, int i0, int i1) {
    D11_EnsureCBuffer(); if (idx < 0) return; if (idx >= 256) idx = 255; int base = idx*4; if (d11_tls.udata.GetCount() < (base+4)) return;
    d11_tls.udata[base+0] = (float)i0; d11_tls.udata[base+1] = (float)i1; D11_UpdateCBuffer();
}
template<class Gfx> void D11GfxT<Gfx>::Uniform3i(int idx, int i0, int i1, int i2) {
    D11_EnsureCBuffer(); if (idx < 0) return; if (idx >= 256) idx = 255; int base = idx*4; if (d11_tls.udata.GetCount() < (base+4)) return;
    d11_tls.udata[base+0] = (float)i0; d11_tls.udata[base+1] = (float)i1; d11_tls.udata[base+2] = (float)i2; D11_UpdateCBuffer();
}
template<class Gfx> void D11GfxT<Gfx>::Uniform1f(int idx, float f) {
    D11_EnsureCBuffer(); if (idx < 0) return; if (idx >= 256) idx = 255; int base = idx*4; if (d11_tls.udata.GetCount() < (base+4)) return;
    d11_tls.udata[base+0] = f; D11_UpdateCBuffer();
}
template<class Gfx> void D11GfxT<Gfx>::Uniform2f(int idx, float f0, float f1) {
    D11_EnsureCBuffer(); if (idx < 0) return; if (idx >= 256) idx = 255; int base = idx*4; if (d11_tls.udata.GetCount() < (base+4)) return;
    d11_tls.udata[base+0] = f0; d11_tls.udata[base+1] = f1; D11_UpdateCBuffer();
}
template<class Gfx> void D11GfxT<Gfx>::Uniform3f(int idx, float f0, float f1, float f2) {
    D11_EnsureCBuffer(); if (idx < 0) return; if (idx >= 256) idx = 255; int base = idx*4; if (d11_tls.udata.GetCount() < (base+4)) return;
    d11_tls.udata[base+0] = f0; d11_tls.udata[base+1] = f1; d11_tls.udata[base+2] = f2; D11_UpdateCBuffer();
}
template<class Gfx> void D11GfxT<Gfx>::Uniform4f(int idx, float f0, float f1, float f2, float f3) {
    D11_EnsureCBuffer(); if (idx < 0) return; if (idx >= 256) idx = 255; int base = idx*4; if (d11_tls.udata.GetCount() < (base+4)) return;
    d11_tls.udata[base+0] = f0; d11_tls.udata[base+1] = f1; d11_tls.udata[base+2] = f2; d11_tls.udata[base+3] = f3; D11_UpdateCBuffer();
}
template<class Gfx> void D11GfxT<Gfx>::ProgramUniform3f(NativeProgram&, int idx, float f0, float f1, float f2) { Uniform3f(idx, f0, f1, f2); }
template<class Gfx> void D11GfxT<Gfx>::Uniform1fv(int idx, int count, float* f) {
    D11_EnsureCBuffer(); if (idx < 0) return; for (int k=0;k<count;k++){int base = (idx+k)*4; if (base+1 <= d11_tls.udata.GetCount()) d11_tls.udata[base+0]=f[k];}
    D11_UpdateCBuffer();
}
template<class Gfx> void D11GfxT<Gfx>::Uniform3fv(int idx, int count, float* f) {
    D11_EnsureCBuffer(); if (idx < 0) return; for (int k=0;k<count;k++){int base = (idx+k)*4; if (base+3 <= d11_tls.udata.GetCount()){ d11_tls.udata[base+0]=f[k*3+0]; d11_tls.udata[base+1]=f[k*3+1]; d11_tls.udata[base+2]=f[k*3+2]; }}
    D11_UpdateCBuffer();
}
template<class Gfx> void D11GfxT<Gfx>::Uniform4fv(int idx, int count, float* f) {
    D11_EnsureCBuffer(); if (idx < 0) return; for (int k=0;k<count;k++){int base = (idx+k)*4; if (base+4 <= d11_tls.udata.GetCount()){ d11_tls.udata[base+0]=f[k*4+0]; d11_tls.udata[base+1]=f[k*4+1]; d11_tls.udata[base+2]=f[k*4+2]; d11_tls.udata[base+3]=f[k*4+3]; }}
    D11_UpdateCBuffer();
}
template<class Gfx> void D11GfxT<Gfx>::UniformMatrix4fv(int idx, const mat4& mat) {
    D11_EnsureCBuffer(); if (idx < 0) return; // write 4 float4 rows: assuming mat[row][col]
    for (int r=0;r<4;r++) {
        int base = (idx + r)*4; if (base+4 <= d11_tls.udata.GetCount()) {
            d11_tls.udata[base+0] = ((float*)&mat[0][0])[r*4+0];
            d11_tls.udata[base+1] = ((float*)&mat[0][0])[r*4+1];
            d11_tls.udata[base+2] = ((float*)&mat[0][0])[r*4+2];
            d11_tls.udata[base+3] = ((float*)&mat[0][0])[r*4+3];
        }
    }
    D11_UpdateCBuffer();
}
template<class Gfx> void D11GfxT<Gfx>::ReadPixels(int, int, int, int, GVar::Sample, int, byte*) {}
template<class Gfx> void D11GfxT<Gfx>::ClearBuffers() {
    if (d11_tls.context && d11_tls.rtv)
        d11_tls.context->ClearRenderTargetView(d11_tls.rtv.Get(), d11_tls.clearColor);
}
template<class Gfx> void D11GfxT<Gfx>::SetSmoothShading(bool) {}
template<class Gfx> void D11GfxT<Gfx>::SetDepthTest(bool) {}
template<class Gfx> void D11GfxT<Gfx>::SetDepthOrderLess(bool) {}
template<class Gfx> void D11GfxT<Gfx>::SetClearValue(RGBA clr, byte depth) {
    d11_tls.clearColor[0] = clr.r / 255.0f;
    d11_tls.clearColor[1] = clr.g / 255.0f;
    d11_tls.clearColor[2] = clr.b / 255.0f;
    d11_tls.clearColor[3] = clr.a / 255.0f;
    d11_tls.clearDepth = depth / 255.0f;
}
template<class Gfx> void D11GfxT<Gfx>::SetFastPerspectiveCorrection(bool) {}
template<class Gfx> void D11GfxT<Gfx>::SetTriangleBacksideCulling(bool) {}
template<class Gfx> void D11GfxT<Gfx>::SetTriangleFrontsideCCW(bool) {}
template<class Gfx> void D11GfxT<Gfx>::SetViewport(Size sz) {
    if (!d11_tls.context) return;
    D3D11_VIEWPORT vp{};
    vp.TopLeftX = 0.0f;
    vp.TopLeftY = 0.0f;
    vp.Width  = (FLOAT)max(1, sz.cx);
    vp.Height = (FLOAT)max(1, sz.cy);
    vp.MinDepth = 0.0f;
    vp.MaxDepth = 1.0f;
    d11_tls.context->RSSetViewports(1, &vp);
}
//template<class Gfx> void D11GfxT<Gfx>::ActivateNextFrame() {TODO}

template<class Gfx> void D11GfxT<Gfx>::SetDebugOutput(bool b) {
	// unfortunately not supported
}

template<class Gfx> void D11GfxT<Gfx>::ClearFramebufferPtr(NativeFrameBufferPtr& fb) {
	fb.Reset();
}

template<class Gfx> void D11GfxT<Gfx>::ClearColorBufferPtr(NativeColorBufferPtr& b) {
	b.Reset();
}

template<class Gfx> void D11GfxT<Gfx>::ClearDepthBufferPtr(NativeDepthBufferPtr& b) {
	b.Reset();
}

template<class Gfx> Serial::FboFormat& D11GfxT<Gfx>::GetFormat(ValueFormat& fmt) {return fmt;}

#if defined flagWIN32 && defined flagDX11
template struct D11GfxT<WinD11Gfx>;
#endif


void WinD11Gfx::ActivateNextFrame(NativeDisplay& , NativeWindow& , NativeRenderer& , NativeColorBufferPtr ) {
	if (d11_tls.swapchain)
		d11_tls.swapchain->Present(1, 0);
}

Size DxGfx::GetWindowSize(NativeWindow& win) { RECT rc{}; GetClientRect(win, &rc); return Size(rc.right-rc.left, rc.bottom-rc.top); }
bool DxGfx::CreateWindowAndRenderer(Size, dword, NativeWindow& , NativeRenderer& ) {return false;}

void DxGfx::SetTitle(NativeDisplay& display, NativeWindow& win, String title) {
	SetWindowTextA(win, title.Begin());
}

void DxGfx::SetWindowFullscreen(NativeWindow& , bool ) {}
void DxGfx::DestroyRenderer(NativeRenderer& ) {}

void DxGfx::ClearRendererPtr(NativeRenderer& rend) {
	rend.Reset();
}

void DxGfx::DestroyWindow(NativeWindow& ) {}
void DxGfx::DeleteContext(NativeGLContext& ) {}
void DxGfx::MaximizeWindow(NativeWindow& win) { ShowWindow(win, SW_MAXIMIZE); }
void DxGfx::RestoreWindow(NativeWindow& win) { ShowWindow(win, SW_RESTORE); }
void DxGfx::SetWindowPosition(NativeWindow& win, Point pt) { SetWindowPos(win, 0, pt.x, pt.y, 0, 0, SWP_NOSIZE|SWP_NOZORDER); }
void DxGfx::SetWindowSize(NativeWindow& win, Size sz) { SetWindowPos(win, 0, 0, 0, sz.cx, sz.cy, SWP_NOMOVE|SWP_NOZORDER); }


END_UPP_NAMESPACE

#endif
