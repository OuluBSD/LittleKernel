#ifdef flagMFC
#include <afx.h>
#include <afxstr.h>
#include <afxtempl.h>
#include <vector>
#include <algorithm>
using namespace ATL;



void Mfc01() {
	
	// 1. Vector
	{
	    // Using MFC CArray as an example container
	    CArray<int, int> v;
	    v.Add(5); v.Add(3); v.Add(1); v.Add(4); v.Add(2);
	
	
	    // No direct sort on CArray; copy to std::vector for sorting as a practical approach
	    std::vector<int> tmp;
	    for(int i = 0; i < v.GetSize(); i++) tmp.push_back(v[i]);
	    std::sort(tmp.begin(), tmp.end());
	    v.RemoveAll();
	    for(int x : tmp) v.Add(x);
	    v.Add(6);
	    v.Add(7);
	    v.Add(8);
	
	    v.InsertAt(1, 5);
	
	    BOOL b = v.IsEmpty();
	
	    // CArray has no explicit reserve
	
	    v[0] = 1;
	    v[v.GetSize()-1] = 10;
	
	
	    v.SetSize(100, 5);
	    
	    // No shrink
	
	    v.RemoveAt(2);
	    v.RemoveAt(1, 2);
	    v.RemoveAt(v.GetSize()-1); // pop
	
	    int count = (int)v.GetSize(); (void)count;
	
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

        BOOL bEmpty = s.IsEmpty();

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
        // ReverseFind only works for a single character; emulate substring reverse find
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
