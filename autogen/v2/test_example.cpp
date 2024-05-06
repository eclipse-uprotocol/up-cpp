#include <iostream>
#include "example.hpp"

using namespace std;

template <typename T>
ostream& operator<<(ostream& os, const optional<T>& arg)
{
    if (arg) os << *arg;
    else os << "null";
    return os;
}

int main(int argc, char *argv[])
{
    // X root;
    // root.i(6).s("Goodbye"); // .os("Optional");
    // cout << root.i_get() << endl;
    // cout << root.s_get() << endl;
    // // cout << root.os_get() << endl;

    Y root;
    // root.i(5).s("Hello").os("Outer").x().i(6).s("Goodbye").os("Optional").e("two");
    root.i(5).s("outer").x().i(6).s("inner");
    cout << "i=" << root.i_get() << endl;
    cout << "s=" << root.s_get() << endl;
    // cout << "os=" << root.os_get() << endl;
    cout << "x.i=" << root.x().i_get() << endl;
    cout << "x.s=" << root.x().s_get() << endl;
    // cout << "x.os=" << root.x().os_get() << endl;
    // cout << "x.e=" << root.x().e_get().name << endl;
    cout << endl;
    cout << "####### dump root" << endl;
    cout << root.debug() << endl;

    // auto saved = root.x();
    // cout << saved.i_get() << endl;
    // cout << saved.s_get() << endl;
}