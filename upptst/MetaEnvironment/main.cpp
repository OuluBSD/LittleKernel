#include <Core/Core.h>
#include <Vfs/Core/Core.h>
#include <Vfs/Overlay/Overlay.h>
#include <ide/Builders/Builders.h>
#include <ide/Vfs/Vfs.h>
#include <ide/clang/clang.h>

using namespace Upp;

Builder *CreateScriptBuilder();

namespace {

struct DummyOverlayView : OverlayView {
    Vector<String> List(String) const override { return Vector<String>(); }
    Value GetMerged(String) const override { return Value(); }
};


void ScriptBuilderOverlayStub()
{
    // Ensure ScriptBuilder symbols stay linkable while overlay integration is pending
    using BuilderFactory = Builder* (*)();
    BuilderFactory factory = &CreateScriptBuilder;
    (void)factory;

    LOG("TODO: drive ScriptBuilder to emit overlay fragments via MetaEnvironment");
}

void AssistAnnotationStub()
{
    AnnotationItem item;
    item.name = "Stub";
    item.pos = Point(1, 1);

    // Stash a dummy path to keep seen-path index active
    MetaEnvironment& env = MetaEnv();
    env.AddSeenPath("/virtual/assist_stub.cpp");

    ASSERT(item.name == "Stub");
    LOG("TODO: load libclang annotations into overlay-backed MetaEnvironment");
}

void SerializationStub()
{
    MetaEnvironment& env = MetaEnv();

    Vector<String> paths;
    paths.Add("/virtual/serialization_stub.cpp");
    env.AddSeenPaths(paths);

    hash_t realized = env.RealizeTypePath("::StubType");
    ASSERT(realized != 0);

    using StoreFn = void (*)(IdeMetaEnvironment&, String&, const String&, ClangNode&);
    StoreFn store = &Store;
    (void)store;

    LOG("TODO: serialize per-file VfsValue fragments to META_FILENAME and reload overlays");
}

void OverlayUnionStub()
{
    DummyOverlayView dummy;
    Vector<String> listed = dummy.List("/virtual/root");
    ASSERT(listed.IsEmpty());

    MetaEnvironment& env = MetaEnv();
    env.AddSeenPath("/virtual/overlay_stub.cpp");

    LOG("TODO: verify virtual root iteration across multiple overlay fragments");
}

} // namespace

CONSOLE_APP_MAIN
{
    StdLogSetup(LOG_COUT|LOG_FILE);

    ScriptBuilderOverlayStub();
    AssistAnnotationStub();
    SerializationStub();
    OverlayUnionStub();

    LOG("MetaEnvironment stub suite finished");
}
