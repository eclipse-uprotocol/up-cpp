#include <gtest/gtest.h>
#include <include/utransport/USerializationHint.h>

using namespace uprotocol::utransport::datamodel;

// Test the USerializationHint enum values
TEST(USerializationHintTest, EnumValues) 
{
    EXPECT_EQ(static_cast<int>(uprotocol::utransport::datamodel::USerializationHint::UNKNOWN), 0);
    EXPECT_EQ(static_cast<int>(uprotocol::utransport::datamodel::USerializationHint::PROTOBUF), 1);
    EXPECT_EQ(static_cast<int>(uprotocol::utransport::datamodel::USerializationHint::JSON), 2);
    EXPECT_EQ(static_cast<int>(uprotocol::utransport::datamodel::USerializationHint::SOMEIP), 3);
    EXPECT_EQ(static_cast<int>(uprotocol::utransport::datamodel::USerializationHint::RAW), 4);
}

// Test USerializationHintFromInt function
TEST(USerializationHintTest, FromInt) 
{
    EXPECT_EQ(uprotocol::utransport::datamodel::USerializationHintFromInt(0), uprotocol::utransport::datamodel::USerializationHint::UNKNOWN);
    EXPECT_EQ(uprotocol::utransport::datamodel::USerializationHintFromInt(1), uprotocol::utransport::datamodel::USerializationHint::PROTOBUF);
    // Add similar checks for other values
    // ...
    EXPECT_EQ(uprotocol::utransport::datamodel::USerializationHintFromInt(4), uprotocol::utransport::datamodel::USerializationHint::RAW);
    EXPECT_EQ(uprotocol::utransport::datamodel::USerializationHintFromInt(5), std::nullopt);  // Expecting std::nullopt for out-of-range value
}

// Test USerializationHintFromString function
TEST(USerializationHintTest, FromString) 
{
    EXPECT_EQ(uprotocol::utransport::datamodel::USerializationHintFromString(""), uprotocol::utransport::datamodel::USerializationHint::UNKNOWN);
    EXPECT_EQ(uprotocol::utransport::datamodel::USerializationHintFromString("application/x-protobuf"), uprotocol::utransport::datamodel::USerializationHint::PROTOBUF);
    // Add similar checks for other values
    // ...
    EXPECT_EQ(uprotocol::utransport::datamodel::USerializationHintFromString("application/octet-stream"), uprotocol::utransport::datamodel::USerializationHint::RAW);
    EXPECT_EQ(uprotocol::utransport::datamodel::USerializationHintFromString("invalid-value"), std::nullopt);  // Expecting std::nullopt for unknown value
}

// Test USerializationHintToString function
TEST(USerializationHintTest, ToString) 
{
    EXPECT_EQ(uprotocol::utransport::datamodel::USerializationHintToString(uprotocol::utransport::datamodel::USerializationHint::UNKNOWN), "");
    EXPECT_EQ(uprotocol::utransport::datamodel::USerializationHintToString(uprotocol::utransport::datamodel::USerializationHint::PROTOBUF), "application/x-protobuf");
    // Add similar checks for other values
    // ...
    EXPECT_EQ(uprotocol::utransport::datamodel::USerializationHintToString(uprotocol::utransport::datamodel::USerializationHint::RAW), "application/octet-stream");
    EXPECT_EQ(uprotocol::utransport::datamodel::USerializationHintToString(static_cast<uprotocol::utransport::datamodel::USerializationHint>(100)), std::nullopt);  // Expecting std::nullopt for undefined value
}

// Test USerializationHintToInt function
TEST(USerializationHintTest, ToInt)
{
    EXPECT_EQ(uprotocol::utransport::datamodel::USerializationHintToInt(uprotocol::utransport::datamodel::USerializationHint::UNKNOWN), 0);
    EXPECT_EQ(uprotocol::utransport::datamodel::USerializationHintToInt(uprotocol::utransport::datamodel::USerializationHint::PROTOBUF), 1);
    // Add similar checks for other values
    // ...
    EXPECT_EQ(uprotocol::utransport::datamodel::USerializationHintToInt(uprotocol::utransport::datamodel::USerializationHint::RAW), 4);
    EXPECT_EQ(uprotocol::utransport::datamodel::USerializationHintToInt(static_cast<uprotocol::utransport::datamodel::USerializationHint>(100)), std::nullopt);  // Expecting std::nullopt for undefined value
}

int main(int argc, char** argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
