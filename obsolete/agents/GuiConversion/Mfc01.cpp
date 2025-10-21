#ifdef flagMFC
#include <afxwin.h>

class CHelloFrame : public CFrameWnd {
public:
    CHelloFrame() {
        Create(NULL, _T("Hello World program"),
               WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX,
               CRect(100, 100, 420, 340));
        CRect rc; GetClientRect(&rc);
        m_label.Create(_T("Hello world!"), WS_CHILD | WS_VISIBLE | SS_CENTER, rc, this, 1);
    }
protected:
    afx_msg void OnDestroy() { CFrameWnd::OnDestroy(); PostQuitMessage(0); }
    DECLARE_MESSAGE_MAP()
private:
    CStatic m_label;
};

BEGIN_MESSAGE_MAP(CHelloFrame, CFrameWnd)
    ON_WM_DESTROY()
END_MESSAGE_MAP()

void Mfc01(int test_num) {
    if (test_num == 0 || test_num < 0) {
        HINSTANCE hInst = (HINSTANCE)::GetModuleHandle(NULL);
        if(!AfxWinInit(hInst, NULL, ::GetCommandLine(), 0))
            return;

        CHelloFrame* pFrame = new CHelloFrame();
        pFrame->ShowWindow(SW_SHOW);
        pFrame->UpdateWindow();

        MSG msg;
        while (GetMessage(&msg, NULL, 0, 0) > 0) {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }

        delete pFrame;
    }

    // 2. Simple events (Button & click -> popup)
    if (test_num == 1 || test_num < 0) {
        class CButtonFrame : public CFrameWnd {
        public:
            CButtonFrame() {
                Create(NULL, _T("Button program"),
                       WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX,
                       CRect(100, 100, 420, 340));
                m_button.Create(_T("Hello world!"), WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
                                CRect(30, 30, 130, 60), this, 1001);
            }
        protected:
            afx_msg void OnDestroy() { CFrameWnd::OnDestroy(); PostQuitMessage(0); }
            afx_msg void OnButtonClicked() { AfxMessageBox(_T("Popup message")); }
            DECLARE_MESSAGE_MAP()
        private:
            CButton m_button;
        };

        BEGIN_MESSAGE_MAP(CButtonFrame, CFrameWnd)
            ON_WM_DESTROY()
            ON_COMMAND(1001, OnButtonClicked)
        END_MESSAGE_MAP()

        HINSTANCE hInst = (HINSTANCE)::GetModuleHandle(NULL);
        if(!AfxWinInit(hInst, NULL, ::GetCommandLine(), 0))
            return;

        CButtonFrame* pFrame = new CButtonFrame();
        pFrame->ShowWindow(SW_SHOW);
        pFrame->UpdateWindow();

        MSG msg;
        while (GetMessage(&msg, NULL, 0, 0) > 0) {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }

        delete pFrame;
    }
}
#else
void Mfc01(int) {}
#endif
