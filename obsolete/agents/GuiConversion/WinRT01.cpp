#ifdef flagWINRT
#include <windows.h>

LRESULT CALLBACK HelloWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    switch (msg) {
    case WM_COMMAND:
        if (HIWORD(wParam) == BN_CLICKED && LOWORD(wParam) == 1001) {
            MessageBoxW(hwnd, L"Popup message", L"", MB_OK | MB_ICONINFORMATION);
            return 0;
        }
        break;
    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;
    }
    return DefWindowProc(hwnd, msg, wParam, lParam);
}

void WinRT01(int test_num) {
    if (test_num == 0 || test_num < 0) {
        HINSTANCE hInst = GetModuleHandleW(NULL);
        const wchar_t* cls = L"WinRT_HelloWorld_Class";
        WNDCLASSW wc{};
        wc.lpfnWndProc = HelloWndProc;
        wc.hInstance = hInst;
        wc.lpszClassName = cls;
        wc.hbrBackground = (HBRUSH)(COLOR_WINDOW+1);
        wc.hCursor = LoadCursor(NULL, IDC_ARROW);
        RegisterClassW(&wc);

        HWND hwnd = CreateWindowExW(0, cls, L"Hello World program",
                                    WS_OVERLAPPEDWINDOW & ~(WS_MAXIMIZEBOX | WS_THICKFRAME),
                                    CW_USEDEFAULT, CW_USEDEFAULT, 320, 240,
                                    NULL, NULL, hInst, NULL);
        if (!hwnd) return;

        RECT rc; GetClientRect(hwnd, &rc);
        CreateWindowW(L"STATIC", L"Hello world!",
                      WS_CHILD | WS_VISIBLE | SS_CENTER,
                      0, 0, rc.right - rc.left, rc.bottom - rc.top,
                      hwnd, (HMENU)1, hInst, NULL);

        ShowWindow(hwnd, SW_SHOW);
        UpdateWindow(hwnd);

        MSG msg;
        while (GetMessage(&msg, NULL, 0, 0) > 0) {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

    // 2. Simple events (Button & click -> popup)
    if (test_num == 1 || test_num < 0) {
        HINSTANCE hInst = GetModuleHandleW(NULL);
        const wchar_t* cls = L"WinRT_Button_Class";
        WNDCLASSW wc{};
        wc.lpfnWndProc = HelloWndProc;
        wc.hInstance = hInst;
        wc.lpszClassName = cls;
        wc.hbrBackground = (HBRUSH)(COLOR_WINDOW+1);
        wc.hCursor = LoadCursor(NULL, IDC_ARROW);
        RegisterClassW(&wc);

        HWND hwnd = CreateWindowExW(0, cls, L"Button program",
                                    WS_OVERLAPPEDWINDOW & ~(WS_MAXIMIZEBOX | WS_THICKFRAME),
                                    CW_USEDEFAULT, CW_USEDEFAULT, 320, 240,
                                    NULL, NULL, hInst, NULL);
        if (!hwnd) return;

        CreateWindowW(L"BUTTON", L"Hello world!",
                      WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
                      30, 30, 100, 30,
                      hwnd, (HMENU)1001, hInst, NULL);

        ShowWindow(hwnd, SW_SHOW);
        UpdateWindow(hwnd);

        MSG msg;
        while (GetMessage(&msg, NULL, 0, 0) > 0) {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }
}
#else
void WinRT01(int) {}
#endif
