# AI-Log
Talking to AI is sometimes an art. We'll add here texts that were written by hand and that have made changes to the code. You don't have to include everything.




#### 14.9.2025 by Seppo Pakonen:
Add to "/AGENTS.md" that ".cpp", ".icpp" and ".c" files of a package (with .upp file) must try to include only the mainheader of the package relatively '#include "PackageName.h"'. The reason is that we use custom header pre compilation, which is not too smart and that helps a lot. It's the U++'s BLITZ. Other includes can be added after that if the implementation needs to forward-include something from later packages (in dependency queue). So every package must also have that "PackageName.h" header, and that's always the main header which includes all other headers in the package, and it shouldn't have much else for clarity (but there's some exceptions for small packages etc.). This is very important. Also we prefer, that other than the main header doesn't have namespace in them, but the main header includes them inside "namespace Upp" scope which is made using NAMESPACE_UPP macro. We avoid clutter in that way, and BLITZ works slightly better too. This is like a "no-exceptions" pattern, which AI easily doesn't do correctly, because other c++ projects and conventions doesn't have this. Also, if you have subpackages like "AI", "AI/Core", "AI/Core/Core"; then subpackages are always independent packages, and not somehow gathered in parent-package (like .h files aren't somehow cross-included). Parent package may include only the main header of sub-package. Sub-packages must chain them correctly including previous sub-package or other packages (without circular dependency). Most likely, sub-package shouldn't include parent package, but it can be other way around, if sub-package really is an extension to the parent-package.


#### 25.9.2025 by Seppo Pakonen:
Today, we are going to make a huge rewrite of VfsValue related code.
 Some background:
 - The IDE had a very primitive way of storing data after parsing libclang, and it required a data structure. This structure evolved over time and became AstNode, from which it was derived as VfsValue+AstValue(in VfsValue::value variable).
 - So VfsValue can have Upp::Value (and AstValue) or a object which has a class which inherits the VfsValueExt.
 - VfsValue is "Core code" and it has similar classes in GUI side, "Ctrl code". If so VfsValueExt inheriting code should have VfsValueExtCtrl inheriting code.
 - Vfs & VfsValue code is a little bit everywhere now, and that's bad
 - "uppsrc/Core2" package could be split to multiple "uppsrc/Core/*" packages. Same goes for "uppsrc/Vfs2" (-> "uppsrc/Vfs/*").
- no documentation was ever written, and code wasn't well commented. All code is by me BTW.
 - we have a special problem in serialization and storage, because there have been and probably still is a need to link VfsValue to a source file, but all those VfsValues most be merged to one tree (and then un-merged for saving/storing).
 - so the merging and unmerging VfsValue-trees has been messy. We should consider if it's even necessary (un-merging part). Could we keep VfsValues separate and merge them virtually (treat them as multiple overlays for root).
     - I'd probably rather keep multiple potential sources for a node... and that might be more correct technically too.
     - the total overlay + virtual merge was too difficult for me previously


