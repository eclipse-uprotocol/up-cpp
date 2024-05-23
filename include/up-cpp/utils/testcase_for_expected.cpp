#include "Expected.h"
#include <cmath>
#include <iomanip>
#include <iostream>
#include <string_view>

enum class parse_error
{
    invalid_input,
    overflow
};

struct CompositeExpect {
    double x;
    double y;
};

using uprotocol::utils::Expected;
using uprotocol::utils::Unexpected;


using namespace std;

auto parse_number(std::string_view str) -> Expected<double, parse_error>
{
    const char* begin = str.data();
    char* end;
    double retval = std::strtod(begin, &end);
 
    if (begin == end)
        return Unexpected(parse_error::invalid_input);
    else if (std::isinf(retval))
        return Unexpected(parse_error::overflow);
 
    str.remove_prefix(end - begin);
    return retval;
}

auto parse_number_with_composite(std::string_view str) -> Expected<CompositeExpect, parse_error>
{
    const char* begin = str.data();
    char* end;
    double retval = std::strtod(begin, &end);
 
    if (begin == end)
        return Unexpected(parse_error::invalid_input);
    else if (std::isinf(retval))
        return Unexpected(parse_error::overflow);
 
    str.remove_prefix(end - begin);
    return CompositeExpect{retval, -retval};
}

int main()
{
    if (1) {
        auto process = [](const std::string_view& str)
        {
            std::cout << "value_or: " << parse_number(str).value_or(55) << std::endl;


            std::cout << "str: " << std::quoted(str) << ", ";
            if (const auto num = parse_number(str); num.has_value())
                std::cout << "value: " << *num << '\n';
                // If num did not have a value, dereferencing num
                // would cause an undefined behavior, and
                // num.value() would throw std::bad_expected_access.
                // num.value_or(123) uses specified default value 123.
            else if (num.error() == parse_error::invalid_input)
                std::cout << "error: invalid input\n";
            else if (num.error() == parse_error::overflow)
                std::cout << "error: overflow\n";
            else
                std::cout << "unexpected!\n"; // or invoke std::unreachable();
        };
    
        for (auto src : {"42", "42abc", "meow", "inf"})
            process(src);
    }
    cout << endl;
    try {
        const auto num = parse_number("5");
        cout << "Testing error() when not unexpected" << endl;
        auto err = num.error(); // should throw
    }
    catch (const std::runtime_error& ex) {
        cout << "caught runtime_error: " << ex.what() << endl;
    }
    cout << endl;
    try {
        const auto num = parse_number("inf");
        cout << "Testing value() when is unexpected." << endl;
        auto val = num.value(); // should throw
    }
    catch (const std::runtime_error& ex) {
        cout << "caught runtime_error: " << ex.what() << endl;
    }
    cout << endl;
    try {
        const auto num = parse_number("inf");
        cout << "Testing deref when unexpected." << endl;
        auto val = *num; // should throw
    }
    catch (const std::runtime_error& ex) {
        cout << "caught runtime_error: " << ex.what() << endl;
    }
    cout << endl;
    try {
        auto num = parse_number("inf");
        cout << "Testing deref when unexpected." << endl;
        auto val = *num; // should throw
    }
    catch (const std::runtime_error& ex) {
        cout << "caught runtime_error: " << ex.what() << endl;
    }
    cout << endl;

    {
        auto result = parse_number_with_composite("5");
        cout << "Testing composite when expected." << endl;
        cout << "x = " << result->x << " y = " << result->y << endl; // should not throw
    }

    try {
        auto result = parse_number_with_composite("inf");
        cout << "Testing composite when unexpected." << endl;
        cout << "x = " << result->x << " y = " << result->y << endl; // should throw
    }
    catch (const std::runtime_error& ex) {
        cout << "caught runtime_error: " << ex.what() << endl;
    }
    cout << endl;
}
