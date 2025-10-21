#include <vector>
#include <algorithm> // sort
#include <string>
#include <cstdio>
#include <cctype>






void Std01() {
    using namespace std;
    
    // 1.
    {
		vector<int> v = {5, 3, 1, 4, 2};
	
	
	
	    // Default ascending order
	    sort(v.begin(), v.end());
	    
	    
	    
	    
	    v.push_back(6);
	    v.push_back(7); // no operator
	    v.push_back(8); // can't add without value & void
	    
	    v.insert(v.begin() + 1, 5);
	    
	    bool b = v.empty();
	    
	    v.reserve(100);
	    
	    v[0] = 1;
	    v.back() = 10;
	    
	    
	    v.resize(100, 5);
	    
	    v.shrink_to_fit();
	    
	    v.erase(v.begin() + 2);
	    v.erase(v.begin() + 1, v.begin() + 3); // count of 3-1=2
	    v.pop_back(); // void
	    
	    int count = v.size();
	    
        v.clear();
    }
    
    // 2. String
    {
        string s = "Hello";

        // Append
        s.push_back(' ');
        s += "World";
        s.push_back('!');

        // Insert
        s.insert(1, 1, 'X');
        s.insert(3, "abc");

        bool bs = s.empty();

        s.reserve(100);

        s[0] = 'h';
        s.back() = '?';

        // Grow to length 100 with fill 'x'
        if(s.size() < 100)
            s.append(100 - s.size(), 'x');

        s.shrink_to_fit();

        s.erase(2, 1);
        s.erase(1, 2); // count of 2
        char last = s.back();
        s.pop_back();

        size_t scount = s.size();

        s.clear();

        // Find / replace / compare
        size_t pos_sz = s.find("abc");
        pos_sz = s.rfind("abc");
        pos_sz = s.find_first_of("abc");
        bool eq = (s == string("abc")); (void)eq;

        // Replace all occurrences of "abc" with "123"
        for(size_t p = 0; (p = s.find("abc", p)) != string::npos; p += 3)
            s.replace(p, 3, "123");

        // Wide conversion round-trip
        std::wstring widechar(s.begin(), s.end());
        s = string(widechar.begin(), widechar.end());

        // Substrings
        string begin = s.substr(0, std::min<size_t>(3, s.size()));
        string end = s.size() >= 3 ? s.substr(s.size() - 3) : s;
        string middle = s.size() > 3 ? s.substr(3, std::min<size_t>(2, s.size() - 3)) : string();

        // Trim both (ASCII whitespace)
        auto l = s.find_first_not_of(" \t\n\r\f\v");
        auto r = s.find_last_not_of(" \t\n\r\f\v");
        s = (l == string::npos) ? string() : s.substr(l, r - l + 1);

        int a = std::stoi("123");
        s = std::to_string(a);

        char buf[32];
        std::snprintf(buf, sizeof(buf), "0x%08X", 123);
        string hex = buf; (void)hex;
    }
    
}
