Scope: uppsrc/ide/MCP

Purpose
- MCP server for TheIDE: exposes workspace/project/file/build APIs over a JSON-RPC 2.0 protocol over TCP (initial MVP). Designed to power OpenAI Codex via MCP with IDE-grade code intelligence and edits.

Conventions
- Follow Header Include Policy (U++ BLITZ): all .cpp/.icpp start with `#include "MCP.h"` and only add rare local includes after.
- `MCP.h` is the main header; it aggregates public headers and wraps them in `NAMESPACE_UPP`.
- Keep Protocol minimal and versioned: start with `mcp.ping` and `workspace.info` for MVP.

Integration Points
- Package is added to `uppsrc/ide/ide.upp`.
- Start/Stop APIs are provided; TheIDE can toggle server on startup or via menu later.

Split
- Headless core lives in `uppsrc/MCP`:
  - Protocol, Log, Server (`McpServerCore`) with transport/framing/capabilities and core endpoints.
- IDE integration lives here (`uppsrc/ide/MCP`):
  - `McpServer` is a thin wrapper subclassing `McpServerCore` and overriding `HandleExtended` to add IDE-only endpoints and lifecycle wiring.

File Map
- MCP.upp: manifest (lists AGENTS.md, CURRENT_TASK.md first).
- CURRENT_TASK.md: working notes and next steps.
- MCP.h / MCP.cpp: main header + start/stop entrypoints.
- Protocol.h: request/response structs and serialization helpers.
- Server.h / Server.cpp: IDE wrapper over headless `McpServerCore` with IDE-only handlers (workspace.*, node.*, edits.*) and builder-aware index status.
- WorkspaceBridge.h / WorkspaceBridge.cpp: adapters to Ide workspace queries.
- Index.h / Index.cpp: adapter over IdeMetaEnvironment/MetaEnvironment for AST-backed queries (placeholder today).
- mcp_client.sh: TCP test client that autodetects available server methods by
  grepping `Server.cpp` for `req.method == "..."` and calls each with `{}` params.
  It uses `bash`'s `/dev/tcp` to connect. Keep handler methods in `Server.cpp`
  as `if(req.method == "name")` so the client picks them up automatically.

MVP Endpoints
- mcp.ping -> { "result": "pong" }
- mcp.capabilities -> discovery: protocol, methods, flags.
- mcp.index.status -> AST/index readiness (SCRIPT builder aware).
- workspace.info -> basic info about current workspace (name, package count).

Codex Focus
- AST lives in MetaEnvironment (Core2/VfsValue.h) exposed via IdeMetaEnvironment (ide/Vfs/Ide.h).
- AST is built by ScriptBuilder (ide/Builders/ScriptBuilder.cpp) when the SCRIPT builder is selected and packages are compiled.
- MCP node.* endpoints will query this environment for symbol-safe operations (find/refs/rename/etc.).
