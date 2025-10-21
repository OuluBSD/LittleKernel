#ifdef HAVE_AGAR
#include <agar/core.h>
#include <agar/gui.h>

static void Agar_OnButton(AG_Event *event) {
    (void)event;
    AG_TextMsg(AG_MSG_INFO, "Popup message");
}

void Agar01(int test_num) {
	if (test_num == 0 || test_num < 0) {
		if (AG_InitCore("agarapp", 0) == 0) {
			if (AG_InitGraphics(NULL) == 0) {
				AG_Window *win = AG_WindowNew(0);
				AG_WindowSetCaption(win, "Hello World program");
				AG_LabelNew(win, 0, "Hello world!");
				AG_WindowSetGeometry(win, 100, 100, 320, 240);
				AG_WindowShow(win);
				AG_EventLoop();
				AG_Destroy();
			}
			AG_Quit();
		}
	}

	// 2. Simple events (Button & click -> popup)
	if (test_num == 1 || test_num < 0) {
		if (AG_InitCore("agarapp", 0) == 0) {
			if (AG_InitGraphics(NULL) == 0) {
				AG_Window *win = AG_WindowNew(0);
				AG_WindowSetCaption(win, "Button program");
				AG_ButtonNewFn(win, 0, "Hello world!", Agar_OnButton, NULL);
				AG_WindowSetGeometry(win, 100, 100, 320, 240);
				AG_WindowShow(win);
				AG_EventLoop();
				AG_Destroy();
			}
			AG_Quit();
		}
	}
}
#else
void Agar01(int) {}
#endif
