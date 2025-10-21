#ifdef flagWTL
#include <atlcoll.h>
#include <atlstr.h>
#include <vector>
#include <algorithm>




void Wtl01() {
	
	// 1. Vector
	{
	    // Using ATL CAtlArray as an example
	    CAtlArray<int> v;
	    int init[] = {5,3,1,4,2};
	    for(int x : init) v.Add(x);
	
	    // Sort ascending order
	    std::vector<int> tmp;
	    for(size_t i = 0; i < v.GetCount(); i++) tmp.push_back(v[(INT_PTR)i]);
	    std::sort(tmp.begin(), tmp.end());
	    v.RemoveAll();
	    for(int x : tmp) v.Add(x);
	
	    v.Add(6); v.Add(7); v.Add(8);
	    
	    
	    v.InsertAt(1, 5);
	
	    bool b = (v.GetCount() == 0);
	
	    // No reserve
	    
	    v[0] = 1;
	    v[v.GetCount()-1] = 10;
	
	
	    v.SetCount(100);
	    for(size_t i = tmp.size(); i < 100; ++i) v[(INT_PTR)i] = 5;
	
	
	    v.RemoveAt(2);
	    v.RemoveAt(1, 2);
	    v.RemoveAt(v.GetCount()-1);
	
	    int count = (int)v.GetCount();
	
	    v.RemoveAll();
    }

    // 2. String
    {
        CString s = _T("Hello");

        // Append
        s += _T(' ');
        s += _T("World");
        s += _T('!');

        // Insert
        s.Insert(1, _T('X'));
        s.Insert(3, _T("abc"));

        bool bs = s.IsEmpty() != FALSE;

        // Reserve-like
        s.GetBuffer(100);
        s.ReleaseBuffer();

        s.SetAt(0, _T('h'));
        s.SetAt(s.GetLength() - 1, _T('?'));

        // Grow to length 100 with fill 'x'
        if(s.GetLength() < 100)
            s += CString(_T('x'), 100 - s.GetLength());

        // No explicit shrink

        s.Delete(2, 1);
        s.Delete(1, 2); // count of 2
        TCHAR last = s[s.GetLength() - 1];
        s.Truncate(s.GetLength() - 1);

        int scount = s.GetLength();

        s.Empty();

        // Find / replace / compare
        int pos = s.Find(_T("abc"));
        // Reverse find substring emulate
        pos = -1; {
            int start = 0, last = -1;
            while(true) {
                int p = s.Find(_T("abc"), start);
                if(p < 0) break; last = p; start = p + 1;
            }
            pos = last;
        }
        pos = s.FindOneOf(_T("abc"));
        BOOL eq = (s == _T("abc")); (void)eq;

        s.Replace(_T("abc"), _T("123"));

        // Wide conversion round-trip
        CStringW widechar(s);
        s = CString(widechar);

        // Substrings
        CString begin = s.Left(3);
        CString end = s.Right(3);
        CString middle = s.Mid(3, 2);

        // Trim both
        s.Trim();

        int a = _ttoi(_T("123"));
        s.Format(_T("%d"), a);

        CString hex; hex.Format(_T("0x%08X"), 123); (void)hex;
    }
    
}

#endif
