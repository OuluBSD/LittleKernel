#ifdef flagWINRT
#include <winrt/windows.foundation.c§ollections.h>
using namespace winrt;
using namespace Windows::Foundation::Collections;
#include <vector>
#include <algorithm>
#include <string>


void WinRT01() {
	
	
	// 1. Vector
	{
	    // Use single_threaded_vector for a simple IVector-like container
	    auto v = single_threaded_vector<int>({5,3,1,4,2});
	
	
	    // Sort by copying out, sorting, and copying back
	    std::vector<int> tmp(v.Size());
	    for(uint32_t i = 0; i < v.Size(); ++i) tmp[i] = v.GetAt(i);
	    std::sort(tmp.begin(), tmp.end());
	    v.Clear();
	    for(int x : tmp) v.Append(x);
	
	    v.Append(6); v.Append(7); v.Append(8);
	
	
	    v.InsertAt(1, 5);
	
	    bool b = (v.Size() == 0); (void)b;
	
	    // No reserve
	
	    v.SetAt(0, 1);
	    v.SetAt(v.Size()-1, 10);
	
	
	    while(v.Size() < 100) v.Append(5);
	
	
	
        v.RemoveAt(2);
	    v.RemoveAt(1); v.RemoveAt(1); // Remove range 1..2
	    v.RemoveAtEnd(); // pop
	
	    int count = (int)v.Size();
	
	    v.Clear();
    }

    // 2. String
    {
        // C++/WinRT strings are hstring; for mutation, use std::wstring as a builder.
        hstring s = L"Hello";

        // Append (via concatenation)
        s = s + L" " + L"World" + L"!";

        // Mutations via std::wstring
        std::wstring w = s.c_str();

        // Insert
        w.insert(1, 1, L'X');
        w.insert(3, L"abc");

        bool bs = w.empty();

        w.reserve(100);

        w[0] = L'h';
        w[w.size() - 1] = L'?';

        // Grow to length 100 with fill 'x'
        if(w.size() < 100)
            w.append(100 - w.size(), L'x');

        w.shrink_to_fit();

        w.erase(2, 1);
        w.erase(1, 2); // count of 2
        wchar_t last = w.back();
        w.pop_back();

        size_t scount = w.size();

        w.clear();
        s = hstring{ w }; // reflect into hstring

        // Find / replace / compare
        size_t pos = w.find(L"abc");
        pos = w.rfind(L"abc");
        pos = w.find_first_of(L"abc");
        bool eq = (w == std::wstring(L"abc")); (void)eq;

        // Replace all
        for(size_t p = 0; (p = w.find(L"abc", p)) != std::wstring::npos; p += 3)
            w.replace(p, 3, L"123");

        // Round-trip wide -> hstring stays same, but keep parity with pattern
        std::wstring widechar = w; (void)widechar;
        s = hstring{ w };

        // Substrings
        std::wstring begin = w.substr(0, std::min<size_t>(3, w.size())); (void)begin;
        std::wstring end = w.size() >= 3 ? w.substr(w.size() - 3) : w; (void)end;
        std::wstring middle = w.size() > 3 ? w.substr(3, std::min<size_t>(2, w.size() - 3)) : std::wstring(); (void)middle;

        // Trim both
        auto l = w.find_first_not_of(L" \t\n\r\f\v");
        auto r = w.find_last_not_of(L" \t\n\r\f\v");
        w = (l == std::wstring::npos) ? std::wstring() : w.substr(l, r - l + 1);

        int a = std::stoi(std::wstring(L"123"));
        w = std::to_wstring(a);

        wchar_t buf[32];
        swprintf(buf, 32, L"0x%08X", 123);
        std::wstring hex = buf; (void)hex;
    }
    
}



#endif