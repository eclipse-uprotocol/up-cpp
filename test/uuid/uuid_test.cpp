#include <gtest/gtest.h>

#include "uuid.h"

using namespace uprotocol::uuid;

//UUID constructors, toByteArray(),getTime(), getCount(),toString()
TEST(UUIDTest, Class)
{
    UUID uuId;

    UUID uuIdFromByteArr(uuId.toByteArray().data());
    EXPECT_TRUE(uuId.getTime() == uuIdFromByteArr.getTime());
    EXPECT_TRUE(uuId.getCount() == uuIdFromByteArr.getCount());

    std::string str = "0080b636-8303-8701-8ebe-7a9a9e767a9f";
    UUID uuIdNew(str);
    EXPECT_EQ(uuIdNew.toString(), str);
}

TEST(UUIDTest, Negative)
{
    std::string str = "0080b636-8303-8701-8ebe-7a9a9e767a9f";
    UUID uuIdNew(str);
    str = "test" +str;
    EXPECT_NE(uuIdNew.toString(), str);
}

int main(int argc, char **argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}