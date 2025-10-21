AGENTS

Scope
- Applies to `stdsrc/*` (the STL-backed standard library layer) and its nested packages.
- Primary focus today is `stdsrc/Core`: a U++ Core-compatible API surface implemented with C++ standard library (and small shims) to make AI-assisted development portable and predictable.

Why stdsrc exists
- Allow agents to build and test logic without system-specific U++ internals while preserving familiar U++ names/APIs (e.g., `Upp::String`, `Upp::Vector`, `Format`, `Path`).
- Keep the header/include discipline and package manifests so code still integrates with TheIDE and larger U++ assemblies.

Quick Start
- Include the aggregator in any implementation or test: `#include "stdsrc/Core/Core.h"`.
- Prefer APIs under `namespace Upp` (`using namespace Upp;`) — the aggregator defines `NAMESPACE_UPP` and wraps public headers.
- Build tests under `stdtst/*` using simple `.upp` manifests that `uses Core` (stdsrc Core), then add a single `main` file.

Package Map (Core)
- Aggregator
  - `Core.h`: wraps all headers and centralizes system includes/macros.

- Core types
  - `Defs.h`: base typedefs (`byte`, `dword`, `qword`, `int16`, `int64`, `wchar`, `char16`), `Nuller`, and helpers (`Moveable` markers, `pick`, `ASSERT`).
  - `String.h`, `WString.h`: U++-compatible `String` and `WString` on top of `std::string`/`std::wstring`. UTF-8/wide conversions use platform facilities (see Platform Notes).
  - `Vcont.h`: `Vector<T>` shim backed by `std::vector<T>`.
  - `Array.h`: `Array<T>` that owns elements via pointers (`std::unique_ptr<T>`) to keep element addresses stable across reallocation and neighbor insert/erase.
  - `Index.h`: unique key container on `std::vector<T> + std::map<T,int>` (source-compatible subset).
  - `Map.h`: `VectorMap<K,T>` with order preserved (`std::vector<pair>` + `std::map<K,int>` index).
  - `ArrayMap.h`: `ArrayMap<K,T>` with ordered keys and owning pointer values for stable addresses; uses `std::map<K,int>` index.
  - `One.h`: unique ownership over `std::unique_ptr`.
  - `Ptr.h`: non-owning `Ptr<T>` wrapper (no reference counting).
  - `Hash.h`: `hash_t`, `CombineHash`, `GetHashValue` for common types.
  - `Uuid.h`: v4 UUID create/parse + `ToString()`.
  - `Value.h`: lightweight `std::any`-backed variant (basic conversions only).

- Utility
  - `Util.h`: `AsString`, `FormatPtr`.
  - `Convert.h`: scanning/formatting helpers and `ConvertInt/Double/Date/Time` shims.
  - `Format.h`: U++-style `Format` implementation (see below). Supports `%` positional/width/flags, keywords, `%m` semantics and backtick suffix.
  - `Base64.h`: Base64 encode/decode helpers and Stream adapters.
  - `Algo.h`: small helpers (`Min/Max/Clamp/sgn/InRange`).
  - `Sort.h`: STL-backed `Sort/StableSort/LowerBound/UpperBound/BinarySearch`.
  - `Tuple.h`: aliases to `std::tuple` (`Tuple`, `MakeTuple`, `Get<N>`).
  - `Complex.h`: aliases to `std::complex` with `Real/Imag/Abs`.
  - `One.h`: unique ownership wrapper with `Attach/Detach/Release/Create/Make`, `Swap`, `IsEmpty`.
  - `Ptr.h`: non-owning pointer wrapper with `Clear/IsEmpty/Get`, comparisons and `ToString()`.
  - `Profile.h`: `RTIMING`, `RHITCOUNT`, `TimeStop` stopwatch (lightweight shims).
  - `Log.h`: logging functions + `LOG` macro. Quick setup via `StdLogSetup(flags, path, append)` with flags `LOG_FILE` and `LOG_COUT`.

- Time & Files
  - `TimeDate.h`: `Date`, `Time`, `Format(Date/Time)`, `ScanDate/ScanTime` (ISO-oriented), arithmetic, `GetSysDate/Time`, `GetUtcTime`.
  - `Stream.h`: abstract `Stream`, `MemoryStream`.
  - `FileStream.h`: file-backed Stream using `std::fstream`.
  - `TextIO.h`: `TextReader`/`TextWriter`, `LoadFileAsString`, `SaveFileAsString`.
    - Helpers: `LoadFile(path, out)`, `SaveFile(path, content)`, `AppendFileAsString(path, s)`, `ReadLines/WriteLines`.
  - `Path.h`: based on `std::filesystem` (pattern match, normalize, temp files, symlinks, `GetFileOnPath`, attributes, file times, tz helpers).

- Data
  - `JSON.h`: simple JSON value with parse/dump, pretty-print, path access, and JSON Pointer helpers.
  - `XML.h`: simple XML node with attributes, namespaces (prefix + xmlns), CDATA parsing, pretty-print.

Platform Notes
- Macros set by the aggregator:
  - Windows: `PLATFORM_WIN32=1`
  - POSIX (non-Windows): `PLATFORM_POSIX=1`, `flagPOSIX=1`
  - Directory separators: `DIR_SEP` (char), `DIR_SEPS` (string)

- UTF-8 <-> wide conversions
  - Windows: `MultiByteToWideChar(CP_UTF8)` / `WideCharToMultiByte(CP_UTF8)`.
  - POSIX: try CRT multibyte functions with current locale (`mbsrtowcs`/`wcsrtombs`). If they fail, fallback to a locale-independent UTF-8 codec (assumes `wchar_t` UCS‑4 on typical Linux).

Header Include Policy
- Follow the U++ BLITZ pattern:
  - Source files should include only the package’s main header first: `#include "Core.h"`.
  - Leaf headers avoid system includes. `Core.h` centralizes `<vector>`, `<map>`, `<filesystem>`, etc.
  - Keep per-file system includes local to that file if necessary (e.g., Windows-only headers inside `#ifdef _WIN32`).

Format (U++-style) — What’s Supported
- General grammar: `%[mapping]~ [posN:] [flags] [width|*] [align] [.prec] (keyword|type) [`suffix]`
  - Position: `%2:s` `%1:d`
  - Flags: `0 +  ,  # ! ^ & _ ?` and space; `'` (thousands grouping for integers)
    - `0` zero-pad; `+` always show `+`; space prints leading space for positives; `,` decimal comma
    - `#` ensure decimal point; `!` keep trailing zeros to precision; `^` remove `+` in exp
    - `&` trim exponent zeros; `_` suppress `-0`; `?` prints empty for NaN; `'` groups thousands in `%d/%i`
  - Width: `%10d`, `%0*d` (reads width from next arg)
  - Align: `<` left, `>` right, `=` center (e.g., `|%20=d|`)
  - Precision: `%.7m`, `%.2e`, etc.
  - Keyword types: `month/Month/MONTH`, `mon/Mon/MON`, `day/Day/DAY`, `dy/Dy/DY`, `tw` (12 wrap, `%0tw` zero-pads to 2)
  - Roman: `%r` (lower), `%R` (upper)
  - Letters: `%a/%A` (Excel-like column letters)
  - `m` (and `M`) floating mode:
    - Default general (“%.p g”-like), trims trailing zeros unless `!` present
    - `%.Nm` significant digits; `%.Nm f` fixed; `%.Nm e` scientific (lower); `%.Nm E` scientific (upper)
    - Exponent tweaks: `^` drop `+`, `&` trim zeros (combined → minimal exponent)
  - Integer: `%d %i %o %x %X`, char `%c`
  - Strings: `%s`
  - Integer thousands grouping: `%'d` → groups every 3 digits; separator is `,` by default, `.` if `,` flag present
  - Default spec: `%` formats the next arg with defaults
  - Null mapping: `%[text]~d` uses `text` if arg is null (`Nuller`, `nullptr`, or `Value::IsVoid()`).
  - Suffix: backtick-separated `%d`pt must be written `%d`pt` as `%d\`pt`; the backtick is not printed.

Known Deviations & Notes
- Containers:
  - `Index<T>` uses `std::map` for key index; advanced “unlinked” behavior is not replicated.
  - `VectorMap<K,T>` preserves insertion order; lookups are via a map index.
- `Value` is a minimal `std::any` shim; it does not mirror all U++ `Value` semantics.
- `Stream` is minimal: memory + file; add adapters as needed.
- `Path` relies on `std::filesystem`; creating symlinks may require privileges.
- JSON/XML are intentionally small and cover common usage; avoid depending on strict edge-case parsing.

Tests
- The `stdtst/*` directory contains small packages that test `stdsrc/Core` facilities:
  - `Format` — U++ Formatting
  - `Path` — filesystem helpers
  - `Stream` — Memory/File stream
  - `Convert` — scanning/formatting, dates/times
  - `JSONXML` — JSON/XML parse and simple ops
  - `Base64` — encode/decode roundtrip

Containers — Helper APIs
- `Array<T>`
  - Ownership: `AddOwned(p)`, `InsertOwned(i,p)`, `Detach(i)`, `PopDetach()`
  - Bulk/sizing: `AddN(n)`, `InsertN(i,n)`, `SetCount(n)`
  - Reorder/removal: `Swap(i,j)`, `SwapRemove(i)`, `RemoveIf(pred)`
  - Access/search: `Find(value)`, `FindPtr(ptr)`, `RemovePtr(ptr)`
- `ArrayMap<K,T>` (stable-value pointers)
  - Insert/update: `Insert(i,k,v)`, `AddOwned(k,p)`, `InsertOwned(i,k,p)`, `Put(k,v)`, `Ensure(k,factory)`
  - Access/search: `FindAdd(k)`, `TryGet(k)`, `TryGetKey(k)`, `TrySet(k,v)`
  - Iterate: range-for yields `{key,value}` via `PairRef`; `Erase(it)` removes current
  - Reorder/bulk: `Swap(i,j)`, `Move(i,j)`, `SwapRemove(i)`, `Append(other|range, overwrite=true)`
  - Views: `Keys()`, `ValuesPtr()`; `RemoveIf(pred)`
- `VectorMap<K,T>`
  - Insert/update: `Insert(i,k,v)`, `Put(k,v)`, `FindAdd(k)`, `Ensure(k,factory)`, `TrySet(k,v)`
  - Access/search: `TryGet(k)`, `Contains(k)`
  - Reorder/bulk: `Swap(i,j)`, `Move(i,j)`, `SwapRemove(i)`, `Append(other|range, overwrite=true)`
  - Views/predicates: `Keys()`, `ValuesPtr()`, `RemoveIf(pred)`
- `Index<T>`
  - Insert/change: `Insert(i,k)`, `Set(i,k)` (guards duplicates)
  - Remove/reorder: `RemoveKey(k, idxOut)`, `Swap(i,j)`, `Move(i,j)`, `SwapRemove(i)`, `RemoveIf(pred)`
  - Views: `Keys()` returns const reference to internal vector
  
Pointer Wrappers
- `One<T>`: Minimal unique-owner with familiar helpers. Prefer `Create/Make` over raw `new`; use `Detach/Release` to transfer ownership to caller.
- `Ptr<T>`: Lightweight, non-owning pointer with clear/empty checks and printable form; do not use for ownership.

How to add a test
- Create `stdtst/YourTest/YourTest.upp`:
  - `uses Core;`
  - `file YourTest.cpp;`
- Write a small `main` that prints `OK` and returns 0 on success (non-zero on failure).

Conventions & Contributions
- Add `AGENTS.md` and `CURRENT_TASK.md` to any new `stdsrc` package; list them first in the `.upp` `file` section.
- Keep leaf headers free of system includes. Always aggregate via `Core.h`.
- Adhere to the package independence rules (parent includes child’s main header only).
- Document deviations from U++ behavior explicitly in `AGENTS.md` and in the relevant header if useful.

FAQ / Tips for Agents
- Need to target Windows? Rely on `PLATFORM_WIN32` and Windows APIs are used internally for UTF-8/wide when necessary. You generally don’t need to include `<Windows.h>` yourself — `WString.h` handles it.
- Need BLITZ-friendly headers? Only include `Core.h` in `.cpp` files; never system headers directly in leaf headers.
- Missing a U++ helper? Prefer writing an adapter under `stdsrc/Core` with STL equivalents and document it here.
- Need a quick console program? Use `CONSOLE_APP_MAIN` to define `main(args)` without boilerplate and `LOG("msg" << x)` for simple logging. Configure sinks via `StdLogSetup(LOG_FILE|LOG_COUT, path)` or individually with `LogToStdErr`, `LogToFile`, and `LogSetLevel`.
 - Graphics/Geometry shims
  - `Color.h`: minimal `Color/RGBA` struct.
  - `Gtypes.h`: minimal `Point/Size/Rect`.

- i18n
  - `i18n.h`: `t_/tt_` passthrough, `LngEntry__`, `AddModule` no-op; enough to compile sources using translation macros.
  - `Lang.h`: minimal `LNG_`/`LNGC_` macros and current language string helpers.
