CURRENT TASK

Objective
- Embed Gubo (3D) rendering into CtrlCore windows via a DHCtrl-based control, suitable for OpenGL rendering.

Plan
- Context creation
  - Win32/WGL: create context for DHCtrl::GetHWND(), choose pixel format, wglCreateContext, wglMakeCurrent.
  - X11/GLX or EGL (GTK/X11): create GLX/EGL context for the native child window.
  - Cocoa: NSOpenGLContext bound to NSView from child.
  - Store and destroy on CLOSE.
- Rendering
  - Implemented basic GLX render path in GuboGLCtrl::RenderGL:
    - Y-down orthographic projection, blending, depth test, clear.
    - Ensures TopGubo layout and redraw, then replays minimal DRAW3 commands (BOX_OP) to GL QUADS.
    - Swaps buffers via glXSwapBuffers.
  - Next: expand DrawCommand3 replay (lines, images placeholder) or integrate with Gu::GuboManager renderer.
- Event forwarding
  - Map Ctrl mouse/keys to CtrlEvent and dispatch to TopGubo/manager (implemented).
- Demo
  - examples/GuboGLCtrlDemo shows layout and event forwarding; fill in GL context to render.

Notes
- Alternative path: embed Shell into TopWindow by creating a child native window and using SDL_CreateWindowFrom or an engine hook to render into that handle; forward events similarly.
