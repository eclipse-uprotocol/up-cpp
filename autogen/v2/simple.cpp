#include <iostream>
#include <vector>
#include "pfiles/myidl.pb.h"

using namespace std;

int main(int argc, char *argv[])
{
    auto root = new tutorial::X();
    root->set_i(5);
    root->set_s("hello");
    root->set_e(tutorial::InnerEnum::three);
    root->set_ad(3.1416);
    cout << "got root" << endl;

    cout << "debug =" << root->DebugString() << endl;
    cout << "---------------" << endl;

    auto reflection = root->GetReflection();
    cout << "got reflection" << endl;
    vector<const google::protobuf::FieldDescriptor*> data;
    reflection->ListFields(*root, &data);
    for (auto field : data) {
        cout << "field" << endl;
    }
}