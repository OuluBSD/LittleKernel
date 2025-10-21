#include <Core/Core.h>
using namespace Upp;

#include <Form/Form.hpp>

GUI_APP_MAIN
{
	String file = ConfigFile("HelloWorld.form");
	if (CommandLine().GetCount())
		file = CommandLine()[0];
	FormWindow form;
	form.Load(file); // see Form/Examples directory
	form.Layout("Default");
	form.Run();
}
