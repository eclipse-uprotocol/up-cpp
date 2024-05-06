#pragma once

#include <stdexcept>
#include <string>
#include <memory>
#include <functional>
#include <dlfcn.h>

template <typename FACT>
class FactoryPlugin {
    void* dl_handle;
public:
    FACT* factory;

    FactoryPlugin(const std::string& dll_path)
    {
        dl_handle = dlopen(dll_path.c_str(), RTLD_NOW|RTLD_LOCAL);
        if (dl_handle == nullptr) throw std::runtime_error("Cannot open plugin");
        void* ptr = dlsym(dl_handle, "get_factory__");
        if (ptr == nullptr) throw std::runtime_error("Cannot open plugin");
        auto get_factory = (void* (*)())ptr;
        factory = (FACT*) (*get_factory)();
    }

    ~FactoryPlugin()
    {
        if (dlclose(dl_handle) != 0) {
            using namespace std;
            cerr << "dlclose failed" << endl;;
        }
    }

    FACT* operator->() { return factory; }
};

#define _EXPOSE extern "C" __attribute__((visibility("default")))
#define FACTORY_EXPOSE(fact) _EXPOSE void * get_factory__() { return (void*) &fact; }