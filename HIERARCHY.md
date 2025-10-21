# Directories and hierarchy

Look in the folder to see if there is a file with the same name as the folder and a ".upp" extension. There may be a "description" section that briefly explains the contents of the folder.

Files ending in ".upp" are project files or package configuration files for TheIDE development software. These files contain a file list and a list of required packages. The required packages list indicates where to download the package's ".upp" file (when the folders to search are known), so the package list is a recursive directory (unless there are circular dependencies).

**So by following the ".upp" files you can figure out the package hierarchy.**La


- autotest/
	- Hundreds of test packages for each part of the library (e.g., Core, Xml, Http, Unicode, etc.).
- bazaar/
	- A large collection of separate add‑on packages and sample applications such as AESStream, ChromiumBrowser, CoWork, FSMon, TabBar, and more.
	- Previously, this was packages added by external users that did not provide any Core functionality. Now, you can think of this as a staging area for some of the features that are imported into Core.
- bazaar/plugin
	- external libraries that have been imported into this repository. These have a U++ TheIDE project file, so they are easy to include in projects.
- benchmarks/
	- Performance benchmarks: AllMaps, CoWork, LZ4, PainterBenchmark, VectorBig, etc.
- doc/
	- Contains rather irrelevant documentation files in Markdown format. The code documentation can be found in folders (not files) ending in ".tpp" which contain files ending in ".tpp" in this project's own QTF file format (a bit like RichText).
- examples/
	- Small sample projects (e.g., AddressBook, GoogleMaps, PainterExamples, SQLApp, SvgView).
- reference/
	- Example code and tests for nearly every feature (e.g., AESEncryption, ArrayCtrlDnD, CoWorkCancel, HttpServer, Xmlize).
- tutorial/
	- Step‑by‑step tutorial packages: Gui01–Gui24, Draw01–Draw06, Sql01–Sql06, Network01–02, etc.
- uppsrc/
	- The main source tree; contains modules like Core, Draw, CtrlCore, CtrlLib, Painter, PdfDraw, Sql, Terminal, and many others.
- upptst/
	- Test applications; parallel examples such as upptst/DrawArc, upptst/PdfText, upptst/GuiMtTest, upptst/Zip, etc.
- rainbow/
	- Various platform ports and graphical experiments (SDL20, WinFb, Gtk, PaintGl, …).
- uppbox/
	- Tools, scripts, and installation packages (MakeInstall*, WinInstaller, UppBuilder, Unicode, …).
- obsolete/
	- Older versions or removed features, organized into subdirectories (src, tutorial, uppsrc, upptst, etc.).

Additionally, the root directory contains files such as AGENTS.md, LICENSE, README.md, Makefile, configure, and other build/configuration files.


## "uppsrc" directory

### "ide" directory
The ide directory contains sub-packages that complement ide, but which are strongly related to other main packages, but which cannot be moved there because they require the ide package, and it cannot be made a general requirement.

#### "ide/Edit3D"

3D editor inside the IDE. Includes keypoint timeline, like Adobe Animate. Does not require a 3D accelerator, but works with a software renderer.

### Repeated parts of the name: Core, Ctrl, Lib, Draw, AI, Meta, Eon, Gubo, Rich, Soft:

Core packages contain the most essential code. Core alone is the most essential, and CtrlCore contains all the most essential GUI code. Lib is less essential compared to Core (CtrlCore -> CtrlLib).

#### The "Core -> Draw -> Ctrl" chain (and the "Core2 -> Draw/Extensions -> Ctrl2" chain)
The "Core -> Draw -> Ctrl" division is based on the fact that "Core" code can be run in a headless environment and in a terminal, and it does not contain graphics at all. "Draw" code can be run in a headless environment on a server, but it contains code that is strongly included outside the headless environment and in user applications. "Ctrl" code is for client programs and graphical user interfaces, and it is cross-platform and cross-platform, supporting several user interface libraries (Qt, Gtk, Win32, X11, Cocoa).

#### The "Core2 -> Vfs -> Meta -> AI -> Eon" chain
Core2 (v2) contains very fundamental classes and functions that are also strongly related to Vfs,Meta,AI code, and it would be too much to include in every regular program via the Core (v1) package.

Vfs is strongly associated with AST functions and the Clang parser of the code. However, it is also used for all dynamic data management within the user program and even for C++ metaprogramming.

The Meta folder contains code that is more general than the AI-centric code in the AI ​​folder. Meta contains a lot of code related to the IDE package, where custom GUI tools are made using the Vfs hierarchy, where the tool is related to the location of the Vfs tree, and probably also to the Vfs clang-based AST plugin, meta-programming and its visualization.

The AI ​​folder contains artificial intelligence functions built on the runtime VFS tree, which may have a GUI tool or visualization.

Eon originally stood for ECS Object Notation, but that name doesn't mean that anymore, it's just a nice name for anything ECS ​​related. It also includes code for a game/multimedia engine, where the engine parts are also in a kind of ECS-like world, but where the ECS world of the game is a completely different thing, with different classes. The Eon and ECS code is heavily based on the Vfs code, and also uses artificial intelligence functions, so it is the last in the chain.

For a comprehensive Eon overview (architecture, script DSL, Atoms/Links, ECS integration, and extension guide), see: `uppsrc/Eon/AGENTS.md`.

#### The "Ctrl -> Gubo" chain
"Gubo" or "Graphical Cuboid" is a class that corresponds to the "Ctrl" libraries (almost one to one), but which is in the 3D world and not in the 2D world. All Ctrl classes can be tried to be converted into "Gubo" classes by adding a "dimension" or converting 2D->3D and taking into account the related issues.

GuboCore may include a wrapper for 3D engines, in the same way that CtrlCore uses gui libraries (Qt, Gnome, etc.). However, a custom engine is essential (which is implemented using Eon's graph structures), and this can be implemented using the VirtualWorld class (as is the VirtualGui package for 2D Ctrl functions). This is a bit uncertain and undefined yet.

#### The "CtrlCore -> CtrlLib" chain

CtrlCore contains all the general and essential structures, which allows the use of different GUI libraries, and which is very essential for GUI-related things.

CtrlLib contains all the specialized GUI-Widget classes and related classes. If a new general GUI widget is to be created, it is added here by default.

CtrlCore does not use widgets from other GUI libraries, only the main window, events and theme. All events are converted to CtrlCore internal events. The GUI library theme is loaded and CtrlCore "disguises" itself as a native GUI by loading the theme to improve the user experience.

#### The "RichText -> RichEdit" chain
Rich text is similar in concept to the RTF file format familiar from Microsoft WordPad, but in this library the information is in QTF files (rather than RTF) and contains slightly different functions.

The RichText package contains classes for viewing QTF text, and the RichEdit package classes can be used to implement a text editor similar to WordPad.

#### "Soft" libraries

The word "soft" in a name usually means that it is a local, compact implementation of some function that would normally require a large library. For example, SoftRend is a 3D renderer (replacing OpenGL) and SoftPhys is a physics library (replacing ODE).

### Versions

Includes versions 1 and 2 of some packages, such as Core, Core2, CtrlLib, CtrlLib2, etc. This is done to freeze certain feature packages. Version 2 includes a more advanced VfsValue class and the ECS library on top of it.

### GridCtrl

Package for Excel -like features.

### ComputerVision

Contains functions similar to the OpenCV library, and can also wrap libraries similar to the OpenCV library. All computer vision and webcam related code is added here. Possibly also functions required by the image processing program.

### DesktopSuite

The VirtualGui function (and the VIRTUALGUI use flag) enables a desktop in e.g. an SDL2 window, and the classes in this package enable an operating system in an SDL2 (or similar) window. Here "Explorer" corresponds to the file manager. "TaskBar" corresponds to the Windows taskbar or OSX Docker. If the package threatens to become too large, sub-packages can be created in the folder, as in the ide package.

### Adventure

The Adventure package includes a SCUMM style game engine. This should use the Vfs,Meta,Eon classes, but it still contains old code.

### Esc, EscAnim

Esc is a scripting language, like JavaScript or ActionScript, but the syntax is different and this is implemented using the conventions and strengths of Ultimate++. This provides a smooth experience for C++ programmers in this repository, as the scripting language resembles familiar conventions as much as possible.

EscAnim corresponds to the functions of Adobe Flash (Adobe Animate) kernel in the way that Esc corresponds to ActionScript. This package contains a 2D graphics engine and classes for displaying graphics and multimedia. If you wanted to implement a program similar to Adobe Animate, then the core functions related to animation would be added here, which would give the user's programs functions similar to "Flash Player".

Functions similar to Adobe Animate itself would be in the EscAnimEdit package, except for the main program. The main program is integrated into the IDE.

## The "api" directory

This is a very important folder. The graph-based Eon engine uses atoms, which are located here. Atoms can wrap system libraries into audio, image, video, friends, etc. Atoms have an interface that defines the input and output ports and the format of the packets passing through them (video, audio, event), etc.

Supporting new libraries is done by looking at old libraries and copying them closely.

These headers are generated automatically, but can still be written to by AI Agents. It will be passed to the generator by humans later.

### "api/Audio"

These Atoms can wrap, for example, "portaudio", the native Windows audio libraries, "pipewire".

### "api/AudioHost"

These Atoms can wrap, for example, "VST", "LADSPA", "LV2", "CLAP".

### "api/Camera"

These Atoms can wrap, for example, "V4L", "Windows native camera api", "OpenCV", "Internet camera libraries".

### "api/Effect"

These Atoms can wrap, for example, specific "Reverb effect libraries", "FFT effect libraries", even specific "VST effects".

### "api/Graphics": OpenGL

This contains template functions for an OpenGL based framework. This can implement 3D pipelines that resemble OpenGL. This also includes a software renderer for debugging. Everything needs to work with the software renderer before it works with the native 3D library. Cinematic 3D renderers can also be added here if they are similar to the OpenGL API. DirectX is unlikely to support this, and probably needs its own "Graphics2" folder.

### "api/Graphics2": DirectX
DirectX template classes like in the "Graphics" package. Also includes a software renderer, which can be used to debug DirectX functions. Cinematic 3D renderers can also be added here if they are similar to the DirectX API. 

### "api/Hal": Hardware Abstraction Layer libraries

This can wrap comprehensive libraries such as SDL, SDL2, SDL3, Allegro, SFML. Even wrapping the Internet browser functionality of a WebAssembly target can happen here if the program is compiled with Emscripten.

### "api/Holograph": VR libraries

This includes atom classes for VR libraries such as OpenVR, OpenHMD, own OpenHMD fork LocalHMD. This could include atoms for Windows WMR libraries as well, if they have a C++ interface. This may also include Android VR support if the program is ported to Android. This could also include Javascript VR support if the program were compiled with Emscripten for the web browser.

### "api/Media"

This package contains atoms for loading video, audio, 3d model, etc. classes. Example libraries include ffmpeg, windows libraries, gstreamer, vlc, etc.

### "api/MidiFile"

Loads MIDI files and sends them forward as events.

### "api/MidiHw"

Reads and writes commands sent by MIDI hardware.

### "api/Physics"

Supports various C++ physics libraries. Requires template classes such as "api/Graphics". Supports primarily the Open Dynamics Engine (ODE) library and libraries that are somewhat compatible with its interface.

### "api/Screen"

Includes Atom classes for just windowing systems without sounds and more. Example libraries: X11, Win32 api, gnome, qt, directfb, DOS frame buffer, etc.

### "api/Synth"

Supports audio synthesizer libraries and frameworks. Sort of like VST instrument support.

### "api/Volumetric"

Support for pointclouds and other 3D data that are not ready-made 3D models. Scanned 3D models can also be included here, as long as pointclouds are still part of them.


## Form, FormEditor

Creating dynamic GUI window content from form files. Compatible with ESC script, allowing the entire program and GUI to be loaded from files at runtime. Form and Esc compatibility is incomplete.

## Geometry

A self-implemented glm-like library for 3D mathematics. This is more limited than glm.

## Intranet

Communication between client programs between different computers. Both server-client and p2p based. Incomplete.

Distribute tasks to other machines if possible. This allows you to call, for example, artificial intelligence functions on the graphics cards of other computers.

This can also be used to read devices on other computers if the client is running there.

This is intended to make it easier to create networks with more difficult devices, such as Android phones.

## Project cleaner

A very limited package for now. This package includes functions for importing C++ projects into the structure generated by the IDE. For example, some antique C++ and C projects are really out of order or without project files, so the structure is guessed by reading the "#include" statements in the ".h" and ".cpp" files.

## Skylark

A package that allows clients to run in internet browsers easily. Includes a web server. Pages are written as ".witz" files, which are html code, but which includes some glue functions and even Esc code execution on the server before sending the web page. This is the easiest and fastest way to make a suitable interface for all clients, if the scope of the html code is not a problem.


## SoftAudio

This library contains all the functionality required for a sound synthesizer. The purpose would be to make modern VST synthesizers possible. This includes the "Core" features and the synthesizer GUIs will be somewhere else.

## Sound

This package is an easy-to-use library for audio. This is not the right library if you plan to combine a lot of audio sources and outputs, as it requires a graph- and atom-based multimedia engine. This allows you to easily record audio and play individual audio files. This can also be used to do some basic audio streaming.

## Sql

A package that enables the use of different SQL servers with the same interface using familiar conventions.

## TrustCore

A trust propagation algorithm and trust metric for local group trust computation. Related classes and functions. Trust-based Moderation. Innovative package. Partly related to artificial intelligence.

## umk

The "make" tool in this repository. It compiles executable files from u++ packages, just like the "uppsrc/ide" program.
Laytou
Only supports ".upp" files, not other Makefile/CMake/etc. files.

## uppweb

Translates the entire repository documentation into html so it can be placed on websites.

## VirtualGui

This works in parallel with other graphics interfaces inside CtrlCore. So VirtualGui is a backend like gtk, x11, win32, cocoa, but the user can attach it wherever they want, like inside an SDL2 window (includes a window manager).

This is enabled in the program by adding the VIRTUALGUI use flag and enabling a backend, such as sdl2: SDL2GL.
See ".upp" files and "mainconfig\n\t\"\" = \"GUI SDL20GL\";"

## VirtualGui3D

CtrlCore 2D gui windows working in the 3D world. Unfinished.

## LayoutCtrl

A simple html renderer. See "reference/BasicHtmlRenderer" for an example.

This would also be used to create 3D pages inside a 3D engine. That is, the user would see a virtual page with depth, not just a flat plane.


# "obsolete" directory

This is not completely unusable, but valid code and packages are still being moved away from here.

## "alt" directory

This is an alternative implementation for some of the libraries in the uppsrc folder. They are "clean room reverse engineering" implementations, when only the "reference" "tutorials" "bazaar" folders and documentation were known. Their implementation may be very different.


## "kernel" directory

"kernel" contains a small kernel that runs on an x86 virtual machine. The idea is that this kernel can also be compiled into a regular program on regular operating systems by changing the include statements to another library.

So there is an alternative package that replaces the LittleKernel package, so that the program starts a terminal or gui operating system, just like a virtual machine would start a kernel. The kernel is therefore monolithic and also contains a gui.

The idea is that debugging is easier when the kernel can be compiled into a windows program and a real x86 kernel for the virtual machine

## "share" directory

This folder should be moved to the root directory if it is being reviewed and checked. It contains everything that the graph-based multimedia engine Eon may need. Also inside this folder are the graph configuration files with the ".eon" extension.

### "eon" directory

This contains files for a graph-based real-time multimedia engine.
The syntax of the file is quite bad, and "eon v2" should be done. This is also "loop" based, as we want a more waterfall-like graph.

### "eon/tests" directory

This shows well what the "eon v1" real-time multimedia engine is capable of. The tests start with events and sound, and move on to images, and more complex ecs worlds in the final tests. These have worked at some point, but the engine must have broken.

All of these are to be converted to "eon v2" syntax and moved to the active folder out of the "obsolete" folder. All of these tests are to be made to work again. All of the code required for these tests has been included in this repository.

### "imgs"

Images to load.

### "models"

3D-models to load.

## "rainbow" directory

This contains alternative CtrlCore backends, and related code. These are enabled by adding a flag to mainconfig in the IDE. Examples that work include SDL20GL, SDL20...



