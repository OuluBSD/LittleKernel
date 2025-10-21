// Minimal callbacks using std::function

using Callback = std::function<void()>;

template <class A>
using Callback1 = std::function<void(A)>;

template <class A, class B>
using Callback2 = std::function<void(A,B)>;

template <class A, class B, class C>
using Callback3 = std::function<void(A,B,C)>;

