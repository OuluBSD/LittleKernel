#ifdef HAVE_VSTGUI
// VSTGUI typically requires a plugin/editor context.
// Provide dummy placeholders to show intended usage.
namespace DummyVst {
    struct Context {};
    struct Window {};
}

void Vst01(int test_num) {
	// Dummy null pointers representing host context and editor window
	DummyVst::Context* hostContext = nullptr;
	DummyVst::Window* editorWindow = nullptr;

	(void)hostContext; // would be passed to VSTGUI frame/view creation
	(void)editorWindow; // would represent the plugin editor window handle
	
	if (test_num == 0 || test_num < 0) {
		// Example (pseudocode) of what real code might look like:
		CRect size(0, 0, 320, 240);
		CFrame* frame = new CFrame(size, editorWindow, hostContext);
		CTextLabel* label = new CTextLabel(size, "Hello world!");
		label->setHAlign(kCenterText);
		frame->addView(label);
		frame->open(editorWindow);
	}

	// 2. Simple events (Button & click -> popup)
	if (test_num == 1 || test_num < 0) {
		// Pseudocode: In a real plugin editor, create a button and show a popup
		CRect size(0, 0, 320, 240);
		CFrame* frame = new CFrame(size, editorWindow, hostContext);
		CRect btnRect(30, 30, 130, 60);
		COnOffButton* button = new COnOffButton(btnRect, nullptr, 0);
		button->setTitle("Hello world!");
		button->setOnClick([frame]{
			// Show an information message in host/editor context
			// (Real code would use host APIs or VSTGUI facilities)
			/* Alert: "Popup message" */
		});
		frame->addView(button);
		frame->open(editorWindow);
	}
}
#else
void Vst01(int) {}
#endif
