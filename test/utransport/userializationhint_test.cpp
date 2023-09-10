#include <gtest/gtest.h>
#include <include/utransport/USerializationHint.h>

using namespace uprotocol::utransport::datamodel;

// Test the USerializationHint enum values
TEST(USerializationHintTest, EnumValues) 
{
    EXPECT_EQ(static_cast<int>(USerializationHint::UNKNOWN), 0);
    EXPECT_EQ(static_cast<int>(USerializationHint::PROTOBUF), 1);
    EXPECT_EQ(static_cast<int>(USerializationHint::JSON), 2);
    EXPECT_EQ(static_cast<int>(USerializationHint::SOMEIP), 3);
    EXPECT_EQ(static_cast<int>(USerializationHint::RAW), 4);
}

// Test USerializationHintFromInt function
TEST(USerializationHintTest, FromInt) 
{
    EXPECT_EQ(USerializationHintFromInt(0), USerializationHint::UNKNOWN);
    EXPECT_EQ(USerializationHintFromInt(1), USerializationHint::PROTOBUF);
    // Add similar checks for other values
    // ...
    EXPECT_EQ(USerializationHintFromInt(4), USerializationHint::RAW);
    EXPECT_EQ(USerializationHintFromInt(5), std::nullopt);  // Expecting std::nullopt for out-of-range value
}

// Test USerializationHintFromString function
TEST(USerializationHintTest, FromString) 
{
    EXPECT_EQ(USerializationHintFromString(""), USerializationHint::UNKNOWN);
    EXPECT_EQ(USerializationHintFromString("application/x-protobuf"), USerializationHint::PROTOBUF);
    // Add similar checks for other values
    // ...
    EXPECT_EQ(USerializationHintFromString("application/octet-stream"), USerializationHint::RAW);
    EXPECT_EQ(USerializationHintFromString("invalid-value"), std::nullopt);  // Expecting std::nullopt for unknown value
}

// Test USerializationHintToString function
TEST(USerializationHintTest, ToString) 
{
    EXPECT_EQ(USerializationHintToString(USerializationHint::UNKNOWN), "");
    EXPECT_EQ(USerializationHintToString(USerializationHint::PROTOBUF), "application/x-protobuf");
    // Add similar checks for other values
    // ...
    EXPECT_EQ(USerializationHintToString(USerializationHint::RAW), "application/octet-stream");
    EXPECT_EQ(USerializationHintToString(static_cast<USerializationHint>(100)), std::nullopt);  // Expecting std::nullopt for undefined value
}

// Test USerializationHintToInt function
TEST(USerializationHintTest, ToInt)
{
    EXPECT_EQ(USerializationHintToInt(USerializationHint::UNKNOWN), 0);
    EXPECT_EQ(USerializationHintToInt(USerializationHint::PROTOBUF), 1);
    // Add similar checks for other values
    // ...
    EXPECT_EQ(USerializationHintToInt(USerializationHint::RAW), 4);
    EXPECT_EQ(USerializationHintToInt(static_cast<USerializationHint>(100)), std::nullopt);  // Expecting std::nullopt for undefined value
}

int main(int argc, char** argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
