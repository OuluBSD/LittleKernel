#ifdef HAVE_WX
 #include <wx/dynarray.h>
 #include <wx/string.h>
#include <vector>
#include <algorithm>




void Wx01() {
	
	// 1. Vector
	{
		// wxArrayInt is a common dynamic array type in wxWidgets
		wxArrayInt v;
		int init[] = {5,3,1,4,2};
		for(int x : init) v.Add(x);
		
		// Sort requires copying to std::vector or manual sort (wxArrayInt lacks sort)
		std::vector<int> tmp(v.begin(), v.end());
		std::sort(tmp.begin(), tmp.end());
		v.Clear();
		for(int x : tmp) v.Add(x);
		
		v.Add(6);
		v.Add(7);
		v.Add(8);
		
		v.Insert(5, 1);
		
		bool b = v.IsEmpty(); (void)b;
		
		// No explicit reserve
		
		v[0] = 1;
		v[v.size()-1] = 10;
		
		// No direct resize with fill; simulate grow
		while(v.size() < 100) v.Add(5);
		
		// No shrink_to_fit
		
		v.Erase(v.begin() + 2);
		v.Erase(v.begin() + 1, v.begin() + 3);
		v.RemoveAt(v.size()-1);
		
		int count = (int)v.size(); (void)count;
		
		v.Clear();
    }

    // 2. String
    {
        wxString s = "Hello";

        // Append
        s += ' ';
        s += "World";
        s += '!';

        // Insert
        s.insert(1, 1, 'X');
        s.insert(3, "abc");

        bool bs = s.IsEmpty();

        s.reserve(100);

        s[0] = 'h';
        s[s.size() - 1] = '?';

        // Grow to length 100 with fill 'x'
        if(s.size() < 100)
            s.append(100 - s.size(), 'x');

        s.shrink_to_fit();

        s.erase(2, 1);
        s.erase(1, 2); // count of 2
        auto last = s.back();
        s.erase(s.size() - 1, 1);

        size_t scount = s.size();

        s.clear();

        // Find / replace / compare
        long pos = (long)s.find("abc");
        pos = (long)s.rfind("abc");
        pos = (long)s.find_first_of("abc");
        bool eq = (s == wxString("abc")); (void)eq;

        s.Replace("abc", "123");

        // Wide conversion round-trip
        std::wstring widechar = s.ToStdWstring();
        s = wxString(widechar);

        // Substrings
        wxString begin = s.Left(3);
        wxString end = s.Right(3);
        wxString middle = s.Mid(3, 2);

        // Trim both
        s = s.Trim().Trim(false);

        long a = std::wcstol(L"123", nullptr, 10);
        s = wxString::Format("%ld", a);

        wxString hex = wxString::Format("0x%08X", 123); (void)hex;
    }
    
}

#endif
