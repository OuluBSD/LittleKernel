#ifdef flagWTL
#include <atlbase.h>
#include <atlapp.h>
CAppModule _Module;
#include <atlwin.h>
#include <atlframe.h>
#include <atlctrls.h>


void Wtl01(int test_num) {
    if (test_num == 0 || test_num < 0) {
		class CHelloFrame : public CFrameWindowImpl<CHelloFrame> {
		public:
		    DECLARE_FRAME_WND_CLASS_EX(L"WTL_HelloWorld_WindowClass", 0, CS_HREDRAW | CS_VREDRAW, COLOR_WINDOW)
		
		    BEGIN_MSG_MAP(CHelloFrame)
		        MESSAGE_HANDLER(WM_CREATE, OnCreate)
		        MESSAGE_HANDLER(WM_DESTROY, OnDestroy)
		    END_MSG_MAP()
		
		    LRESULT OnCreate(UINT, WPARAM, LPARAM, BOOL&) {
		        RECT rc; GetClientRect(&rc);
		        m_label.Create(m_hWnd, rc, L"Hello world!", WS_CHILD | WS_VISIBLE | SS_CENTER, 0);
		        SetWindowText(L"Hello World program");
		        CenterWindow();
		        return 0;
		    }
		
		    LRESULT OnDestroy(UINT, WPARAM, LPARAM, BOOL&) {
		        PostQuitMessage(0);
		        return 0;
		    }
		
		private:
		    CStatic m_label;
		};
        _Module.Init(NULL, GetModuleHandle(NULL));
        CMessageLoop theLoop;
        _Module.AddMessageLoop(&theLoop);

        CHelloFrame wnd;
        if (wnd.CreateEx() == NULL) {
            _Module.RemoveMessageLoop();
            _Module.Term();
            return;
        }
        wnd.ShowWindow(SW_SHOW);
        theLoop.Run();
        _Module.RemoveMessageLoop();
        _Module.Term();
    }

    // 2. Simple events (Button & click -> popup)
    if (test_num == 1 || test_num < 0) {
        class CButtonFrame : public CFrameWindowImpl<CButtonFrame> {
        public:
            DECLARE_FRAME_WND_CLASS_EX(L"WTL_Button_WindowClass", 0, CS_HREDRAW | CS_VREDRAW, COLOR_WINDOW)

            BEGIN_MSG_MAP(CButtonFrame)
                MESSAGE_HANDLER(WM_CREATE, OnCreate)
                COMMAND_ID_HANDLER(1001, OnButtonClicked)
                MESSAGE_HANDLER(WM_DESTROY, OnDestroy)
            END_MSG_MAP()

            LRESULT OnCreate(UINT, WPARAM, LPARAM, BOOL&) {
                m_button.Create(m_hWnd, CRect(30, 30, 130, 60), L"Hello world!", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, 0, 1001);
                SetWindowText(L"Button program");
                CenterWindow();
                return 0;
            }

            LRESULT OnButtonClicked(WORD, WORD, HWND, BOOL&) {
                MessageBox(L"Popup message", L"", MB_OK | MB_ICONINFORMATION);
                return 0;
            }

            LRESULT OnDestroy(UINT, WPARAM, LPARAM, BOOL&) {
                PostQuitMessage(0);
                return 0;
            }

        private:
            CButton m_button;
        };

        _Module.Init(NULL, GetModuleHandle(NULL));
        CMessageLoop theLoop;
        _Module.AddMessageLoop(&theLoop);

        CButtonFrame wnd;
        if (wnd.CreateEx() == NULL) {
            _Module.RemoveMessageLoop();
            _Module.Term();
            return;
        }
        wnd.ShowWindow(SW_SHOW);
        theLoop.Run();
        _Module.RemoveMessageLoop();
        _Module.Term();
    }
}
#else
void Wtl01(int) {}
#endif
