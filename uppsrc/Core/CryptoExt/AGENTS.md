AGENTS

Scope
- Applies to `uppsrc/Core/CryptoExt`.

Purpose
- Groups lightweight cryptography helpers and verifiers historically bundled with `Core2`.
- Keeps math-backed security primitives isolated for audit and potential replacement.

Responsibilities
- Destination for `Crypto.*`, `Verifier.*`, and related helpers.
- Document algorithm choices, security assumptions, and compatibility requirements with existing data.

Guidelines
- This package must remain GUI-free and avoid hidden dependencies on ECS/Vfs internals.
- Clearly mark experimental or non-production components inside the relevant headers.
- Implementation files must include `CryptoExt.h` first in accordance with BLITZ policy.

Migration Notes
- As algorithms modernize, capture rationale and validation status in this AGENTS file.
- Coordinate with packages consuming signatures to ensure serialization formats remain compatible.
