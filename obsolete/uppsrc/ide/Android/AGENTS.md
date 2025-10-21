AGENTS

Scope
- Applies to `uppsrc/ide/Android`.

Purpose
- Android SDK/NDK integration: detect tools, manage manifests/APKs, run `adb`, read `logcat`, and support building via `ide/Builders`.

Package Overview
- Manifest: `Android.upp` (uses `ide/Core`, `plugin/pcre`).
- Files: `Android.{h,cpp}`, `AndroidSDK.cpp`, `AndroidNDK.cpp`, `AndroidManifest.cpp`, `Apk.cpp`, `Executables.h`, `Adb.cpp`, `LogCat.cpp`, `NDKBuild.cpp`, `Devices.h`.

Extension Points
- Add platform tools and queries in `AndroidSDK.cpp`/`AndroidNDK.cpp`.
- Extend project/manifest handling in `AndroidManifest.cpp` and `Apk.cpp`.
- Surface APIs for `ide/Builders` via headers.

.upp File Notes
- Keep `AGENTS.md` first in the `file` list and use separators for logical groupings.

