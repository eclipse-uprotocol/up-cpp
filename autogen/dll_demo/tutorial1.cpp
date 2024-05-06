#include <iostream>
#include <memory>
#include "FactoryPlugin.hpp"
#include "my_plugin_api.hpp"

using namespace std;

int main(int argc, char* argv[])
{
    auto plugin = *new FactoryPlugin<Factories>(argv[1]);
    cout << plugin->hello() << endl;

    // auto plugin = make_shared<Plugin>("my_plugin_sum.so");

    // auto session = plugin->session_factory("hello");
    // auto publisher = plugin->publisher_factory();
    // cout << publisher->calculate(session) << endl;
}

