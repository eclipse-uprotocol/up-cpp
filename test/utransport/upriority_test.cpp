#include <gtest/gtest.h>
#include <include/utransport/datamodel/UPriority.h>

using namespace uprotocol::utransport::datamodel;
// Test the UPriority enum values
TEST(UPriorityTest, EnumValues)
{
    EXPECT_EQ(static_cast<int>(UPriority::LOW), 0);
    EXPECT_EQ(static_cast<int>(UPriority::STANDARD), 1);
    EXPECT_EQ(static_cast<int>(UPriority::OPERATIONS), 2);
    EXPECT_EQ(static_cast<int>(UPriority::MULTIMEDIA_STREAMING), 3);
    EXPECT_EQ(static_cast<int>(UPriority::REALTIME_INTERACTIVE), 4);
    EXPECT_EQ(static_cast<int>(UPriority::SIGNALING), 5);
    EXPECT_EQ(static_cast<int>(UPriority::NETWORK_CONTROL), 6);
    EXPECT_EQ(static_cast<int>(UPriority::UNDEFINED), 7);
}

// Test UPriorityFromInt function
TEST(UPriorityTest, FromInt)
{
    EXPECT_EQ(UPriorityFromInt(0), UPriority::LOW);
    EXPECT_EQ(UPriorityFromInt(1), UPriority::STANDARD);
    // Add similar checks for other values
    // ...
    EXPECT_EQ(UPriorityFromInt(7), UPriority::UNDEFINED);
    EXPECT_EQ(UPriorityFromInt(8), std::nullopt);  // Expecting std::nullopt for out-of-range value
}

// Test UPriorityFromString function
TEST(UPriorityTest, FromString)
{
    EXPECT_EQ(UPriorityFromString("CS0"), UPriority::LOW);
    EXPECT_EQ(UPriorityFromString("CS1"), UPriority::STANDARD);
    // Add similar checks for other values
    // ...
    EXPECT_EQ(UPriorityFromString("CS6"), UPriority::NETWORK_CONTROL);
    EXPECT_EQ(UPriorityFromString("CS7"), std::nullopt);  // Expecting std::nullopt for unknown value
}

// Test UPriorityToString function
TEST(UPriorityTest, ToString)
{
    EXPECT_EQ(UPriorityToString(UPriority::LOW), "CS0");
    EXPECT_EQ(UPriorityToString(UPriority::STANDARD), "CS1");
    // Add similar checks for other values
    // ...
    EXPECT_EQ(UPriorityToString(UPriority::NETWORK_CONTROL), "CS6");
    EXPECT_EQ(UPriorityToString(UPriority::UNDEFINED), std::nullopt);  // Expecting std::nullopt for undefined value
}

// Test UPriorityToInt function
TEST(UPriorityTest, ToInt)
{
    EXPECT_EQ(UPriorityToInt(UPriority::LOW), 0);
    EXPECT_EQ(UPriorityToInt(UPriority::STANDARD), 1);
    // Add similar checks for other values
    // ...
    EXPECT_EQ(UPriorityToInt(UPriority::NETWORK_CONTROL), 6);
    EXPECT_EQ(UPriorityToInt(UPriority::UNDEFINED), std::nullopt);  // Expecting std::nullopt for undefined value
}

int main(int argc, char** argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}