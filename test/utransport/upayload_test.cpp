#include <gtest/gtest.h>
#include <uprotocol/transport/datamodel/UPayload.h>

using namespace uprotocol::utransport;

// Test fixture for UPayload class
class UPayloadTest : public ::testing::Test
{
protected:
    // Test objects
    const uint8_t* testData = reinterpret_cast<const uint8_t*>("Hello, World!");
    const size_t testDataSize = 13;
    uprotocol::utransport::UPayload payload;

    UPayloadTest() : payload(testData, testDataSize) {}
};

// Test the data() method
TEST_F(UPayloadTest, Data)
{
    EXPECT_EQ(payload.data(), testData);
}

// Test the size() method
TEST_F(UPayloadTest, Size)
{
    EXPECT_EQ(payload.size(), testDataSize);
}

// Test the isEmpty() method with non-empty payload
TEST_F(UPayloadTest, IsEmptyFalse)
{
    EXPECT_FALSE(payload.isEmpty());
}

// Test the isEmpty() method with an empty payload
TEST(UPayloadTest, IsEmptyTrue) 
{
    uprotocol::utransport::UPayload emptyPayload(nullptr, 0);
    EXPECT_TRUE(emptyPayload.isEmpty());
}

int main(int argc, char** argv) 
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}