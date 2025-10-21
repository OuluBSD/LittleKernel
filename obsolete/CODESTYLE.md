# Ultimate++ code style

## Introduction

Code style has always been subject to a lot of discussions, some of them being really passionate and emotional. Those of you already having seen the intimate details of Ultimate++ in uppsrc and maybe bazaar surely have a concept in a certain degree of order, functionality and maybe, like I did, even a certain beauty (yes, I am married in real world :), all hard to describe. For me the code felt useful, well structured and quite easy to read and thus easier to understand. And this is maybe the essence of good code style. All the things mentioned below are tiny little details that can help to achieve this overall perception in U++ code. Don't hesitate to look at the code in uppsrc to learn. I admit, that to 'learn' U++ style isn't done in a day because it isn't a fixed law. One needs to have worked with it for a while and feel some comfort with it. But this document should shorten the time.

If you like to contribute code or major fixes, you are encouraged to pay attention to these things. It makes life easier for the community, which usually keeps an eye on the changes made to the code base. It enables us (the community, of which you are about to be part of) to spot bugs fast and provide fast fixes. Having X coding styles mixed together in a project is just about the worst case. Though U++ is split in packages it is still considered one single project. So, please make sure you consider overall goals that will be introduced in the next paragraph.

## Design

- Produce clean, 'intuitive' and compact code, making usage easy as well as reading (and ideally understanding) the code.

- Use Topic++ (.tpp files) to comment your class usage, this is the prefered method of code commenting. Use separate files for API and implementation documentation. TheIDE can generate the appropriate files for you. Click on the little cyan rectangles next to functions, variables or classes. 'New reference topic" adds to 'src.tpp', 'New implementation topic' adds to 'srcimp.tpp'. Provide also the "Big picture" design aspects of your package / class, briefly outlined in the src.tpp as a header (see U++ class documentation as an example). This relieves a lot of headache when studying the code.

- Be familiar with U++, use its facilities. This is a general prerequisite anyway, but it means to have studied the existing code and its possibilities a bit. It might happen that you have invented things that U++ already provides for you and you could have saved your time. It also means to know, understand, and thus stick to the design pattern, i.e. when implementing new Ctrl's, like knowing that there is a virtual Updated() function to be overridden to recalculate the Ctrl's state only, without repaint, and is called with Ctrl::Update() in a generalized manner. Another example is the Callback idea or the NTL Containers.

- The first file in a package is usually a file with the same name as the package, ending in ".h". There are exceptions to this, however. Sometimes it is the second file, and the first file is a "Readme.txt" (or similar) or a "Cypying" file.
  This header named package includes all other headers in the package. All ".c" and ".cpp" include this header named package. This helps compile packages faster because the IDE's BLITZ header precompiler uses this information.

- Each class has got its own .h and (maybe multiple) .cpp (or .hpp for template), except for classes that are tight together really closely and belong together.

- Include package files using "Insert package directory file" mainly, instead of using "Insert any file". Otherwise use relative to package directory file paths instead of absolute paths.

- If you use a Bazaar package, include it to your Assembly doing this: "File > Set Main Package", "Assembly" area, right button, "Edit assembly..." option, "Package nests:" field, adding the absolute path to Bazaar at the end.

- Include relative paths instead of absolute in #includes, like #include <Scatter/Scatter.h> instead of #include "/folderA/folderB/Scatter.h"

- Use the package idea of U++. A package compounds one or more classes and its files, because they logically belong together and should all be available when using the package inside another package. A package should have an overall name and a .h with same name (which then can #include the other class headers if any). Example: #include <Core/Core.h>. This is the way you also should use other packages inside yours. (See 'Overview' for more info on packages)

- Provide a Test / Demo package for your package. Generally it is a good idea to have a 'MyPackage' and 'MyPackageTest' package shipped together, where functionality and Testing / Presenting is separated. Others won't need to trouble with the code too much if the can simply #include <MyPackage/MyPackage.h>. a package whose usage they have observed in the Test package.

- Avoid usage of new / delete as hell. Use U++ NTL containers for your data or encapsulate the allocation or deallocation of objects. Memory leaks can cause serious brain damage :). of course, don't mix new/delete with malloc/free. Pointers should only be used to really point to things and NOT to maintain the last reference to a dynamically allocated object (this should be done in containers, Array<>, Vector<>, One<>, Any<>, etc. everything belongs somewhere...).

- Avoid usage of  #define constants, use compile time safe constants inside your class where possible. Example: static const int MaxSize = 100;

- Do not forget the virtual ~Destructor(), As soon as your class is meant to be a base class (i.e. has any other virtual function) it should have a virtual destructor, even if it's empty.

- Pay attention to implicit conversion. Some bugs come from constructors parametrized by default, which are used then as implicit conversion constructors. (you may protect with the 'explicit' keyword)

- Maintain the const correctness. Failing to do so opens doors to many bugs and makes the usage of your class difficult in classes that maintain it.

- Use integer counter access. Do not use iterators on the Containers. Example:
```
for(int i = 0; i < vec.GetCount(); i++)

{

   vec[i] = 0;

}
```

- User for each loop when you want to go over whole container.

- Use positive action semantic. Think of general usage patterns, known to the user, like Add(), Remove(), Detach(). Example:
```
void Enable(bool e = true) { /*your code*/ }

void Disable() { return Enable(false); }

bool IsEnabled() const { /*your code*/ }
```
Upp code is both easier to read and to write because of little things like that.

- Implement small functions in class definition itself. Many functions only have 1 or 2 syntactically important statements (return *this doesn't count) and are maybe important to understand class function. A developer will first look in header file. Rule of thumb: >3 statements should go into own implementation in .cpp file. Example:
```
String Right(int count) const { return Mid(GetLength() - count); }
```

- Provide for Methods chaining in your class by returning *this, where logically useful. This makes it possible to myControl.HSizePos().VSizePos().Enable().

- Provide a String ToString() const; Users will thank you if they can inspect an object in a preformatted way with LOG(objectInstance); without having to setup own functions to show content during debug. Maybe it should be virtual to allow derived classes to extend it. This is heavily used in object design orientated languages like C#.

- Think of providing a unsigned GetHashValue() const; If it is logical, such a function can enable your object to directly be used as a Key in Upp containers like Index.

## Class layout

A class is made for someone to use it and should expose most important info first. So keep the class components' order as following: (unneeded or unused parts can be left out)

 
```
class tempc

    : public EmptyClass

    , public Any

{

    //FRIENDS

    friend class Ctrl;

    friend void Vector<int>::Clear();

 

public:

    /*

    * all handling data structures and typedefs nesseccary for the usage of public classes API go here

    */

 

    typedef unsigned HANDLE;

    typedef tempc CLASSNAME;

 

    struct SubStruct

    {}

    

    static const int AnIntConstant = 123;

    //static const float constf = 1.2f; //NOT POSSIBLE, arch dependant.

 

    //CTORS, DTORS, all concerning this class generation

    //INTERFACE, defines how to operate it

 

    //explicit

    tempc(int a = 0);

    virtual ~tempc() {}

 

    void AReadOnlyMethod() const { /*do something, bot dont change things in class*/ }

    void AWritableMethod() { /*here you can change your class state*/ }

 

    /*

    * Here follows the public available data, typically its place for a not close related members

    */

    

protected:

    /*

    * all handling data structures and typedefs interesting for the deriving classes go here

    */

 

    /*

    * Here follows the interface, deriving classes might be interested in, accessing it directly.

    */

    

    /*

    * Here follows the data, which mostly for performance reasons should be made available to derived classes

    */

    

private:

    /*

    * all handling data structures and typedefs which we use only internally, go here

    */

 

    /*

    * Here follows the private interface, which mostly are own helper classes and functions

    */

 

    /*

    * Here follows the private data, which only the class itself takes care of and none should ever know about.

    * in case of wrapper class this is mostly the objects that are wrapped

    */

};
```
 

## Readability

- The default code style in TheIDE is the first reference (Edit > Advanced > Format code in editor).

- Use Pascal Case for function names. (concatenated with capital letter at the beginning and on word boundaries, example:. void AddMemberElement(int a) ). Use small letters anywhere else, especially in class members' names. Don't use Hungarian style (Linus: "Brain damage") or things like 'm_myMember, _anotherMember, yetAnotherMember'. Example: Font font; Size textsize; Assist++ makes life so easy, that we do not need this to figure out if its a class member (Hit Ctrl+Space)

- Stick to implicit variable names semantic inside function bodies if possible and useful. i,j,k,m,n,q, u mostly are integer counters or indexes in arrays, p is probably a pointer, a,b,c are typically bool, d is double, f is a float. v is a Vector or a Value, s is a String etc.. this makes code less typographic and focuses more on the algorithm used. But don't use such variables exclusively, please. a1 a2 and a3 are quite the same at first look :)

- Tend to avoid comments in general, except for necessary hints on important sections, critical for understanding. instead, focus and pursue a good design structure in your package / class, which makes most comments disposable.

- Use Tabs to intend code, except in the header of a class, where many functions might have a single command body. This should be aligned with spaces (see container headers for example)

- Put spaces around operators, example: if(a > 12) { /*code*/ }

- The ampersand belongs to the object. example: Ctrl& GetCtrlRef();

- Usually put static variable instantiation, CH_STYLE and other housekeeping stuff at the bottom.

 
