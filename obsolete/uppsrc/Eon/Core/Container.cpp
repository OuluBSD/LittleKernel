#include "Core.h"

#if defined __GNUG__ && (defined flagGCC || defined flagCLANG)
	#include <cxxabi.h>
	#include <memory>
#endif

NAMESPACE_UPP

bool& EnvState::SetBool(int key, bool b) {
	Value& o = data.GetAdd(key);
	auto& v = CreateRawValue<bool>(o);
	v = b;
	return v;
}

int& EnvState::SetInt(int key, int i) {
	Value& o = data.GetAdd(key);
	auto& v = CreateRawValue<int>(o);
	v = i;
	return v;
}

bool& EnvState::GetBool(int key) {
	Value& o = data.GetAdd(key);
	if (!o.Is<bool>())
		CreateRawValue<bool>(o) = false;
	return const_cast<bool&>(o.To<bool>());
}

int& EnvState::GetInt(int key) {
	Value& o = data.GetAdd(key);
	if (!o.Is<int>())
		o = CreateRawValue<int>(o) = 0;
	return const_cast<int&>(o.To<int>());
}

String Demangle(const char* name) {
	#if defined __GNUG__ && (defined flagGCC || defined flagCLANG)
	int status = -4; // some arbitrary value to eliminate the compiler warning

    // enable c++11 by passing the flag -std=c++11 to g++
    std::unique_ptr<char, void(*)(void*)> res {
        abi::__cxa_demangle(name, NULL, NULL, &status),
        std::free
    };

    return (status==0) ? res.get() : name ;
    #endif
    return name;
}

END_UPP_NAMESPACE
