#include "Builder.hpp"

BuilderEnum::BuilderEnum(const std::variant<int,std::string>& arg, const Table& enum_map)
{
    if (std::holds_alternative<int>(arg)) {
        auto down_value = std::get<int>(arg);
        for (const auto& rec : enum_map) {
            if (down_value == rec.value) {
                inner_value = rec.inner_value;
                return;
            }
        }
    }
    else if (std::holds_alternative<std::string>(arg)) {
        auto down_value = std::get<std::string>(arg);
        for (const auto& rec : enum_map) {
            if (down_value == rec.name) {
                inner_value = rec.inner_value;
                return;
            }
        }
    }
    throw std::runtime_error("Cannot map required enum to protobuf");
}

BuilderEnum::BuilderEnum(int inner_value, const Table& enum_map)
{
    for (const auto& rec : enum_map) {
        if (inner_value == rec.inner_value) {
            value = rec.value;
            name = rec.name;
            return;
        }
    }
    throw std::runtime_error("Cannot map protobuf enum value to external type");
}
