#include <iostream>
#include <optional>
#include "UpCoreApi.h"

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
#if 0
    {
        cout << "\n\n%%%%%%%%%%%%%%%%%%% UStatus" << endl;
        UStatus data;
        data.code(UCode::NOT_FOUND).message("hello");
        auto s = data.serialize();

        UStatus data2;
        data2.deserialize(s);
        cout << "####################### to_string" << endl;
        cout << data2.to_string() << endl;
        cout << "-----------------------" << endl;
        cout << "code = " << data2.code() << endl;
        cout << "message = " << data2.message() << endl;
    }

    {
        cout << "\n\n%%%%%%%%%%%%%%%%%%% UUID" << endl;
        UUID data;
        data.msb(100).lsb(200);
        auto s = data.serialize();

        UUID data2;
        data2.deserialize(s);
        cout << "####################### to_string" << endl;
        cout << data2.to_string() << endl;
        cout << "-----------------------" << endl;
        cout << "code = " << data2.msb() << endl;
        cout << "message = " << data2.lsb() << endl;
    }

    {
        cout << "\n\n%%%%%%%%%%%%%%%%%%% UResource" << endl;
        UResource data;
        data.name("hello").instance("instance").id(123);
        auto s = data.serialize();

        UResource data2;
        data2.deserialize(s);
        cout << "####################### to_string" << endl;
        cout << data2.to_string() << endl;
        cout << "-----------------------" << endl;
        cout << "name = " << data2.name() << endl;
        cout << "instance = " << data2.instance() << endl;
        cout << "message = " << data2.message() << endl;
        cout << "id = " << data2.id() << endl;
    }

    {
        cout << "\n\n%%%%%%%%%%%%%%%%%%% UEntity" << endl;
        UEntity data;
        data.name("uE").id(123).version_minor(55);
        auto s = data.serialize();

        UEntity data2;
        data2.deserialize(s);
        cout << "####################### to_string" << endl;
        cout << data2.to_string() << endl;
        cout << "-----------------------" << endl;
        cout << "name = " << data2.name() << endl;
        cout << "id = " << data2.id() << endl;
        cout << "version_major = " << data2.version_major() << endl;
        cout << "version_minor = " << data2.version_minor() << endl;
    }

    {
        cout << "\n\n%%%%%%%%%%%%%%%%%%% UAuthority" << endl;
        UAuthority data;
        data.name("uA");
        auto s = data.serialize();

        UAuthority data2;
        data2.deserialize(s);
        cout << "####################### to_string" << endl;
        cout << data2.to_string() << endl;
        cout << "-----------------------" << endl;
        cout << "name = " << data2.name() << endl;
    }

    if (0) {
        cout << "\n\n%%%%%%%%%%%%%%%%%%% UUri" << endl;
        UUri data;
        data.authority(UAuthority().name("uA")).entity(UEntity().name("uE")).resource(UResource().name("uR").message("msg"));
        auto s = data.serialize();

        UUri data2;
        data2.deserialize(s);
        cout << "####################### to_string" << endl;
        // cout << data2.to_string() << endl;
        // cout << "-----------------------" << endl;
        // cout << "name = " << data2.name() << endl;
    }
#endif
    {
        cout << "\n\n%%%%%%%%%%%%%%%%%%% UAttributes" << endl;
        UUID uuid;
        uuid.msb(100).lsb(200);

        UAttributes uattr;
        // uattr.id(UUID().msb(100).lsb(200));
        uattr.type(UMessageType::UMESSAGE_TYPE_REQUEST);
        uattr.priority(UPriority::UPRIORITY_CS3);
        uattr.ttl(1234).permission_level(4321).commstatus(UCode::NOT_FOUND).reqid(uuid);
        uattr.token("token");
        uattr.traceparent("traceparent");
        cout << "####################### from_string" << endl;
        cout << uattr.to_string() << endl;
        // data.authority(UAuthority().name("uA")).entity(UEntity().name("uE")).resource(UResource().name("uR").message("msg"));
        auto s = uattr.serialize();

        UAttributes data;
        data.deserialize(s);
        cout << "####################### to_string" << endl;
        cout << data.to_string() << endl;
        cout << "-----------------------" << endl;
        cout << "token = " << data.token() << endl;
        cout << "traceparent = " << data.traceparent() << endl;
    }
}
