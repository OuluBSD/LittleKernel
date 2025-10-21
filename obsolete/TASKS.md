# TASKS

Here are the next "forever tasks".

- Support GUI libraries in addition to the existing ones (Qt, Gtk, Win32, X11, Cocoa)
- Add AI functionality to the uppsrc/ide (TheIDE) package and the packages it requires
- Esc scripting interface for
	- Animation
	- Form
	- Shell Scripting
	- Game scripting
	- website like pages
- Servers (own implementation or wrapper)
	- ftp
	- https
	- ssh
	- internet radio
	- internet video
	- game servers
	- peer to peer nodes
	- web-browser based control panels
- Web-Browser compatibility alternatives
	- TURTLE: programs runs natively in server and user has client program in the web-browser
	- Emscripten: the program is compiled into webassembly or javascript
		a. The GUI is like in TURTLE via VirtualGui
		b. Gui is via Eon Atoms and VirtualGui in the WebGL engine
		c. No Ctrl support at all, but Core and Draw work, and the 3D engine runs in the main loop.
	- Translation: The entire repository is translated into a TypeScript library, and parts of the implementation and interface are omitted because TypeScript is an object-oriented language, not memory-based.
	- Esc is translated into JavaScript (easy). Esc functions are linked to JavaScript libraries. This is a good idea. That is, the features of the Esc library are implemented using well-known javascript/typescript libraries that work in web browsers.
	- The GUI is HTML code in the client's web browser. The Ctrl functions are converted to HTML code on the server. The session is kept in memory on the server. The interface would be familiar to the programmer from CtrlCore and CtrlLib. Programming would be done in C++.
- Graph and Atom systems
	- add better graph-based systems for connecting atoms (now complex loop-based)
	- add atom classes that wrap various c++ libraries in "uppsrc/api/" subdirectories.
	- (obsolete/)share/eon/tests should showcase new atoms
- uppsrc/Skylark
	- Add classes to implement a traditional website: forums, communications, content, video pages, news. Add classes for gui widgets and gui layouts. Add classes to make it easy to make a good website with skylark.
- (obsolete/)kernel/LittleKernel: make own hobby OS, and then some more!
- Codex like AI functionality
	- see files:
		- "AICore2/AICore.upp" -> Task Manager separator
			- files related to AI calling
		- IDE frontend similar to Codex webpage
			- "uppsrc/ide/Shell/Agent.h"
			- "uppsrc/ide/Shell/Agent.cpp"
		- Vfs classes for containing tasks etc.
			- "uppsrc/AICore2/Agent.h"
		- Vfs classes for visualizing tasks etc.
			- "uppsrc/AICtrl/Agent.h"
		- Enumerate new backend AI prompt:
			- "uppsrc/AICore2/Common.h"
		- AI prompt implementation:
			- "uppsrc/AICore2/CreateInput.cpp"
