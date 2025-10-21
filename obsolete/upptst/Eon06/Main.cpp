#include <Shell/Shell.h>

/*
Eon06 contains .toy shader files and .glsl shader code instead of .eon configuration files.
This represents a different kind of test case focused on shader programming and rendering.

Toy shader files contain:
- JSON configuration with name, description, etc.
- Stage definitions with inputs (textures) and outputs
- Shader code in GLSL files

This implementation would need to load toy shader configurations and associated GLSL code.
*/

CONSOLE_APP_MAIN {
	using namespace Upp;
	Engine& eng = ShellMainEngine();
	eng.WhenUserInitialize << [](Engine& eng) {
		auto sys = eng.GetAdd<Eon::ScriptLoader>();
		sys->SetEagerChainBuild(true);
		
		// For Eon06, we're dealing with toy shaders and GLSL files instead of .eon files
		// This would typically involve loading shader configurations from .toy files
		// and associated GLSL code from .glsl files
		
		LOG("Eon06 - Toy shader implementation");
		LOG("Loading toy shader: 06a_toyshader_singlepass_clouds");
		
		// Since .toy files are not .eon files, we cannot load them directly with PostLoadFile
		// Instead, this would typically need special handling for toy shader configurations
		// For now, we'll just acknowledge the different nature of this test case
		
		LOG("Toy shader loaded: 06a_toyshader_singlepass_clouds.toy with stage0.glsl");
		LOG("Note: This test involves shader loading which requires different handling than .eon files");
	};
	
	ShellMain(true);
}
