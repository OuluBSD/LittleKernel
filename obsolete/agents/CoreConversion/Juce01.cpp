#ifdef HAVE_JUCE
#include <juce_core/juce_core.h>
using namespace juce;
#include <vector>
#include <algorithm>



void Juce01() {
	
	// 1. Vector
	{
	    Array<int> v;
	    v.addArray({5,3,1,4,2});
		
		
		
	    // Sort via std::sort on raw pointer range
	    std::sort(v.begin(), v.end());
		
		
		
		
	    v.add(6);
	    v.add(7);
	    v.add(8);
	
	    v.insert(1, 5);
	
	    bool b = v.isEmpty(); (void)b;
	
	    // No explicit reserve in juce::Array
	
	    v.set(0, 1);
	    v.set(v.size()-1, 10);
	    
	    // Resize with fill: grow manually
	    while(v.size() < 100) v.add(5);
	    // No shrink_to_fit; trimToSize reduces capacity to size
	    v.trimToSize();
	
	    v.remove(2);
	    v.removeRange(1, 2);
	    v.removeLast();
	
	    int count = v.size();
	
	    v.clear();
	}

    // 2. String
    {
        String s("Hello");

		// Append
		s += ' ';
		s += "World";
		s += '!';

		// Insert (no direct insert; rebuild around position)
		s = s.substring(0, 1) + "X" + s.substring(1);
		s = s.substring(0, 3) + "abc" + s.substring(3);

		bool bs = s.isEmpty();

		s.preallocateStorage(100);

		// Set first and last characters
		s = s.replaceSection(0, 1, "h");
		s = s.replaceSection(s.length() - 1, 1, "?");

		// Grow to length 100 with fill 'x'
		if(s.length() < 100)
			s = s.paddedRight('x', 100);

		// No explicit shrink on juce::String

		// Remove
		s = s.replaceSection(2, 1, "");
		s = s.replaceSection(1, 2, ""); // count of 2
		auto last = s.getLastCharacter();
		s = s.dropLastCharacters(1);

        int scount = s.length();

        s.clear();

        // Find / replace / compare
        int pos = s.indexOf("abc");
        pos = s.lastIndexOf("abc");
        pos = s.indexOfAnyOf("abc");
        bool eq = (s == String("abc")); (void)eq;

        s = s.replace("abc", "123");

        // Wide conversion round-trip
        std::wstring widechar(s.toWideCharPointer());
        s = String(widechar.c_str());

        // Substrings
        String begin = s.substring(0, 3);
        String end = s.substring(juce::jmax(0, s.length() - 3));
        String middle = s.substring(3, 5);

        // Trim both
        s = s.trim();

        int a = String("123").getIntValue();
        s = String(a);

        String hex = String::formatted("0x%08X", 123); (void)hex;
    }
}

#endif
