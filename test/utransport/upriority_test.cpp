#include <gtest/gtest.h>
#include <include/utransport/UPriority.h>

using namespace uprotocol::utransport::datamodel;
// Test the UPriority enum values
TEST(UPriorityTest, EnumValues)
{
    EXPECT_EQ(static_cast<int>(uprotocol::utransport::datamodel::UPriority::LOW), 0);
    EXPECT_EQ(static_cast<int>(uprotocol::utransport::datamodel::UPriority::STANDARD), 1);
    EXPECT_EQ(static_cast<int>(uprotocol::utransport::datamodel::UPriority::OPERATIONS), 2);
    EXPECT_EQ(static_cast<int>(uprotocol::utransport::datamodel::UPriority::MULTIMEDIA_STREAMING), 3);
    EXPECT_EQ(static_cast<int>(uprotocol::utransport::datamodel::UPriority::REALTIME_INTERACTIVE), 4);
    EXPECT_EQ(static_cast<int>(uprotocol::utransport::datamodel::UPriority::SIGNALING), 5);
    EXPECT_EQ(static_cast<int>(uprotocol::utransport::datamodel::UPriority::NETWORK_CONTROL), 6);
    EXPECT_EQ(static_cast<int>(uprotocol::utransport::datamodel::UPriority::UNDEFINED), 7);
}

// Test UPriorityFromInt function
TEST(UPriorityTest, FromInt)
{
    EXPECT_EQ(uprotocol::utransport::datamodel::UPriorityFromInt(0), uprotocol::utransport::datamodel::UPriority::LOW);
    EXPECT_EQ(uprotocol::utransport::datamodel::UPriorityFromInt(1), uprotocol::utransport::datamodel::UPriority::STANDARD);
    // Add similar checks for other values
    // ...
    EXPECT_EQ(uprotocol::utransport::datamodel::UPriorityFromInt(7), uprotocol::utransport::datamodel::UPriority::UNDEFINED);
    EXPECT_EQ(uprotocol::utransport::datamodel::UPriorityFromInt(8), std::nullopt);  // Expecting std::nullopt for out-of-range value
}

// Test UPriorityFromString function
TEST(UPriorityTest, FromString)
{
    EXPECT_EQ(uprotocol::utransport::datamodel::UPriorityFromString("CS0"), uprotocol::utransport::datamodel::UPriority::LOW);
    EXPECT_EQ(uprotocol::utransport::datamodel::UPriorityFromString("CS1"), uprotocol::utransport::datamodel::UPriority::STANDARD);
    // Add similar checks for other values
    // ...
    EXPECT_EQ(uprotocol::utransport::datamodel::UPriorityFromString("CS6"), uprotocol::utransport::datamodel::UPriority::NETWORK_CONTROL);
    EXPECT_EQ(uprotocol::utransport::datamodel::UPriorityFromString("CS7"), std::nullopt);  // Expecting std::nullopt for unknown value
}

// Test UPriorityToString function
TEST(UPriorityTest, ToString)
{
    EXPECT_EQ(uprotocol::utransport::datamodel::UPriorityToString(uprotocol::utransport::datamodel::UPriority::LOW), "CS0");
    EXPECT_EQ(uprotocol::utransport::datamodel::UPriorityToString(uprotocol::utransport::datamodel::UPriority::STANDARD), "CS1");
    // Add similar checks for other values
    // ...
    EXPECT_EQ(uprotocol::utransport::datamodel::UPriorityToString(uprotocol::utransport::datamodel::UPriority::NETWORK_CONTROL), "CS6");
    EXPECT_EQ(uprotocol::utransport::datamodel::UPriorityToString(uprotocol::utransport::datamodel::UPriority::UNDEFINED), std::nullopt);  // Expecting std::nullopt for undefined value
}

// Test UPriorityToInt function
TEST(UPriorityTest, ToInt)
{
    EXPECT_EQ(uprotocol::utransport::datamodel::UPriorityToInt(uprotocol::utransport::datamodel::UPriority::LOW), 0);
    EXPECT_EQ(uprotocol::utransport::datamodel::UPriorityToInt(uprotocol::utransport::datamodel::UPriority::STANDARD), 1);
    // Add similar checks for other values
    // ...
    EXPECT_EQ(uprotocol::utransport::datamodel::UPriorityToInt(uprotocol::utransport::datamodel::UPriority::NETWORK_CONTROL), 6);
    EXPECT_EQ(uprotocol::utransport::datamodel::UPriorityToInt(uprotocol::utransport::datamodel::UPriority::UNDEFINED), std::nullopt);  // Expecting std::nullopt for undefined value
}

int main(int argc, char** argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}