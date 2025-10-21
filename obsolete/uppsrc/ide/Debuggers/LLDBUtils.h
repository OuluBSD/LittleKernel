#ifndef _ide_Debuggers_LLDBUtils_h_
#define _ide_Debuggers_LLDBUtils_h_

#include <Core/Core.h>

namespace Upp {

class ILLDBUtils {
public:
	virtual ~ILLDBUtils() = default;
	
	virtual String BreakRunning(int pid) = 0;
};

class LLDBUtilsFactory final {
public:
	One<ILLDBUtils> Create();
};

#if defined(PLATFORM_WIN32)

class LLDBWindowsUtils final : public ILLDBUtils {
public: /* ILLDBUtils */
	virtual String BreakRunning(int pid) override;
	
private:
	bool Is64BitIde() const;
	bool Is64BitProcess(HANDLE handle) const;
};

#elif defined(PLATFORM_POSIX)

class LLDBPosixUtils final : public ILLDBUtils {
public: /* ILLDBUtils */
	virtual String BreakRunning(int pid) override;
};

#endif

}

#endif
