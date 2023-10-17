#include <gtest/gtest.h>
#include <include/utransport/datamodel/UMessageType.h>

using namespace uprotocol::utransport;

// Test fixture for UMessageType enum and conversion functions
class UMessageTypeTest : public ::testing::Test 
{
    protected:
        // Test objects
        UMessageType publish = UMessageType::PUBLISH;
        UMessageType request = UMessageType::REQUEST;
        UMessageType response = UMessageType::RESPONSE;
        UMessageType undefined = UMessageType::UNDEFINED;
};

// Test UMessageType to int conversion
TEST_F(UMessageTypeTest, UMessageTypeToInt)
{
    EXPECT_EQ(UMessageTypeToInt(publish), 0);
    EXPECT_EQ(UMessageTypeToInt(request), 1);
    EXPECT_EQ(UMessageTypeToInt(response), 2);
    EXPECT_FALSE(UMessageTypeToInt(undefined).has_value());
}

// Test int to UMessageType conversion
TEST_F(UMessageTypeTest, IntToUMessageType)
{
    EXPECT_EQ(UMessageTypeFromInt(0), publish);
    EXPECT_EQ(UMessageTypeFromInt(1), request);
    EXPECT_EQ(UMessageTypeFromInt(2), response);
    EXPECT_FALSE(UMessageTypeFromInt(3).has_value());
}

// Test UMessageType to string conversion
TEST_F(UMessageTypeTest, UMessageTypeToString)
{
    EXPECT_EQ(UMessageTypeToString(publish), "pub.v1");
    EXPECT_EQ(UMessageTypeToString(request), "req.v1");
    EXPECT_EQ(UMessageTypeToString(response), "res.v1");
    EXPECT_FALSE(UMessageTypeToString(undefined).has_value());
}

// Test string to UMessageType conversion
TEST_F(UMessageTypeTest, StringToUMessageType)
{
    EXPECT_EQ(UMessageTypeFromString("pub.v1"), publish);
    EXPECT_EQ(UMessageTypeFromString("req.v1"), request);
    EXPECT_EQ(UMessageTypeFromString("res.v1"), response);
    EXPECT_FALSE(UMessageTypeFromString("invalid").has_value());
}

int main(int argc, char** argv) 
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
