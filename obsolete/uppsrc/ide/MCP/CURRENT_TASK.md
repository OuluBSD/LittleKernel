MCP for OpenAI Codex — Vision & Plan

Context
- This package implements an MCP server embedded in TheIDE, specifically to power OpenAI Codex (via MCP) with precise, IDE-backed code intelligence and edits.
- Transport is TCP for now; protocol is JSON-RPC-like over framed messages. We will evolve to full JSON‑RPC 2.0 compliance.

Core Model
- Workspace Graph
  - Nodes: files, translation units, declarations (classes, functions, variables, enums), definitions, references, comments.
  - Edges: defines, declares, references, includes, contains, overrides, implements.
  - Identity: stable node IDs derived from VfsValue path + AST location (file:offset/extent + symbol USR-like key). Avoid pure text spans.
- Vfs + AST
  - Parse sources into VfsValue → AST tree with symbol table and cross-refs.
  - Maintain a workspace-wide index with incremental updates on file changes.
  - Support multiple variants/configs (defines, includes) as feasible; surface active config.

Protocol Shape
- Resource Types
  - node: { id, kind, name, usr, file, range, parents, children, resolved_target? }
  - location: { file, start:{line,col}, end:{line,col} }
  - edit: structured refactor (rename/insert/delete/replace) or text hunks; always validate first.
  - result sets support paging with cursors for large outputs.
- Target Addressing
  - Accept node `id` or a `locator` { file, position, hint? } to resolve to a node.
  - For rename/edits, prefer symbol identity (USR) over text.

Read-only Features
- node.locate → resolve {file,position|query} to node (+disambiguation list).
- node.get → node metadata + pretty-printed C++ text and span.
- node.references → refs grouped by role (read/write/call/inherit), paged.
- node.definition / node.implementation → definition/impl locations.

Edit Features
- node.add → create declaration/definition under parent or at file anchor.
- node.implement → create function/method definition stub at the right TU; add include if needed.
- node.rename → symbol-safe rename across refs/decl/def; dry‑run (validate_only) returns patch plan and conflicts.
- node.delete → remove declaration/definition with optional cascade.
- comment.add → insert formatted comment (line/block/doxygen) relative to node.
- edits.apply → apply multi-file edits atomically via IDE buffers (undo-friendly).

Indexing & Performance
- Background indexer keeps a symbol map and xrefs; updates incrementally.
- Large queries return paged results; enforce caps and timeouts.
- Cache symbol→refs; invalidate on edit/reparse.

Rename Quality & Safety
- Operate on symbol identity (USR), not text.
- Handle overload sets explicitly; avoid unintended macro/template renames.
- Dry‑run returns: edits, conflicts, required includes; client applies via edits.apply.

Discovery & Ergonomics
- mcp.capabilities → protocol version, supported namespaces/methods, flags (batch, notifications).
- mcp.index.status → index phase/progress/stale files.
- Optional future mcp.events for push updates.

Security & Boundaries
- Restrict writes to workspace; size caps on reads/writes.
- Cancellation tokens for long ops; per-request timeouts.
- Apply edits transactionally through IDE buffer API.

Transport & Framing
- Stabilize message framing with per‑client buffers (newline‑delimited or length‑prefixed) to handle partial/merged reads.
- Maintain per-client state: inbuf/outbuf/last_activity; prune stale clients.

Immediate Roadmap (sequenced)
1) Protocol + discovery
   - Add JSON‑RPC 2.0 compliance (validate jsonrpc=="2.0"; standard errors -32700, -32600, -32601, -32602, -32603).
   - Implement mcp.capabilities and standardize error responses; echo request ids.
2) Index core
   - Integrate existing MetaEnvironment AST via IdeMetaEnvironment (ide/Vfs/Ide.h) which wraps Core2/VfsValue.h.
   - Use SCRIPT builder (ide/Builders/ScriptBuilder.cpp) to populate/update AST; require active SCRIPT builder for freshness.
   - Provide EnvIndex adapter to query env: LocateByPos, GetById, GetDefinition, FindReferences; mint stable node ids (USR-like).
3) Read-only queries
   - node.locate, node.get, node.definition, node.references with paging.
4) Edit pipeline
   - edits.apply (atomic, undoable). node.rename in validate_only mode to produce a patch set.
5) Implement/Generate
   - node.implement for methods/functions; node.add skeletons.

Server Hardening
- Replace Array<TcpSocket> with Array<Client> (socket + buffers + id + timestamps).
- Use RW locks with scoped guards; avoid re-entrancy on clients container.
- Add max message size, socket timeouts, and parse error backoff before disconnect.
- Introduce MCPLOG(req_id, method, msg) macro; log durations; gate verbose logs behind IDE option.

Done
- Package skeleton, AGENTS.md.
- Lifecycle wiring into TheIDE (start on init, stop on shutdown; config flag/port).
 - Per-client framing (newline-delimited), capabilities endpoint, JSON-RPC error semantics.

Next (actionable)
- Add mcp.index.status and mcp.index.refresh (SCRIPT builder aware) endpoints.
- Implement EnvIndex adapter backed by IdeMetaEnvironment; return index_not_ready when AST missing/stale.
- Wire node.locate/node.get/node.definition/node.references to EnvIndex.
- Update mcp_client.sh to use mcp.capabilities for discovery (done); extend with sample node.locate request.
- Define required Env APIs in repo root CURRENT_TASK.md for Core2/VfsValue.h and ide/Vfs/Ide.h:
   - EnvLocate(file,line,col), EnvGet(id), EnvDefinition(id), EnvReferences(id,page,limit), EnvCode(id|range), EnvStatus().
   - Add header stubs with TODOs so MCP can link now.
