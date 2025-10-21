#include <Core/Core.h>







void Upp01() {
	using namespace Upp;
	
	// 1. Vector
	{
		Vector<int> v = {5, 3, 1, 4, 2};
	
	
	
	    // Ascending order
	    Sort(v, StdLess<int>());
	    
	    
	    
	    
	    v.Add(6); // add function
	    v << 7; // operator
	    v.Add() = 8; // return value is reference
	    
	    v.Insert(1, 5);
	    
	    bool b = v.IsEmpty();
	    
	    v.Reserve(100);
	    
	    v[0] = 1;
	    v.Top() = 10;
	    
	    
	    v.SetCount(100, 5);
	    
	    v.Shrink();
	    
	    v.Remove(2);
	    v.Remove(1, 2); // count of 2
	    int value = v.Pop();
	    
	    int count = v.GetCount();
	    
	    v.Clear();
	}
	
	// 2. String
	{
		String s = "Hello";
		
		
		
		// Append
		s.Cat(' ');
		s << "World";
		s.Cat("!");
		
		
		
		// Insert
		s.Insert(1, 'X');
		s.Insert(3, "abc");
		
		bool bs = s.IsEmpty();
		
		s.Reserve(100);
		
		s.Set(0, 'h');
		s.Set(s.GetCount() - 1, '?');
		
		
		// Grow to length 100 with fill 'x'
		if(s.GetCount() < 100)
			s.Cat('x', 100 - s.GetCount());
		
		s.Shrink();
		
		s.Remove(2);
		s.Remove(1, 2); // count of 2
		int last = *s.Last();
		s.TrimLast();
		
		int scount = s.GetCount();
		
		s.Clear();
		
		
		int pos = s.Find("abc");
		pos = s.ReverseFind("abc");
		pos = s.FindFirstOf("abc");
		bool eq = s == "abc";
		s.Replace("abc", "123");
		
		
		
		WString widechar = s.ToWString();
		s = widechar.ToString();
		
		
		
		String begin = s.Left(3);
		String end = s.Right(3);
		String middle = s.Mid(3,2);
		
		
		s = TrimBoth(s); // removes blank characters from begin and end
		
		
		int a = StrInt("123");
		s = IntStr(a);
		
		
		String hex = Format("0x%08X", 123);
	}
	
}
