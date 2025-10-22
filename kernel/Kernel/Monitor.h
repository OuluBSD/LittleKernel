#ifndef _Kernel_Monitor_h_
#define _Kernel_Monitor_h_

// Don't include other headers in this file - only the package header should include other headers

#define VIDEO_ADDRESS 0xb8000
#define MAX_ROWS 25
#define MAX_COLS 80
#define WHITE_ON_BLACK 0x0f
#define RED_ON_WHITE 0xf4

// Screen I/O ports
#define REG_SCREEN_CTRL 0x3d4
#define REG_SCREEN_DATA 0x3d5

class Monitor {
private:
    int cursor_x, cursor_y;
    uint8 default_attribute;
    
    void MoveCursor();
    void Scroll();
    void UpdateCursorPosition(int x, int y);
    
public:
    Monitor();
    void Initialize();
    void Write(const char* str);
    void WriteChar(char c);
    void WriteFormat(const char* format, ...);
    void Clear();
    void SetColor(uint8 color);
    void SetPosition(int x, int y);
    int GetRow() { return cursor_y; }
    int GetCol() { return cursor_x; }
};

#endif