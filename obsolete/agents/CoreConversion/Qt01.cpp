#ifdef HAVE_QT
#include <QVector>
#include <QString>
#include <algorithm>
using namespace std;




void Qt01() {

    // 1.
    {
        QVector<int> v = {5, 3, 1, 4, 2};




        // Default ascending order
        sort(v.begin(), v.end());




        v.push_back(6);
        v.push_back(7);
        v.push_back(8);

        v.insert(v.begin() + 1, 5);

        bool b = v.isEmpty();
        
        v.reserve(100);

        v[0] = 1;
        v.back() = 10;
        

        v.resize(100, 5);
        v.squeeze(); // shrink capacity


        v.erase(v.begin() + 2);
        v.erase(v.begin() + 1, v.begin() + 3); // count of 3-1=2
        v.pop_back(); // void

        int count = v.size();

        v.clear();
    }
    
    // 2. String
    {
        QString s = "Hello";

        // Append
        s += ' ';
        s += "World";
        s += '!';

        // Insert
        s.insert(1, QChar('X'));
        s.insert(3, "abc");

        bool bs = s.isEmpty();

        s.reserve(100);

        s[0] = QChar('h');
        s[s.size() - 1] = QChar('?');

        // Grow to length 100 with fill 'x'
        if(s.size() < 100)
            s += QString(100 - s.size(), QChar('x'));

        s.squeeze(); // shrink capacity

        s.remove(2, 1);
        s.remove(1, 2); // count of 2
        QChar last = s.back();
        s.chop(1);

        int scount = s.size();

        s.clear();

        // Find / replace / compare
        int pos = s.indexOf("abc");
        pos = s.lastIndexOf("abc");
        // Find first of any in set "abc"
        {
            QString set = "abc";
            pos = -1;
            for(int i = 0; i < s.length(); ++i) { if(set.contains(s[i])) { pos = i; break; } }
        }
        bool eq = (s == QString("abc")); (void)eq;

        s.replace("abc", "123");

        // Wide conversion round-trip
        std::wstring widechar = s.toStdWString();
        s = QString::fromStdWString(widechar);

        // Substrings
        QString begin = s.left(3);
        QString end = s.right(3);
        QString middle = s.mid(3, 2);

        // Trim both
        s = s.trimmed();

        bool ok = false; int a = QString("123").toInt(&ok); (void)ok;
        s = QString::number(a);

        QString hex = QString("0x%1").arg(123, 8, 16, QLatin1Char('0')).toUpper(); (void)hex;
    }
    
}

#endif
