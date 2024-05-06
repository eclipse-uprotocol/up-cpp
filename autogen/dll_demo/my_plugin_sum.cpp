#include <iostream>
#include "FactoryPlugin.hpp"
#include "my_plugin_api.hpp"

using namespace std;

string hello()
{
    return "hello";
}

#if 0
struct SessionImpl : public SessionApi {
    string name;

    SessionImpl(const std::string& arg)
    {
        cout << __PRETTY_FUNCTION__ << endl;
        name = arg;
    }

    ~SessionImpl() override
    {
        cout << __PRETTY_FUNCTION__ << endl;
    }

    float calculate() override
    {
        cout << __PRETTY_FUNCTION__ << endl;
        return 3.1416;
    }
};

struct PublisherImpl : public PublisherApi {
    PublisherImpl()
    {
        cout << __PRETTY_FUNCTION__ << endl;
    }

    ~PublisherImpl() override
    {
        cout << __PRETTY_FUNCTION__ << endl;
    }

    float calculate(std::shared_ptr<SessionApi>& session) override
    {
        cout << __PRETTY_FUNCTION__ << ' ' << dynamic_pointer_cast<SessionImpl>(session)->name << endl;
        return 3.1416;
    }
};


struct BoostPluginImpl : public BoostPlugin {
    std::shared_ptr<SessionApi> session_factory(const std::string& arg)
    {
        return make_shared<SessionImpl>(arg);
    }

     std::shared_ptr<PublisherApi> publisher_factory()
    {
        return make_shared<PublisherImpl>();
    }
};
#endif

Factories factories = {
    hello
};

FACTORY_EXPOSE(factories)

// extern "C" __attribute__((visibility("default")))
// void * get_factory()
// {
//     return (void*) &factories;
// }