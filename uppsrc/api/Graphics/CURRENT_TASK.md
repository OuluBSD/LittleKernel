Graphics: D3D11 Backend Bring-up — State, Gaps, Options

Context
- Package implements Software (SoftRend) and OpenGL backends. A Direct3D 11 backend (flagDX11, WinD11) is being implemented.
- Windowing for DX11 is provided by Screen/WinD11.cpp (DeviceResources style). Graphics/Dx.cpp provides the rendering API used by the engine (BufferStage/ProgramState/etc.).

Current State (D3D11)
- Device bridge: After creating swapchain and backbuffer RTV, Screen/WinD11 calls D11_Internal_SetDeviceAndTargets(device, context, swapchain, rtv) to expose objects to the Gfx layer.
- Frame setup: BindFramebufferDefault (OMSetRenderTargets), SetViewport, SetClearValue, Clear(COLOR_BUFFER), Present.
- HLSL support:
  - Minimal VS/PS templates compiled with D3DCompile (vs_5_0 / ps_5_0).
  - Program handle (uint32) maps to VS/PS/InputLayout + blobs (TLS map). AttachShader reflects blob to detect stage; LinkProgram creates VS/PS and InputLayout.
  - Default PS returns a visible color (magenta) for bring-up.
- Uniforms (constant buffer):
  - A single CB0 (b0) with float4 U[256]. Uniform* setters write into U and update/bind the CB for both VS and PS.
  - GetProgramiv/GetActiveUniform provide a stable name mapping using GVar::gvars; ProgramState finds indices by name; setters then use those indices.
- Geometry:
  - Vertex/index buffer creation (DEFAULT usage), binding via IASetVertexBuffers/IndexBuffer; triangle list topology. DrawIndexed used in DrawVertexElements.
- Pipeline program/pipeline binding: BindProgramPipeline is a no-op (context-driven), UseProgram/UseProgramStages binds input layout and shaders.

Important Details & Assumptions
- Vertex layout assumed as: POSITION float4, NORMAL float3, TEXCOORD float2. InputLayout created accordingly. This must match sizeof(Vertex) and member ordering used by Mesh.
- Uniform index mapping: DX11 path pretends every known uniform exists and exposes names in GVar order. ProgramState looks up indices by name; CB writes use those indices. This is a pragmatic bridge until full reflection/mapping is added.
- Many API methods remain stubs (textures, depth/stencil, MRTs, some window ops in DxGfx not used by WinD11 Screen path).
- Debug: SetDebugOutput is currently a stub; consider enabling the D3D11 debug layer + info queue in debug builds.

Gaps / Backlog
1) Textures + Samplers
   - Implement GenTexture, TexImage2D/Float, SetTexture (2D/3D/cube), BindTextureRO/RW, TexParameteri (Filter/Wrap mapping), GenerateMipmap (D3D11GenerateMips), SRV and Sampler creation. Bind via PSSetShaderResources/PSSetSamplers.

2) Depth/Stencil and States
   - Create depth-stencil texture + DSV for swapchain target and offscreen targets. Wire Clear(DEPTH_BUFFER), SetDepthTest/SetDepthOrderLess. Add rasterizer state for culling and front-face winding.

3) MRTs / Framebuffer Equivalents
   - Map DrawBuffers (GVar::RenderTarget mask) to OMSetRenderTargets with multiple RTVs. Manage offscreen render targets and their lifetime.

4) Shader Uniform Parity
   - Option A: Add HLSL macros/accessors mapping common names (iTime, iResolution, iView, iModel, iScale, iCameraPos/Dir, iLightDir, etc.) to indices in U[].
   - Option B: Switch to a structured cbuffer with explicit fields and update ProgramState to map names to byte offsets, while maintaining GL parity.
   - Option C: Hybrid: keep U[] backing but generate a small name→index map accessible to HLSL via #defines.

5) Fullscreen Quad / RenderScreenRect
   - Implement a dedicated full-screen triangle/quad and the RenderScreenRect path.

6) Error Handling & Diagnostics
   - Extend error propagation and logging for D3D calls; optionally enable DXGI/D3D debug layers in debug builds.

Build / Linking
- Screen.upp links: "D3D11 D3DCompiler dxgi". Dx.cpp includes d3dcompiler.h.
- Graphics.upp lists Dx.cpp and dxstdafx.h; this CURRENT_TASK.md is added to the package file list for visibility.

Next Options (pick order)
- A) Uniform name parity: add HLSL accessors for core uniforms; then migrate to structured CB if needed.
- B) Implement textures/samplers end-to-end.
- C) Implement depth-stencil creation and states (depth test/orderless, culling, winding).
- D) MRTs and offscreen framebuffer management for multi-stage pipelines.

Current Known Working Path
- Window/display via WinD11 Screen; frame clear + viewport + program bind + VBO/IBO draw + present; uniforms updated through CB0.

Template/Design Issues To Fix
- DX11 HLSL templates don’t call user entry-points:
  - OGL path integrates mainVertex/mainImage convention; DX includes ${USER_CODE} but never calls it. Add guarded calls so user code runs.
- No named uniforms in HLSL template:
  - Expose macros for iTime, iResolution, iFrame, iView/iProjection/iModel/iScale, iCameraPos/Dir, iLightDir, etc., mapping to CB0 U[] indices consistent with GVar ordering.
- No transforms applied in VS:
  - VSMain should compute Position using iProjection * iView * iModel (with clear row/column-major policy). Currently passthrough.
- Matrix layout mismatch risk:
  - UniformMatrix4fv writes four consecutive float4s. Ensure HLSL uses matching row/column-major or transpose data on upload.
- Input layout offset assumptions:
  - Replace hardcoded offsets with offsetof(Vertex, position/normal/tex_coord) (mirrors OGL usage) to avoid struct layout drift.
- Inefficient uniform updates:
  - Multiple UpdateSubresource calls per draw. Batch uniform writes and commit CB once per program/object.
