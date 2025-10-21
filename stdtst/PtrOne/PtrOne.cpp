#include <iostream>
#include <Core/Core.h>
using namespace Upp;

static int fails = 0;
static void Check(const char* name, bool ok) { if(!ok){ std::cout << "FAIL: " << name << "\n"; ++fails; } }

struct Obj { int x=0; Obj(){} Obj(int xx):x(xx){} };

int main(){
    // One<T>
    One<Obj> a; Check("one empty", a.IsEmpty());
    a.Create(5); Check("one create", a->x == 5);
    a = nullptr; Check("one clear", a.IsEmpty());
    a = new Obj(7); Check("one attach", a->x == 7);
    One<Obj> b = One<Obj>::Make(9); Check("one make", b->x == 9);
    a.Swap(b); Check("one swap", a->x == 9 && b->x == 7);
    Obj* raw = a.Detach(); Check("one detach", a.IsEmpty() && raw->x == 9); delete raw;
    a.Create(10); Obj* rel = a.Release(); Check("one release", rel->x == 10); delete rel;

    // Ptr<T>
    Obj o{42}; Ptr<Obj> p(&o); Check("ptr access", p->x == 42);
    Check("ptr tostring", p.ToString().GetLength() > 0);
    p.Clear(); Check("ptr clear", p == nullptr);
    p = &o; Check("ptr assign", !p.IsEmpty() && p.Get() == &o);

    if(fails==0) std::cout << "OK\n"; return fails?1:0;
}

